// Fill out your copyright notice in the Description page of Project Settings.


#include "Connection/UDPClient.h"
#include "Networking.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "HAL/RunnableThread.h"
#include "Connection/UDPSendRunnable.h"
#include "ConnectionGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "UI/LogWidget.h"
#include "Components/CanvasPanelSlot.h"


AUDPClient::AUDPClient()
{
	static ConstructorHelpers::FClassFinder<ULogWidget> LogWidgetClassRef(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/WBP_LogWidget.WBP_LogWidget_C'"));
	if (LogWidgetClassRef.Succeeded())
	{
		LogWidgetClass = LogWidgetClassRef.Class;
	}

	PrimaryActorTick.bCanEverTick = false;
	ClientSocket = nullptr;
	SendRunnableThread = nullptr;

	ServerIP = TEXT("192.168.0.106");
	ServerPort = 8001;
}

void AUDPClient::BeginPlay()
{
	Super::BeginPlay();


	CreateSocket();
	StartClientThread();

	if (LogWidgetClass)
	{
		LogWidget = CreateWidget<ULogWidget>(GetWorld(), LogWidgetClass);
		LogWidget->AddToViewport();
	}

}



void AUDPClient::SendMessageToThread(FString InMessage)
{
	// �������Ʈ���� ȣ��
	if (SendRunnable)
	{
		SendRunnable->SendedMessage(InMessage);
	}
}

void AUDPClient::CreateSocket()
{
	// Ŭ���̾�Ʈ ���� �ʱ�ȭ
	//ClientSocket = FUdpSocketBuilder(TEXT("UDPClientSocket"))
	//    .AsNonBlocking()
	//    .AsReusable()
	//    .WithSendBufferSize(2 * 1024 * 1024);

	// RemoteAddr ���� �� ����
	ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	ClientSocket = SocketSubsystem->CreateSocket(NAME_DGram, TEXT("UDP_SOCKET"), true);
	RemoteAddr = SocketSubsystem->CreateInternetAddr(); //ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	ClientSocket->SetBroadcast(true);
	ClientSocket->SetReuseAddr(true);


	FIPv4Address IP;
	FIPv4Address::Parse(ServerIP, IP);
	FIPv4Endpoint Endpoint(IP, ServerPort);
	RemoteAddr = SocketSubsystem->CreateInternetAddr();
	RemoteAddr->SetIp(IP.Value);
	RemoteAddr->SetPort(ServerPort);

	if (ClientSocket)
	{
		const FString Message = TEXT("Hello");
		int32 BytesSent = 0;
		ClientSocket->SendTo((uint8*)TCHAR_TO_UTF8(*Message), Message.Len(), BytesSent, *RemoteAddr);
	}
}

void AUDPClient::StartClientThread()
{
	SendRunnable = MakeShared<FUDPSendRunnable>(ClientSocket, RemoteAddr);
	SendRunnableThread = FRunnableThread::Create(SendRunnable.Get(), TEXT("UDPClientRunnableThread"), 0, TPri_BelowNormal);
	SendRunnable->Owner = this;

	ReceiveRunnable = MakeShared<FUDPReceiveRunnable>(ClientSocket, RemoteAddr, GetWorld());
	ReceiveRunnableThread = FRunnableThread::Create(ReceiveRunnable.Get(), TEXT("UDPServerRunnableThread"), 0, TPri_BelowNormal);
	ReceiveRunnable->Owner = this;
}


void AUDPClient::StopClient()
{
	bClientRunning = false;
	if (SendRunnableThread)
	{
		SendRunnableThread->Kill(true);
		delete SendRunnableThread;
		SendRunnableThread = nullptr;
	}

	if (ClientSocket)
	{
		ClientSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ClientSocket);
		ClientSocket = nullptr;
	}
}

void AUDPClient::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopClient();
	Super::EndPlay(EndPlayReason);
}
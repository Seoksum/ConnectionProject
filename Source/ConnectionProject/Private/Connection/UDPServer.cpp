// Fill out your copyright notice in the Description page of Project Settings.


#include "Connection/UDPServer.h"
#include "Networking.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "HAL/RunnableThread.h"
#include "Connection/UDPReceiveRunnable.h"


// Sets default values
AUDPServer::AUDPServer()
{
    PrimaryActorTick.bCanEverTick = false;

    ServerIP = TEXT("0.0.0.0");  // 0.0.0.0���� �����Ͽ� ��� IP���� ���� ����
    ServerPort = 8001;
}

// Called when the game starts or when spawned
void AUDPServer::BeginPlay()
{
    Super::BeginPlay();

    CreateSocket();
    StartServerThread();
}

void AUDPServer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    StopServer();
    Super::EndPlay(EndPlayReason);
}

void AUDPServer::CreateSocket()
{
    // ���� ���� �ʱ�ȭ
    FIPv4Address IPAddress;
    FIPv4Address::Parse(ServerIP, IPAddress);
    FIPv4Endpoint Endpoint(IPAddress, ServerPort);

    Socket = FUdpSocketBuilder(TEXT("UDPServerSocket"))
        .AsNonBlocking()
        .AsReusable()
        .WithBroadcast()   
        .BoundToEndpoint(Endpoint)
        .WithReceiveBufferSize(2 * 1024 * 1024)
        .Build();

    if (Socket)
    {
        LocalAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
        bool bIsValid;
        LocalAddr->SetIp(*ServerIP, bIsValid);
        LocalAddr->SetPort(ServerPort);
    }

   
}

void AUDPServer::StartServerThread()
{
    // UDPServerRunnable ���� �� ������ ����
    ServerRunnable = MakeShared<FUDPReceiveRunnable>(Socket, LocalAddr, GetWorld());
    //ServerRunnable = new FUDPServerRunnable(Socket, LocalAddr);
    ServerThread = FRunnableThread::Create(ServerRunnable.Get(), TEXT("UDPServerRunnableThread"), 0, TPri_BelowNormal);
}

void AUDPServer::StopServer()
{
    //bServerRunning = false;
    if (ServerThread)
    {
        ServerRunnable->Stop();
        ServerThread->Kill(true);
        delete ServerThread;
        ServerThread = nullptr;
    }

    if (Socket)
    {
        Socket->Close();
        ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
        Socket = nullptr;
    }
}

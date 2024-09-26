// Fill out your copyright notice in the Description page of Project Settings.

#include "Connection/UDPSendRunnable.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Common/UdpSocketBuilder.h"
#include "HAL/Runnable.h"
#include "ConnectionGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "UI/LogWidget.h"
#include "Connection/UDPClient.h"


FUDPSendRunnable::FUDPSendRunnable(FSocket* InSocket, TSharedPtr<FInternetAddr> InRemoteAddr)
    : Socket(InSocket), RemoteAddr(InRemoteAddr), Thread(nullptr), bStopping(false)
{
    Thread = FRunnableThread::Create(this, TEXT("UDPClientRunnableThread"), 0, TPri_BelowNormal);

}

FUDPSendRunnable::~FUDPSendRunnable()
{
    Stop();
}

bool FUDPSendRunnable::Init()
{

    return (Socket != nullptr && RemoteAddr.IsValid());
}

uint32 FUDPSendRunnable::Run()
{
    while (!bStopping)
    {
        // 데이터 수신 및 처리 로직
        if (!MessageToSend.IsEmpty())
        {
            // 메시지가 있는 경우 데이터 전송
            SendData(MessageToSend);
            MessageToSend.Empty(); // 메시지 초기화
        }

        FPlatformProcess::Sleep(0.5f); // 주기적 대기
    }
                       
    return 0;
}

void FUDPSendRunnable::Stop()
{
    bStopping = true;
}

void FUDPSendRunnable::SendData(const FString& DataToSend)
{
    FTCHARToUTF8 Convert(*DataToSend);
    int32 BytesSent = 0;

    bool bDidSend = Socket->SendTo((uint8*)TCHAR_TO_UTF8(*DataToSend), DataToSend.Len(), BytesSent, *RemoteAddr);
    WantToLog = DataToSend;

    //bool bDidSend = Socket->SendTo((uint8*)Convert.Get(), Convert.Length(), BytesSent, *RemoteAddr);
}

void FUDPSendRunnable::SendedMessage(const FString& InMessageToSend)
{
    MessageToSend = InMessageToSend;
}


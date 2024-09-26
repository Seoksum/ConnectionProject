// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "Sockets.h"
#include "Networking.h"

/**
 * 
 */

struct FClientInfo
{
    FIPv4Address IPAddress;  // 클라이언트 IP 주소
    int32 Port;              // 클라이언트 포트
    FDateTime LastReceived;     // 마지막으로 메시지를 받은 시간
};

class CONNECTIONPROJECT_API FUDPReceiveRunnable : public FRunnable
{
    // FUDPServerRunnable -> 스레드 클래스. 
     // 데이터 송수신과 같은 네트워크 작업을 비동기적으로 처리(SendData, ReceiveData)

public:

    FUDPReceiveRunnable(FSocket* InSocket, TSharedPtr<FInternetAddr> InLocalAddr, UWorld* InWorld);
    virtual ~FUDPReceiveRunnable();

    virtual bool Init() override;
    virtual uint32 Run() override;
    virtual void Stop() override;

private:
    FSocket* Socket;
    TSharedPtr<FInternetAddr> LocalAddr;
    FRunnableThread* Thread;
    FThreadSafeBool bStopping;

    void ReceiveData();

    void SaveToJson(const FString& ReceivedMessage, UWorld* MyWorld);

    TWeakObjectPtr<UWorld> World;

    FString ReceivedString;

protected:

    int32 MinimumStringLen;

    TMap<FString, FClientInfo> Clients;

    void OnReceiveClientMessage(const FIPv4Address& ClientIP, int32 ClientPort);

public:

    class AUDPClient* Owner;


};

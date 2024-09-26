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
    FIPv4Address IPAddress;  // Ŭ���̾�Ʈ IP �ּ�
    int32 Port;              // Ŭ���̾�Ʈ ��Ʈ
    FDateTime LastReceived;     // ���������� �޽����� ���� �ð�
};

class CONNECTIONPROJECT_API FUDPReceiveRunnable : public FRunnable
{
    // FUDPServerRunnable -> ������ Ŭ����. 
     // ������ �ۼ��Ű� ���� ��Ʈ��ũ �۾��� �񵿱������� ó��(SendData, ReceiveData)

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

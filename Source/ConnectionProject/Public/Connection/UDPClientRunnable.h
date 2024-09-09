// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"

/**
 * 
 */
class CONNECTIONPROJECT_API FUDPClientRunnable : public FRunnable
{
    // FUDPClientRunnable -> 스레드 클래스. 
     // 데이터 송수신과 같은 네트워크 작업을 비동기적으로 처리(SendData)

public:
    FUDPClientRunnable(FSocket* InSocket, TSharedPtr<FInternetAddr> InRemoteAddr);
    virtual ~FUDPClientRunnable();

    virtual bool Init() override;
    virtual uint32 Run() override;
    virtual void Stop() override;

    void SendData(const FString& DataToSend);

private:

    // 클라이언트 측 UDP 소켓, 데이터 처리와 관련된 멤버 변수들
    FSocket* Socket;
    TSharedPtr<FInternetAddr> RemoteAddr;
    FRunnableThread* Thread;
    FThreadSafeBool bStopping;

    UPROPERTY(BlueprintReadOnly)
    FString MessageToSend;


public:

    void SendedMessage(const FString& InMessageToSend);

    UPROPERTY(BlueprintReadOnly)
    FString WantToLog;


    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    class ULogWidget* LogWidget;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Meta = (BlueprintProtected = true))
    TSubclassOf<class ULogWidget> LogWidgetClass;

    class AUDPClient* Owner;


};

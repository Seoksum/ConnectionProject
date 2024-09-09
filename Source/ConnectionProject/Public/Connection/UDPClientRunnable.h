// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"

/**
 * 
 */
class CONNECTIONPROJECT_API FUDPClientRunnable : public FRunnable
{
    // FUDPClientRunnable -> ������ Ŭ����. 
     // ������ �ۼ��Ű� ���� ��Ʈ��ũ �۾��� �񵿱������� ó��(SendData)

public:
    FUDPClientRunnable(FSocket* InSocket, TSharedPtr<FInternetAddr> InRemoteAddr);
    virtual ~FUDPClientRunnable();

    virtual bool Init() override;
    virtual uint32 Run() override;
    virtual void Stop() override;

    void SendData(const FString& DataToSend);

private:

    // Ŭ���̾�Ʈ �� UDP ����, ������ ó���� ���õ� ��� ������
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

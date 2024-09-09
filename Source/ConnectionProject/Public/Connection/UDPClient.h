// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Networking.h"
#include "UDPServerRunnable.h"
#include "UDPClientRunnable.h"
#include "UDPClient.generated.h"



UCLASS()
class CONNECTIONPROJECT_API AUDPClient : public AActor
{
	GENERATED_BODY()
	
    // UDPClient -> 네트워크 초기화, 소켓 생성, 스레드 시작

public:
    // Sets default values for this actor's properties
    AUDPClient();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Networking")
    FString ServerIP;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Networking")
    int32 ServerPort;

    UFUNCTION(BlueprintCallable, Category = "Networking")
    void SendMessageToThread(FString InMessage);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Networking")
    FString WantsToPrintLogMessage;

private:
    void CreateSocket();
    void StopClient();
    void StartClientThread();

    FSocket* ClientSocket;
    TSharedPtr<FInternetAddr> RemoteAddr;
    FThreadSafeBool bClientRunning;
    
    TSharedPtr<FUDPClientRunnable> ClientRunnable;
    TSharedPtr<FUDPServerRunnable> ServerRunnable;

    FRunnableThread* ClientRunnableThread;
    FRunnableThread* ServerRunnableThread;

    // 구조체
public:

    FClientInfo ClientInfo;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    class ULogWidget* LogWidget;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Meta = (BlueprintProtected = true))
    TSubclassOf<class ULogWidget> LogWidgetClass;


};

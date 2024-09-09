// Fill out your copyright notice in the Description page of Project Settings.


#include "Connection/UDPServerRunnable.h"
#include "Networking.h"
#include "ConnectionGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Connection/UDPServer.h"
#include "Connection/UDPClient.h"
#include "UI/LogWidget.h"

FUDPServerRunnable::FUDPServerRunnable(FSocket* InSocket, TSharedPtr<FInternetAddr> InLocalAddr, UWorld* InWorld)
    : Socket(InSocket), LocalAddr(InLocalAddr), bStopping(false), World(InWorld)
{
    Thread = FRunnableThread::Create(this, TEXT("UDPServerRunnableThread"), 0, TPri_BelowNormal);
    MinimumStringLen = 3;
}

FUDPServerRunnable::~FUDPServerRunnable()
{
    if (Thread)
    {
        Thread->Kill(true);
        delete Thread;
        Thread = nullptr;
    }
}

bool FUDPServerRunnable::Init()
{
    UE_LOG(LogTemp, Log, TEXT("[PHONE] ServerRunnable Init() "));
    return (Socket != nullptr && LocalAddr.IsValid());
}

uint32 FUDPServerRunnable::Run()
{
    // 서버 스레드의 메인 루프
    while (!bStopping)
    {
        ReceiveData();
        FPlatformProcess::Sleep(0.1f);  // 서버가 과도하게 CPU를 사용하지 않도록 대기
    }

    return 0;
}

void FUDPServerRunnable::Stop()
{
    bStopping = true;
}

void FUDPServerRunnable::ReceiveData()
{
    TArray<uint8> ReceivedData;
    ReceivedData.SetNumUninitialized(1024);

    int32 BytesRead = 0;
    TSharedRef<FInternetAddr> Sender = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
    

    // 데이터 수신
    if (Socket->RecvFrom(ReceivedData.GetData(), ReceivedData.Num(), BytesRead, *Sender))
    {
        AsyncTask(ENamedThreads::GameThread, [this]()
            {
                Owner->LogWidget->AddLogMessage("???");
                UE_LOG(LogTemp, Log, TEXT("'!?!?!"));
            });


        if (BytesRead > 0)
        {
            ReceivedString = FString(ANSI_TO_TCHAR(reinterpret_cast<const char*>(ReceivedData.GetData())));
            UE_LOG(LogTemp, Log, TEXT("[PHONE]Received Messages : %s"), *ReceivedString);
            //ReceivedString = "";

            AsyncTask(ENamedThreads::GameThread, [this]()
                {
                    // UWorld를 사용한 작업은 반드시 게임 스레드에서 실행되어야 합니다.
                    // ValidWorld에서 필요한 작업 수행
                    if (World.IsValid())
                    {
                        // Json 파일로 저장
                        UWorld* ValidWorld = World.Get();
                        SaveToJson(ReceivedString, ValidWorld);    
                        Owner->LogWidget->AddLogMessage(ReceivedString);
                    }
                });
            ReceivedString.Empty();
            //OnReceiveClientMessage(Sender, Sender->GetPort());
        }
    }
}

void FUDPServerRunnable::SaveToJson(const FString& ReceivedMessage, UWorld* MyWorld)
{
    TArray<FString> Paths;
    ReceivedMessage.ParseIntoArray(Paths, TEXT("."));

    if (Paths.Num() < MinimumStringLen)
        return;

    if (Paths.Num() == MinimumStringLen)
    {
        UConnectionGameInstance* GameInstance = Cast<UConnectionGameInstance>(UGameplayStatics::GetGameInstance(World.Get()));
        if (GameInstance)
        {
            GameInstance->SaveToJson(Paths[0], Paths[1], Paths[2]);
        }
    }

    // 로그 출력
    for (FString& Element : Paths)
    {
        UE_LOG(LogClass, Warning, TEXT("Split Message : [%s]"), *Element);
    }
}

void FUDPServerRunnable::OnReceiveClientMessage(const FIPv4Address& ClientIP, int32 ClientPort)
{
    FString ClientKey = ClientIP.ToString() + TEXT(":") + FString::FromInt(ClientPort);

    if (!Clients.Contains(ClientKey))
    {
        // 새로운 클라이언트 정보 등록
        FClientInfo NewClient;
        NewClient.IPAddress = ClientIP;
        NewClient.Port = ClientPort;
        NewClient.LastReceived = FDateTime::Now();

        Clients.Add(ClientKey, NewClient);
    }
    else
    {
        // 이미 존재하는 클라이언트라면 마지막 수신 시간 갱신
        Clients[ClientKey].LastReceived = FDateTime::Now();
    }
}

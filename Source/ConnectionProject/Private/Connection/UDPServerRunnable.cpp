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
    // ���� �������� ���� ����
    while (!bStopping)
    {
        ReceiveData();
        FPlatformProcess::Sleep(0.1f);  // ������ �����ϰ� CPU�� ������� �ʵ��� ���
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
    

    // ������ ����
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
                    // UWorld�� ����� �۾��� �ݵ�� ���� �����忡�� ����Ǿ�� �մϴ�.
                    // ValidWorld���� �ʿ��� �۾� ����
                    if (World.IsValid())
                    {
                        // Json ���Ϸ� ����
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

    // �α� ���
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
        // ���ο� Ŭ���̾�Ʈ ���� ���
        FClientInfo NewClient;
        NewClient.IPAddress = ClientIP;
        NewClient.Port = ClientPort;
        NewClient.LastReceived = FDateTime::Now();

        Clients.Add(ClientKey, NewClient);
    }
    else
    {
        // �̹� �����ϴ� Ŭ���̾�Ʈ��� ������ ���� �ð� ����
        Clients[ClientKey].LastReceived = FDateTime::Now();
    }
}

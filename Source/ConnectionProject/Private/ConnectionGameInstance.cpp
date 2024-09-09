// Fill out your copyright notice in the Description page of Project Settings.


#include "ConnectionGameInstance.h"
#include "JsonObjectConverter.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Templates/SharedPointer.h"
#include "Kismet/GameplayStatics.h"
#include "AndroidPermissionFunctionLibrary.h"

UConnectionGameInstance::UConnectionGameInstance()
{
	Ip = "192.168.0.112";
	Port = 8001;

#if PLATFORM_WINDOWS
	// 윈도우 플랫폼에서만 실행되는 코드
	GI_IsServer = true;
#else
	// 윈도우가 아닌 다른 플랫폼에서 실행되는 코드
	GI_IsServer = false;
#endif

}

void UConnectionGameInstance::Init()
{
	Super::Init();

	// 저장소 권한이 있는지 확인하고, 없다면 요청
	if (UAndroidPermissionFunctionLibrary::CheckPermission("android.permission.WRITE_EXTERNAL_STORAGE") == false)
	{
		TArray<FString> Permissions;
		Permissions.Add("android.permission.WRITE_EXTERNAL_STORAGE");
		Permissions.Add("android.permission.READ_EXTERNAL_STORAGE");
		UAndroidPermissionFunctionLibrary::AcquirePermissions(Permissions);
	}
}

void UConnectionGameInstance::SetIp(const FString& IpAddr)
{
	Ip = IpAddr;
}

void UConnectionGameInstance::SetPort(int32 InPort)
{
	Port = InPort;
}

FString UConnectionGameInstance::GetIp()
{
	return Ip;
}
int32 UConnectionGameInstance::GetPort()
{
	return Port;
}

// Name, SocketName, null, IsVisiblity 
void UConnectionGameInstance::SaveToJson(FString Name, FString Section, FString IsHidden)
{
	FJsonDetailData DetailData;
	DetailData.Name = Name;
	DetailData.SectionName = Section;
	DetailData.IsHidden = StringToBoolType(IsHidden);
	JsonDatas.Datas.Add(DetailData);

	FString JsonString;
	FJsonObjectConverter::UStructToJsonObjectString(JsonDatas, JsonString);
	FFileHelper::SaveStringToFile(*JsonString, *(FPaths::ProjectContentDir() + TEXT("/Json/Test.json")));

	LoadFromJson();
}

void UConnectionGameInstance::LoadFromJson()
{
	FString FilePath = FPaths::ProjectContentDir() + "Json/Test.json";
	FString JsonString;
	FFileHelper::LoadFileToString(JsonString, *FilePath);

	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonString);
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());


	if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
	{
		TArray<TSharedPtr<FJsonValue>> JsonValueArray = JsonObject->GetArrayField(TEXT("datas"));

		for (int32 i = 0; i < JsonValueArray.Num(); i++)
		{
			TSharedPtr<FJsonValue> JsonValue = JsonValueArray[i];
			TSharedPtr<FJsonObject> JsonValueObject = JsonValue->AsObject();

			FJsonDetailData DetailData;
			DetailData.Name = JsonValueObject->GetStringField(TEXT("name"));
			DetailData.SectionName = JsonValueObject->GetStringField(TEXT("sectionname"));
			DetailData.IsHidden = JsonValueObject->GetBoolField(TEXT("ishidden"));

			JsonDatas.Datas.Push(DetailData);


			TArray<AActor*> FoundActors;
			UGameplayStatics::GetAllActorsWithTag(GetWorld(), *DetailData.Name, FoundActors);

			for (AActor* Actor : FoundActors)
			{
				SetVisiblity(Actor, DetailData.SectionName, DetailData.IsHidden);
			}

		}
	}
}

bool UConnectionGameInstance::StringToBoolType(FString str)
{
	if (str.Equals("true")) { return true; }
	else { return false; }
}

void UConnectionGameInstance::SetVisiblity(AActor* TargetActor, FString TagName, bool flag)
{
	TArray<AActor*> ChildActors;
	TargetActor->GetAttachedActors(ChildActors);

	for (AActor* ChildActor : ChildActors)
	{
		if (ChildActor)
		{
			if (ChildActor->ActorHasTag(*TagName))
			{
				ChildActor->SetActorHiddenInGame(flag);
			}
		}
	}
}

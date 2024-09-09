// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ConnectionGameInstance.generated.h"


USTRUCT(BlueprintType)
struct FJsonDetailData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "JsonData")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "JsonData")
	FString SectionName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "JsonData")
	bool IsHidden = false;
};

USTRUCT(BlueprintType)
struct FJsonData
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "JsonData")
	TArray<FJsonDetailData> Datas;
};

/**
 * 
 */
UCLASS()
class CONNECTIONPROJECT_API UConnectionGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	UConnectionGameInstance();

	virtual void Init() override;

	UFUNCTION(BlueprintCallable)
	void SetIp(const FString& IpAddr);

	UFUNCTION(BlueprintCallable)
	void SetPort(int32 InPort);

	FString GetIp();
	int32 GetPort();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool GI_IsServer;

private:

	FString Ip;
	int32 Port;


	// JSON
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "JsonData")
	FJsonData JsonDatas;

	// Json ���Ϸ� ����
	UFUNCTION(BlueprintCallable, Category = "JsonData")
	void SaveToJson(FString Name, FString Section, FString IsHidden);

	// Json ���� �о����
	UFUNCTION(BlueprintCallable, Category = "JsonData")
	void LoadFromJson();

	bool StringToBoolType(FString str);

	void SetVisiblity(AActor* TargetActor, FString TagName, bool flag);


};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LogWidget.generated.h"

/**
 * 
 */
UCLASS()
class CONNECTIONPROJECT_API ULogWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	void AddLogMessage(const FString& NewMessage);

	UFUNCTION(BlueprintCallable)
	void AddLogMessageIpAddr();

	UFUNCTION(BlueprintCallable)
	void ClearLogTextBlock();


protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* LogTextBlock;

	FTimerHandle LifeSpanTimer;
	
	float LifeSpan = 3.f;

};

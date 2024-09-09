// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LogWidget.h"
#include "Components/TextBlock.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "TimerManager.h"

void ULogWidget::AddLogMessage(const FString& NewMessage)
{
	if (LogTextBlock)
	{
		FString CurrentText = LogTextBlock->GetText().ToString();
		CurrentText += "\n" + NewMessage;
		//CurrentText = NewMessage;
		LogTextBlock->SetText(FText::FromString(CurrentText));
	}
}

void ULogWidget::AddLogMessageIpAddr()
{
	bool bCanBind = false;
	TSharedPtr<FInternetAddr> Addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLocalHostAddr(*GLog, bCanBind);
	AddLogMessage(Addr->ToString(false));

}

void ULogWidget::ClearLogTextBlock()
{
	if (LogTextBlock)
	{
		// 3초 뒤 텍스트 사라짐
		GetWorld()->GetTimerManager().SetTimer(LifeSpanTimer, FTimerDelegate::CreateLambda([this] {
			LogTextBlock->SetText(FText::GetEmpty());
			}), LifeSpan, false);

	}
}

#include "SGUI.h"
#include "FloatingTextWidget.h"

UFloatingTextWidget* UFloatingTextWidget::Create(UObject* worldContextObject, FString message, FLinearColor textColor)
{
	auto widget = InstantiateWidget<UFloatingTextWidget>(worldContextObject);
	if (!widget) return nullptr;

	widget->messageLabel = Cast<UTextBlock>(widget->GetWidgetFromName(TEXT("MessageLabel")));
	widget->messageLabel->SetText(FText::FromString(message));
	widget->SetColor(textColor);

	return widget;
}

void UFloatingTextWidget::Spawn(UObject* worldContextObject, FString message, FLinearColor textColor /*= FLinearColor::White*/)
{
	if (!IsGameRunning(worldContextObject)) return;

	auto widget = Create(worldContextObject, message, textColor);
	if (!widget) return;

	GetSlot(widget->messageLabel)->SetPosition(GetViewportCenter());
}

void UFloatingTextWidget::SpawnAtPosition(UObject* worldContextObject, FVector2D screenPosition, FString message, FLinearColor textColor /*= FLinearColor::White*/)
{
	if (!IsGameRunning(worldContextObject)) return;

	auto widget = Create(worldContextObject, message, textColor);
	if (!widget) return;

	GetSlot(widget->messageLabel)->SetPosition(screenPosition / GetViewportScale());
}

void UFloatingTextWidget::SpawnAtActor(UObject* worldContextObject, AActor* targetActor, FString message, FVector offset /*= FVector::ZeroVector*/, FLinearColor textColor /*= FLinearColor::White*/)
{
	if (!IsGameRunning(worldContextObject)) return;

	auto widget = Create(worldContextObject, message, textColor);
	if (!widget) return;

	FVector2D screenPos;
	widget->MasterController->ProjectWorldLocationToScreen(targetActor->GetActorLocation() + offset, screenPos);
	GetSlot(widget->messageLabel)->SetPosition(screenPos / GetViewportScale());
}

void UFloatingTextWidget::Tick_Implementation(FGeometry myGeometry, float deltaTime)
{
	Super::Tick_Implementation(myGeometry, deltaTime);

	auto curPos = GetSlot(messageLabel)->GetPosition();

	GetSlot(messageLabel)->SetPosition(curPos + FVector2D(0, -FloatingSpeed * deltaTime));

	auto opacity = FMath::InterpExpoOut(messageLabel->ColorAndOpacity.GetSpecifiedColor().A,
		FMath::Clamp(curPos.Y / (GetViewportSize().Y / 2), 0.f, 1.f),
		deltaTime * FadeSpeed);
	auto color = messageLabel->ColorAndOpacity.GetSpecifiedColor();
	color.A = opacity;
	SetColor(color);

	if (curPos.Y < 0) RemoveFromViewport();
}

void UFloatingTextWidget::SetColor(const FLinearColor& color)
{
	messageLabel->SetColorAndOpacity(color);
	auto shadowColor = messageLabel->ShadowColorAndOpacity;
	// base shadow opacity is .8 (.2 less than base text opacity), so excluding it
	messageLabel->SetShadowColorAndOpacity(FLinearColor(shadowColor.R, shadowColor.G, shadowColor.B, FMath::Clamp(color.A - .2f, 0.f, 1.f)));
}



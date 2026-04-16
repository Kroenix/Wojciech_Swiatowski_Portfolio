#include "DamageNumberWidget.h"
#include "Components/TextBlock.h"

void UDamageNumberWidget::SetDamage(float DamageAmount)
{
    if (!DamageText) return;

  
    FString DamageString = FString::Printf(TEXT("%.0f"), DamageAmount);
    DamageText->SetText(FText::FromString(DamageString));

  
    FLinearColor TextColor;

    if (DamageAmount >= 25.0f)
    {
        TextColor = FLinearColor(1.0f, 0.3f, 0.0f, 1.0f); 
    }
    else if (DamageAmount >= 10.0f)
    {
        TextColor = FLinearColor(1.0f, 1.0f, 0.0f, 1.0f); 
    }
    else
    {
        TextColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f); 
    }

    DamageText->SetColorAndOpacity(FSlateColor(TextColor));
}
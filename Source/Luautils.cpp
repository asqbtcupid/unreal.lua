
#include "Your_Project_Module_Name.h" 
#include "TableUtil.h"
#include "Luautils.h"

UWorld* ULuautils::GetWorld(AActor* obj)
{
	return obj->GetWorld();
}

void ULuautils::SetupAttachment(USceneComponent* Child, USceneComponent* InParent, FName InSocketName)
{
	Child->SetupAttachment(InParent, InSocketName);
}

void ULuautils::GetPlayerViewPoint(APlayerController* controler, FVector& out_Location, FRotator& out_Rotation )
{
	if (controler)
		controler->GetPlayerViewPoint(out_Location, out_Rotation);
}

FRotator ULuautils::GetViewRotation(APawn* pawn)
{
	return pawn->GetViewRotation();
}

FVector ULuautils::VRandCone(FRandomStream stream, FVector const& Dir, float HorizontalConeHalfAngleRad, float VerticalConeHalfAngleRad)
{
	return stream.VRandCone(Dir, HorizontalConeHalfAngleRad, VerticalConeHalfAngleRad);
}

FVector2D ULuautils::FVector2D_New(float InX, float InY)
{
	return FVector2D(InX, InY);
}

FColor ULuautils::FColor_New(uint8 InR, uint8 InG, uint8 InB, uint8 InA)
{
	return FColor(InR,InG,InB,InA);
}

void ULuautils::GetAllWidgets(UUserWidget* UserWidget, TArray<FName>& Names, TArray<UWidget*>& Widgets)
{
	UWidgetTree* WidgetTree = UserWidget->WidgetTree;
	WidgetTree->GetAllWidgets(Widgets);
	for (auto &v : Widgets)
	{
		Names.Add(v->GetFName());
	}
}
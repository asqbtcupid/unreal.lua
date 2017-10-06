
#include "CatchMe.h" 
#include "TableUtil.h"
#include "Luautils.h"

UWorld* ULuautils::GetWorld1(AActor* obj)
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

void ULuautils::GetAllWidgets(UUserWidget* UserWidget, TArray<FName>& Names, TArray<UWidget*>& Widgets, TArray<FName>&TypesOfWidget)
{
	UWidgetTree* WidgetTree = UserWidget->WidgetTree;
	WidgetTree->GetAllWidgets(Widgets);
	for (auto &v : Widgets)
	{
		UClass* Class = v->GetClass();
		FString namecpp = FString::Printf(TEXT("%s%s"), Class->GetPrefixCPP(), *Class->GetName());
		Names.Add(v->GetFName());
		TypesOfWidget.Add(*namecpp);
	}
}

void ULuautils::log(FString content)
{
	UTableUtil::log(content);
}

FString ULuautils::GetName(UObject* p)
{
	check(p != nullptr)
	return p->GetName();
}

void ULuautils::UpdateNav(UActorComponent *Component)
{
	UNavigationSystem::UpdateComponentInNavOctree(*Component);
}

void ULuautils::AddGCRef(UObject*p)
{
	UTableUtil::addgcref(p);
}

void ULuautils::Ctor(const UObject* WorldContextObject, const FString& LuaClassName)
{
	UTableUtil::call("Ctor", LuaClassName, WorldContextObject);
}

void ULuautils::Call(const UObject* WorldContextObject, const FString& FunctionName)
{
	UTableUtil::inscall("Call", TCHAR_TO_ANSI(*FunctionName), WorldContextObject);
}


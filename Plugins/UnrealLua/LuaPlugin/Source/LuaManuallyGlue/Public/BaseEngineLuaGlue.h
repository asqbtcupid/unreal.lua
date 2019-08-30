// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "JustForLinkGlue.h"
#include "UnrealLua.h"
#include "WidgetBlueprintLibrary.h"
#include "HAL/PlatformProcess.h"
#include "FileManager.h"
#include "FileHelper.h"
#include "IConsoleManager.h"
#include "TaskGraphInterfaces.h"
#include "Reply.h"
#include "DragAndDrop.h"
#include "BaseStructGlue.h"
#include "GameFramework/Character.h"
#include "Engine/GameViewportClient.h"
#include "Slate/SceneViewport.h"

LUA_GLUE_ARR(bool, bool);
LUA_GLUE_ARR(uint8, uint8)
LUA_GLUE_ARR(int, int)
LUA_GLUE_ARR(uint32, uint32)
LUA_GLUE_ARR(float, float);
LUA_GLUE_ARR(double, double);
LUA_GLUE_ARR(uint64, uint64)
LUA_GLUE_ARR(FString, FString)
LUA_GLUE_ARR(FName, FName)
LUA_GLUE_ARR(FVector, FVector)
LUA_GLUE_ARR(UObject, UObject*)
LUA_GLUE_ARR(TArray_int, TArray<int>)

LUA_GLUE_MAP(int_int, int, int)
LUA_GLUE_MAP(int_FString, int, FString)
LUA_GLUE_MAP(int_TArray_int, int, TArray<int>)
LUA_GLUE_MAP(FString_int, FString, int)
LUA_GLUE_MAP(FName_UObject, FName, UObject*)
LUA_GLUE_MAP(FString_FString, FString, FString)

LUA_GLUE_SET(FString, FString)
LUA_GLUE_SET(int, int32)

static FTimespan MakeFTimespanFrom(int32 Hours, int32 Minutes, int32 Seconds)
{
	return FTimespan(Hours, Minutes, Seconds);
}

static FText TimeSpanToText(FTimespan Timespan)
{
	return FText::AsTimespan(Timespan);
}

LUA_GLUE_BEGIN(FTimespan)
LUA_GLUE_FUNCTION_OUT(FTimespan, MakeFTimespanFrom)
LUA_GLUE_FUNCTION_OVERLOAD(__add, operator+, FTimespan(FTimespan::*)(const FTimespan& Other) const)
LUA_GLUE_FUNCTION_OVERLOAD(__sub, operator-, FTimespan(FTimespan::*)(const FTimespan& Other) const)
LUA_GLUE_FUNCTION_OVERLOAD(__mul, operator*, FTimespan(FTimespan::*)(double Scalar) const)
LUA_GLUE_FUNCTION_OVERLOAD(__div, operator/, FTimespan(FTimespan::*)(double Scalar) const)
LUA_GLUE_FUNCTION_OVERLOAD(__mod, operator%, FTimespan(FTimespan::*)(const FTimespan& Other)const)
LUA_GLUE_FUNCTION_OVERLOAD(__eq, operator==, bool (FTimespan::*)(const FTimespan& Other) const)
LUA_GLUE_FUNCTION(GetDays)
LUA_GLUE_FUNCTION(GetHours)
LUA_GLUE_FUNCTION(GetSeconds)
LUA_GLUE_FUNCTION(GetDuration)
LUA_GLUE_FUNCTION(GetMinutes)
LUA_GLUE_FUNCTION(GetTicks)
LUA_GLUE_FUNCTION(GetDuration)
LUA_GLUE_FUNCTION(GetTotalSeconds)
LUA_GLUE_FUNCTION(FromDays)
LUA_GLUE_FUNCTION(FromHours)
LUA_GLUE_FUNCTION(FromMinutes)
LUA_GLUE_FUNCTION(FromSeconds)
LUA_GLUE_FUNCTION(Parse)
LUA_GLUE_FUNCTION(Zero)
LUA_GLUE_FUNCTION_OUT(ToText, TimeSpanToText)
LUA_GLUE_OVERLOAD(ToString, FString(FTimespan::*)(const TCHAR* Format)const)
LUA_GLUE_END()

static FDateTime MakeDateTime(int32 Year, int32 Month, int32 Day, int32 Hour,  int32 Minute,  int32 Second, int32 Millisecond)
{
	if (FDateTime::Validate(Year, Month, Day, Hour, Minute, Second, Millisecond))
		return FDateTime(Year, Month, Day, Hour, Minute, Second, Millisecond);
	else
	{
		ensureAlwaysMsgf(0, TEXT("Time is not valid"));
// 		UnrealLua::ReportError("Time is not valid");
		return FDateTime();
	}
}

static FDateTime MakeDateTimeFromTick(int64 InTicks)
{
	return FDateTime(InTicks);
}

LUA_GLUE_BEGIN(FDateTime)
LUA_GLUE_FUNCTION(Now)
LUA_GLUE_FUNCTION(UtcNow)
LUA_GLUE_FUNCTION(Today)
LUA_GLUE_FUNCTION(ToUnixTimestamp)
LUA_GLUE_FUNCTION(FromUnixTimestamp)
LUA_GLUE_FUNCTION(GetYear)
LUA_GLUE_FUNCTION(GetHour)
LUA_GLUE_FUNCTION(GetMinute)
LUA_GLUE_FUNCTION(GetSecond)
LUA_GLUE_FUNCTION(GetMonth)
LUA_GLUE_FUNCTION(GetDay)
LUA_GLUE_OVERLOAD(GetDate, FDateTime(FDateTime::*)()const)
LUA_GLUE_FUNCTION_OVERLOAD(__sub, operator-, FTimespan(FDateTime::*)(const FDateTime& Other) const)
LUA_GLUE_FUNCTION_OVERLOAD(__add, operator+, FDateTime(FDateTime::*)(const FTimespan& Other) const)
LUA_GLUE_FUNCTION_NAME(__eq, operator==)
LUA_GLUE_FUNCTION_NAME(__lt, operator<)
LUA_GLUE_FUNCTION_NAME(__le, operator<=)
LUA_GLUE_FUNCTION_OUT(Make, MakeDateTime, 0,0,0,0,0,0,0)
LUA_GLUE_FUNCTION_OUT(MakeFromTicks, MakeDateTimeFromTick)
LUA_GLUE_OVERLOAD(ToString, FString(FDateTime::*)(const TCHAR*)const)
LUA_GLUE_END()

LUA_GLUE_BEGIN(UWidgetBlueprintLibrary)
LUA_GLUE_FUNCTION(Create)
LUA_GLUE_END()

LUA_GLUE_BEGIN_NAME(FPlatformMemory, FPlatformMemory)
LUA_GLUE_FUNCTION(GetPhysicalGBRam)
LUA_GLUE_END()


LUA_GLUE_INTERFACE(FPaths, FPaths)
LUA_GLUE_FUNCTION(FileExists)
LUA_GLUE_FUNCTION(DirectoryExists)
LUA_GLUE_FUNCTION(ProjectSavedDir)
LUA_GLUE_FUNCTION(ProjectDir)
LUA_GLUE_FUNCTION(ProjectContentDir)
LUA_GLUE_FUNCTION(EngineContentDir)
LUA_GLUE_FUNCTION(EngineConfigDir)
LUA_GLUE_FUNCTION(EngineSavedDir)
LUA_GLUE_FUNCTION(ProjectSavedDir)
LUA_GLUE_FUNCTION(ProjectPluginsDir)
LUA_GLUE_FUNCTION(ProjectPersistentDownloadDir)
LUA_GLUE_OVERLOAD(ConvertRelativePathToFull, FString (*)(const FString& InPath))
LUA_GLUE_FUNCTION(GeneratedConfigDir)
LUA_GLUE_END()

LUA_GLUE_INTERFACE(IFileManager, IFileManager)
LUA_GLUE_FUNCTION(Get)
LUA_GLUE_FUNCTION(FileExists)
LUA_GLUE_FUNCTION(DirectoryExists)
LUA_GLUE_FUNCTION(MakeDirectory)
LUA_GLUE_FUNCTION(DeleteDirectory)
LUA_GLUE_FUNCTION(Copy)
LUA_GLUE_FUNCTION(Move)
LUA_GLUE_FUNCTION(Delete)
LUA_GLUE_OVERLOAD(FindFiles, void (IFileManager::*)(TArray<FString>&, const TCHAR*, bool, bool))
LUA_GLUE_OVERLOAD(FindFiles, void (IFileManager::*)(TArray<FString>&, const TCHAR*, const TCHAR*))
LUA_GLUE_FUNCTION(FindFilesRecursive)
LUA_GLUE_FUNCTION(GetTimeStamp)
LUA_GLUE_END()

LUA_GLUE_BEGIN_NAME(FFileHelper, FFileHelper)
LUA_GLUE_FUNCTION(LoadFileToString)
LUA_GLUE_FUNCTION(SaveStringToFile)
LUA_GLUE_FUNCTION(LoadFileToArray, 0)
LUA_GLUE_END()

LUA_GLUE_BEGIN(FPlatformProperties)
LUA_GLUE_FUNCTION(PlatformName)
LUA_GLUE_FUNCTION(IniPlatformName)
LUA_GLUE_FUNCTION(GetPhysicsFormat)
LUA_GLUE_FUNCTION(IsGameOnly)
LUA_GLUE_FUNCTION(IsServerOnly)
LUA_GLUE_FUNCTION(IsClientOnly)
LUA_GLUE_END()

static FConfigCacheIni* GetGConfig()
{
	return GConfig;
}

static FString GConfig_LoadGloablIni(FString const & IniName)
{
	FString Result;
	FConfigCacheIni::LoadGlobalIniFile(Result, *IniName, NULL, false, true);
	return Result;
}

LUA_GLUE_BEGIN_NAME(FConfigCacheIni, FConfigCacheIni)
LUA_GLUE_FUNCTION_OUT(GConfig, GetGConfig)
LUA_GLUE_FUNCTION(Flush)
LUA_GLUE_OVERLOAD(GetString, bool (FConfigCacheIni::*)(const TCHAR* Section, const TCHAR* Key, FString& Value, const FString& Filename))
LUA_GLUE_OVERLOAD(SetString, void (FConfigCacheIni::*)(const TCHAR* Section, const TCHAR* Key, const TCHAR* Value, const FString& Filename))
LUA_GLUE_OVERLOAD(GetArray,   int32 (FConfigCacheIni::*)(const TCHAR* Section, const TCHAR* Key,TArray<FString>&	out_Arr,const FString&	Filename))
LUA_GLUE_FUNCTION(LoadGlobalIniFile, nullptr, false, false, true, *FPaths::GeneratedConfigDir())
LUA_GLUE_FUNCTION_OUT(LoadGloablIni, GConfig_LoadGloablIni)
LUA_GLUE_END()

LUA_GLUE_BEGIN(AActor)
LUA_GLUE_FUNCTION(GetNetMode)
LUA_GLUE_FUNCTION(GetWorld)
LUA_GLUE_END()


LUA_GLUE_BEGIN(ACharacter)
LUA_GLUE_FUNCTION(GetCharacterMovement)
LUA_GLUE_FUNCTION(GetCapsuleComponent)
LUA_GLUE_END()

LUA_GLUE_BEGIN(UUserWidget)
LUA_GLUE_FUNCTION(GetSlotNames)
LUA_GLUE_FUNCTION(GetContentForSlot)
LUA_GLUE_FUNCTION(SetContentForSlot)
LUA_GLUE_FUNCTION(RemoveFromParent)
LUA_GLUE_FUNCTION(GetWidgetFromName)
LUA_GLUE_FUNCTION(AddToViewport, 0)
LUA_GLUE_FUNCTION_NAME(FindWidget, GetWidgetFromName)
LUA_GLUE_END()

LUA_GLUE_BEGIN(UWorld)
LUA_GLUE_FUNCTION(GetFirstPlayerController)
LUA_GLUE_FUNCTION(GetFirstLocalPlayerFromController)
LUA_GLUE_FUNCTION(RegisterAutoActivateCamera)
LUA_GLUE_OVERLOAD(GetGameInstance, UGameInstance* (UWorld::*)() const)
LUA_GLUE_FUNCTION(GetGameViewport)
LUA_GLUE_FUNCTION(AreActorsInitialized)
LUA_GLUE_FUNCTION(HasBegunPlay)
LUA_GLUE_FUNCTION(GetTimeSeconds)
LUA_GLUE_FUNCTION(GetUnpausedTimeSeconds)
LUA_GLUE_FUNCTION(GetRealTimeSeconds)
LUA_GLUE_FUNCTION(GetAudioTimeSeconds)
LUA_GLUE_FUNCTION(GetDeltaSeconds)
LUA_GLUE_END()

LUA_GLUE_BEGIN_NAME(IConsoleVariable, IConsoleVariable)
// LUA_GLUE_FUNCTION(Set)
LUA_GLUE_FUNCTION(GetInt)
LUA_GLUE_FUNCTION(GetFloat)
LUA_GLUE_FUNCTION(GetString)
LUA_GLUE_OVERLOAD(SetWithCurrentPriority, void (IConsoleVariable::*)(int32 InValue))
LUA_GLUE_OVERLOAD(SetWithCurrentPriority, void (IConsoleVariable::*)(float InValue))
LUA_GLUE_END()

LUA_GLUE_BEGIN_NAME(IConsoleManager, IConsoleManager)
// LUA_GLUE_FUNCTION(RegisterConsoleVariable)
// LUA_GLUE_FUNCTION(RegisterConsoleVariableRef)
LUA_GLUE_FUNCTION(FindConsoleObject)
LUA_GLUE_FUNCTION(FindConsoleVariable)
LUA_GLUE_FUNCTION(Get)
LUA_GLUE_END()

static FMonitoredProcess* Make_FMonitoredProcess(const FString& InURL, const FString& InParams, bool InHidden, bool InCreatePipes )
{
	return new FMonitoredProcess(InURL, InParams, InHidden, InCreatePipes);
}

LUA_GLUE_DELEGATE(FOnMonitoredProcessCompleted, FOnMonitoredProcessCompleted)
LUA_GLUE_DELEGATE(FOnMonitoredProcessOutput, FOnMonitoredProcessOutput)
LUA_GLUE_DELEGATE(FSimpleDelegate, FSimpleDelegate)

static int32 OnFMonitoredProcessOutPut(lua_State*inL)
{
	FMonitoredProcess* Ins = (FMonitoredProcess*)tovoid(inL, 1);
	TSharedPtr<UnrealLuaFunctor> LuaCallBack = MakeShared<UnrealLuaFunctor>(inL, 2);

	auto MoniterThreadToCall = [LuaFunc{ LuaCallBack }](FString OutPut) {
		auto MainThreadToCall = [=](ENamedThreads::Type a, const  FGraphEventRef& b){ 
			if (LuaFunc->IsValid())
				LuaFunc->Call(OutPut); 
		};
		FDelegateGraphTask::CreateAndDispatchWhenReady(FDelegateGraphTask::FDelegate::CreateLambda(MainThreadToCall), TStatId(), nullptr, ENamedThreads::AnyThread, ENamedThreads::GameThread);
	};

	Ins->OnOutput().BindLambda(MoniterThreadToCall);
	return 0;
}

static int32 OnFMonitoredProcessComplete(lua_State*inL)
{
	FMonitoredProcess* Ins = (FMonitoredProcess*)tovoid(inL, 1);
	TSharedPtr<UnrealLuaFunctor> LuaCallBack = MakeShared<UnrealLuaFunctor>(inL, 2);

	auto MoniterThreadToCall = [LuaFunc{ LuaCallBack }](int32 Code) {
		auto MainThreadToCall = [=](ENamedThreads::Type a, const  FGraphEventRef& b) {
			if (LuaFunc->IsValid())
				LuaFunc->Call(Code);
		};
		FDelegateGraphTask::CreateAndDispatchWhenReady(FDelegateGraphTask::FDelegate::CreateLambda(MainThreadToCall), TStatId(), nullptr, ENamedThreads::AnyThread, ENamedThreads::GameThread);
	};

	Ins->OnCompleted().BindLambda(MoniterThreadToCall);
	return 0;
}

static int32 OnFMonitoredProcessCancel(lua_State*inL)
{
	FMonitoredProcess* Ins = (FMonitoredProcess*)tovoid(inL, 1);
	TSharedPtr<UnrealLuaFunctor> LuaCallBack = MakeShared<UnrealLuaFunctor>(inL, 2);

	auto MoniterThreadToCall = [LuaFunc{ LuaCallBack }]() {
		auto MainThreadToCall = [=](ENamedThreads::Type a, const  FGraphEventRef& b) {
			if (LuaFunc->IsValid())
				LuaFunc->Call();
		};
		FDelegateGraphTask::CreateAndDispatchWhenReady(FDelegateGraphTask::FDelegate::CreateLambda(MainThreadToCall), TStatId(), nullptr, ENamedThreads::AnyThread, ENamedThreads::GameThread);
	};

	Ins->OnCanceled().BindLambda(MoniterThreadToCall);
	return 0;
}

LUA_GLUE_BEGIN_NAME(FMonitoredProcess, FMonitoredProcess)
LUA_GLUE_FUNCTION_OUT(NewOne, Make_FMonitoredProcess)
LUA_GLUE_FUNCTION(Cancel)
LUA_GLUE_FUNCTION(GetDuration)
LUA_GLUE_FUNCTION(Update)
LUA_GLUE_FUNCTION(Launch)
LUA_GLUE_FUNCTION(SetSleepInterval)
LUA_GLUE_FUNCTION(OnCanceled)
LUA_GLUE_FUNCTION(OnCompleted)
LUA_GLUE_CUSTOM_FUNCTION_OUT(OnOutput, OnFMonitoredProcessOutPut)
LUA_GLUE_CUSTOM_FUNCTION_OUT(OnCanceled, OnFMonitoredProcessCancel)
LUA_GLUE_CUSTOM_FUNCTION_OUT(OnCompleted, OnFMonitoredProcessComplete)
LUA_GLUE_FUNCTION(GetReturnCode)
LUA_GLUE_END()

LUA_GLUE_BEGIN_NAME(FReply, FReply)
LUA_GLUE_FUNCTION(Handled)
LUA_GLUE_FUNCTION(Unhandled)
LUA_GLUE_END()

LUA_GLUE_BEGIN(FPointerEvent)
LUA_GLUE_FUNCTION(GetScreenSpacePosition)
LUA_GLUE_END()

LUA_GLUE_BEGIN(FGeometry)
LUA_GLUE_FUNCTION(GetAbsoluteSize)
LUA_GLUE_END()

LUA_GLUE_BEGIN_NAME(FDragDropOperation, FDragDropOperation)

LUA_GLUE_END()

LUA_GLUE_BEGIN_NAME(FExternalDragOperation, FExternalDragOperation)
LUA_GLUE_FUNCTION(HasText)
LUA_GLUE_FUNCTION(HasFiles)
LUA_GLUE_FUNCTION(GetText)
LUA_GLUE_FUNCTION(GetFiles)
LUA_GLUE_END(FDragDropOperation)


LUA_GLUE_SHARED(FDragDropOperation, FDragDropOperation)
LUA_GLUE_SHARED(FExternalDragOperation, FExternalDragOperation)

static FExternalDragOperation* GetExternalDrag(FDragDropEvent* Event)
{
	TSharedPtr<FExternalDragOperation> Result = Event->GetOperationAs<FExternalDragOperation>();
	return Result.IsValid() ? Result.Get() : nullptr;
}

LUA_GLUE_BEGIN_NAME(FDragDropEvent, FDragDropEvent)
LUA_GLUE_FUNCTION(GetOperation)
LUA_GLUE_FUNCTION(GetScreenSpacePosition)
LUA_GLUE_FUNCTION(GetLastScreenSpacePosition)
LUA_GLUE_FUNCTION(GetCursorDelta)
LUA_GLUE_FUNCTION(IsMouseButtonDown)
LUA_GLUE_FUNCTION(GetEffectingButton)
LUA_GLUE_FUNCTION(GetWheelDelta)
LUA_GLUE_FUNCTION(GetUserIndex)
LUA_GLUE_FUNCTION(GetPointerIndex)
LUA_GLUE_FUNCTION(GetTouchpadIndex)
// LUA_GLUE_FUNCTION(GetTouchForce)
LUA_GLUE_FUNCTION(IsTouchEvent)
LUA_GLUE_FUNCTION(GetGestureType)
LUA_GLUE_FUNCTION(GetGestureDelta)
LUA_GLUE_FUNCTION_OUT(GetExternalDrag, GetExternalDrag)
LUA_GLUE_FUNCTION(IsDirectionInvertedFromDevice)
LUA_GLUE_END()

LUA_GLUE_BEGIN_NAME(FPlatformProcess, FPlatformProcess)
LUA_GLUE_FUNCTION(LaunchURL)
LUA_GLUE_FUNCTION(ExploreFolder)
LUA_GLUE_FUNCTION(CanLaunchURL)
#if PLATFORM_IOS
// LUA_GLUE_FUNCTION(OpenIOSSetting)
#endif
#if PLATFORM_ANDROID
// LUA_GLUE_FUNCTION(OpenLocationSetting)
#endif
LUA_GLUE_END()


static FLinearColor FLinearColor_New1(float R, float G, float B, float A)
{
	return FLinearColor(R, G, B, A);
}

LUA_GLUE_BEGIN(FLinearColor)
LUA_GLUE_FUNCTION_OUT(Temp1, FLinearColor_New1)
LUA_GLUE_PROPERTY_GET(White)
LUA_GLUE_PROPERTY_GET(Gray)
LUA_GLUE_PROPERTY_GET(Black)
LUA_GLUE_PROPERTY_GET(Transparent)
LUA_GLUE_PROPERTY_GET(Red)
LUA_GLUE_PROPERTY_GET(Green)
LUA_GLUE_PROPERTY_GET(Blue)
LUA_GLUE_PROPERTY_GET(Yellow)
LUA_GLUE_END()

static FString FPlatformMisc_GetDeviceModel()
{
#if PLATFORM_ANDROID
	return FPlatformMisc::GetDeviceModel();
#elif PLATFORM_IOS
	return FPlatformMisc::GetDeviceId();
#else 
	return TEXT("");
#endif
}

LUA_GLUE_BEGIN_NAME(FPlatformMisc, FPlatformMisc)
LUA_GLUE_FUNCTION(HasActiveWiFiConnection)
LUA_GLUE_FUNCTION(GetBatteryLevel)
LUA_GLUE_FUNCTION(IsRunningOnBattery)
LUA_GLUE_FUNCTION_OUT(GetDeviceModel, FPlatformMisc_GetDeviceModel)
LUA_GLUE_END()

LUA_GLUE_BEGIN(FApp)
LUA_GLUE_FUNCTION(GetProjectName)
LUA_GLUE_END()

static FSlateChildSize FSlateChildSize_New()
{
	return FSlateChildSize();
}

LUA_GLUE_BEGIN(FSlateChildSize)
LUA_GLUE_FUNCTION_OUT(New, FSlateChildSize_New)
LUA_GLUE_END()

static FSlateColor FSlateColor_New_Out(float R, float G, float B, float A)
{
	return FSlateColor(FLinearColor(R, G, B, A));
}

LUA_GLUE_BEGIN(FSlateColor)
LUA_GLUE_FUNCTION_OUT(Temp, FSlateColor_New_Out, 0, 0, 0, 1)
LUA_GLUE_FUNCTION_OUT(New, FSlateColor_New_Out, 0, 0, 0, 1)
LUA_GLUE_END()

LUA_GLUE_BEGIN(FMD5)
LUA_GLUE_FUNCTION(HashAnsiString)
LUA_GLUE_END()


static FVector2D GetVirtualDesktopPixelToViewport(UGameViewportClient* viewportClient, FVector2D touchPos/*, FVector2D viewportSize*/)
{
	FSceneViewport* Viewport = viewportClient->GetGameViewport();
	FVector2D viewportSize;
	viewportClient->GetViewportSize(viewportSize);
	return Viewport->VirtualDesktopPixelToViewport(FIntPoint((int32)touchPos.X, (int32)touchPos.Y)) * viewportSize;
}

LUA_GLUE_BEGIN(FSceneViewport)
//LUA_GLUE_FUNCTION(VirtualDesktopPixelToViewport)
LUA_GLUE_FUNCTION_OUT(GetVirtualDesktopPixelToViewport, GetVirtualDesktopPixelToViewport)
LUA_GLUE_END()


static FEventReply Handled()
{
	static FEventReply TheReply(true);
	return TheReply;
}

static FEventReply Unhandled()
{
	static FEventReply TheReply(false);
	return TheReply;
}

LUA_GLUE_BEGIN(FEventReply)
LUA_GLUE_TEMP(Temp, (bool), false)
LUA_GLUE_FUNCTION_OUT(Handled, Handled)
LUA_GLUE_FUNCTION_OUT(Unhandled, Unhandled)
LUA_GLUE_PROPERTY(NativeReply)
LUA_GLUE_END()

LUA_GLUE_BEGIN(IModuleInterface)
LUA_GLUE_END()

LUA_GLUE_BEGIN(FModuleManager)
LUA_GLUE_FUNCTION(Get)
LUA_GLUE_FUNCTION(LoadModule)
LUA_GLUE_OVERLOAD(LoadModuleChecked, IModuleInterface& (FModuleManager::*)(const FName InModuleName))
LUA_GLUE_END()

LUA_GLUE_BEGIN(FParse)
LUA_GLUE_FUNCTION(Param)
LUA_GLUE_END()

LUA_GLUE_BEGIN(FCommandLine)
LUA_GLUE_FUNCTION(Get)
LUA_GLUE_END()


LUA_GLUE_BEGIN(FInputEvent)
LUA_GLUE_FUNCTION(IsRepeat)
LUA_GLUE_FUNCTION(IsShiftDown)
LUA_GLUE_FUNCTION(IsLeftShiftDown)
LUA_GLUE_FUNCTION(IsRightShiftDown)
LUA_GLUE_FUNCTION(IsControlDown)
LUA_GLUE_FUNCTION(IsLeftControlDown)
LUA_GLUE_FUNCTION(IsRightControlDown)
LUA_GLUE_FUNCTION(IsAltDown)
LUA_GLUE_FUNCTION(IsLeftAltDown)
LUA_GLUE_FUNCTION(IsRightAltDown)
LUA_GLUE_FUNCTION(IsCommandDown)
LUA_GLUE_FUNCTION(AreCapsLocked)
LUA_GLUE_FUNCTION(GetUserIndex)
LUA_GLUE_FUNCTION(ToText)
LUA_GLUE_FUNCTION(IsPointerEvent)
LUA_GLUE_END()

LUA_GLUE_BEGIN(FKeyEvent)
LUA_GLUE_FUNCTION(GetKey)
LUA_GLUE_FUNCTION(GetCharacter)
LUA_GLUE_FUNCTION(GetKeyCode)
LUA_GLUE_END(FInputEvent)

LUA_GLUE_BEGIN(FPlatformTime)
LUA_GLUE_FUNCTION(Seconds)
LUA_GLUE_FUNCTION(Cycles)
LUA_GLUE_FUNCTION(Cycles64)
LUA_GLUE_END()

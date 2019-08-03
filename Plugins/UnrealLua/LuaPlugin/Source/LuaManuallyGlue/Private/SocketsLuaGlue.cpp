// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
#include "ILuaManuallyGlue.h"
#include "JustForLinkGlue.h"

void UJustForLinkGlue::LinkFixFunctino()
{
}

#include "HttpLuaGlue.h"
#include "UnrealLua.h"
#include "CoreMinimal.h"
#include "SocketSubsystem.h"
#include "Sockets.h"
#include "IPAddress.h"

LUA_GLUE_BEGIN( FInternetAddr)
LUA_GLUE_OVERLOAD(SetIp, void (FInternetAddr::*)(uint32 InAddr))
LUA_GLUE_OVERLOAD(SetIp, void (FInternetAddr::*)(const TCHAR* InAddr, bool& bIsValid))
LUA_GLUE_FUNCTION(GetIp)
LUA_GLUE_FUNCTION(SetPort)
LUA_GLUE_FUNCTION(GetPort)
LUA_GLUE_END()

LUA_GLUE_SHARED(FInternetAddr, FInternetAddr)

static int32 FSocket_SendBufferTo(lua_State*inL)
{
	FSocket *TheSocket = (FSocket *)tovoid(inL, 1);
	size_t bufferLen = 0;
	const char* bufstr = (const char*)luaL_checklstring(inL, 2, &bufferLen);
	FInternetAddr* Destination = (FInternetAddr*)tovoid(inL, 3);
	int32 SendedLen = 0;
	bool IsSend = TheSocket->SendTo((const uint8*)bufstr, bufferLen, SendedLen, *Destination);
	UTableUtil::push(inL, IsSend);
	UTableUtil::push(inL, SendedLen);
	return 2;
}

static int32 FSocket_SendBuffer(lua_State*inL)
{
	FSocket *TheSocket = (FSocket *)tovoid(inL, 1);
	size_t bufferLen = 0;
	const char* bufstr = (const char*)luaL_checklstring(inL, 2, &bufferLen);
	int32 SendedLen = 0;
	bool IsSend = TheSocket->Send((const uint8*)bufstr, bufferLen, SendedLen);
	UTableUtil::push(inL, IsSend);
	UTableUtil::push(inL, SendedLen);
	return 2;
}

static int32 FSocket_RecvFrom(lua_State*inL)
{
	const int32 MaxLen = 512;
	FSocket *TheSocket = (FSocket *)tovoid(inL, 1);
	uint8 DataBuffer[MaxLen];
	int32 ReadLen = 0;
	ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get();
	TSharedRef<FInternetAddr> FromAddr = SocketSubsystem->CreateInternetAddr();
	bool Result = TheSocket->RecvFrom(DataBuffer, MaxLen, ReadLen, *FromAddr);
	UTableUtil::push(inL, Result);
	const char* DataRef = (const char*)(&DataBuffer[0]);
	LuaStringBuffer buffer{ DataRef, ReadLen };
	UTableUtil::push(inL, buffer);
	return 2;
}

static int32 FSocket_Recv(lua_State*inL)
{
	const int32 MaxLen = 512;
	FSocket *TheSocket = (FSocket *)tovoid(inL, 1);
	uint8 DataBuffer[MaxLen];
	int32 ReadLen = 0;
	ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get();
	bool Result = TheSocket->Recv(DataBuffer, MaxLen, ReadLen);
	UTableUtil::push(inL, Result);
	const char* DataRef = (const char*)(&DataBuffer[0]);
	LuaStringBuffer buffer{ DataRef, ReadLen };
	UTableUtil::push(inL, buffer);
	return 2;
}

LUA_GLUE_BEGIN(FSocket)
LUA_GLUE_FUNCTION(Close)
LUA_GLUE_FUNCTION(Bind)
LUA_GLUE_FUNCTION(Connect)
LUA_GLUE_FUNCTION(Listen)
LUA_GLUE_FUNCTION(WaitForPendingConnection)
LUA_GLUE_FUNCTION(HasPendingConnection)
LUA_GLUE_FUNCTION(HasPendingData)
LUA_GLUE_OVERLOAD(Accept, FSocket* (FSocket::*)(const FString& InSocketDescription))
LUA_GLUE_OVERLOAD(Accept, FSocket* (FSocket::*)(FInternetAddr& OutAddr, const FString& InSocketDescription))
LUA_GLUE_FUNCTION_OUT(SendTo, FSocket_SendBufferTo)
LUA_GLUE_FUNCTION_OUT(Send, FSocket_SendBuffer)
LUA_GLUE_FUNCTION_OUT(RecvFrom, FSocket_RecvFrom)
LUA_GLUE_FUNCTION_OUT(Recv, FSocket_Recv)
LUA_GLUE_FUNCTION(Wait)
LUA_GLUE_FUNCTION(GetAddress)
LUA_GLUE_FUNCTION(GetPeerAddress)
LUA_GLUE_FUNCTION(SetNonBlocking)
LUA_GLUE_FUNCTION(SetBroadcast)
// LUA_GLUE_FUNCTION(JoinMulticastGroup)
// LUA_GLUE_FUNCTION(LeaveMulticastGroup)
LUA_GLUE_FUNCTION(SetMulticastLoopback)
LUA_GLUE_FUNCTION(SetMulticastTtl)
LUA_GLUE_FUNCTION(SetReuseAddr)
LUA_GLUE_FUNCTION(SetLinger)
LUA_GLUE_FUNCTION(SetRecvErr)
LUA_GLUE_FUNCTION(SetSendBufferSize)
LUA_GLUE_FUNCTION(SetReceiveBufferSize)
LUA_GLUE_FUNCTION(GetPortNo)
LUA_GLUE_FUNCTION(GetSocketType)
LUA_GLUE_FUNCTION(GetDescription)
LUA_GLUE_END()

static ISocketSubsystem* ISocketSubsystem_Get()
{
	return ISocketSubsystem::Get();
}

LUA_GLUE_BEGIN( ISocketSubsystem)
LUA_GLUE_FUNCTION_OUT(Get, ISocketSubsystem_Get)
LUA_GLUE_OVERLOAD(CreateSocket, FSocket* (ISocketSubsystem::*)(const FName&, const FString&, bool bForceUDP), false)
LUA_GLUE_FUNCTION(CreateInternetAddr)
LUA_GLUE_FUNCTION(BindNextPort)
LUA_GLUE_END()

LUA_GLUE_INTERFACE(FInternationalization, FInternationalization)
LUA_GLUE_FUNCTION(Get)
LUA_GLUE_FUNCTION(SetCurrentCulture)
LUA_GLUE_END()
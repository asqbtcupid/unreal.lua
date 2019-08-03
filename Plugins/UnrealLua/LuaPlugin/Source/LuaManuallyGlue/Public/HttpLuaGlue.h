// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
#include "CoreMinimal.h"
#include "JustForLinkGlue.h"
#include "UnrealLua.h"
#include "IHttpBase.h"
#include "IHttpRequest.h"
#include "HttpModule.h"
#include "IHttpResponse.h"
#include "BaseEngineLuaGlue.h"

LUA_GLUE_BEGIN_NAME(IHttpBase, IHttpBase)
LUA_GLUE_FUNCTION(GetContent)
LUA_GLUE_FUNCTION(GetContentLength)
LUA_GLUE_FUNCTION(GetAllHeaders)
LUA_GLUE_FUNCTION(GetHeader)
LUA_GLUE_FUNCTION(GetURLParameter)
LUA_GLUE_FUNCTION(GetURL)
LUA_GLUE_END()

LUA_GLUE_BEGIN_NAME(IHttpRequest, IHttpRequest)
LUA_GLUE_FUNCTION(SetVerb)
LUA_GLUE_FUNCTION(SetURL)
LUA_GLUE_FUNCTION(ProcessRequest)
LUA_GLUE_FUNCTION(OnProcessRequestComplete)
LUA_GLUE_END(IHttpBase)

LUA_GLUE_BEGIN_NAME(IHttpResponse, IHttpResponse)
LUA_GLUE_FUNCTION(GetContentAsString)
LUA_GLUE_FUNCTION(GetResponseCode)
LUA_GLUE_END(IHttpBase)

LUA_GLUE_SHARED(IHttpResponse, IHttpResponse, ESPMode::ThreadSafe)
LUA_GLUE_SHARED(IHttpRequest, IHttpRequest)
LUA_GLUE_DELEGATE(FHttpRequestCompleteDelegate, FHttpRequestCompleteDelegate)

LUA_GLUE_INTERFACE(FHttpModule, FHttpModule)
LUA_GLUE_FUNCTION(Get)
LUA_GLUE_FUNCTION(CreateRequest)
LUA_GLUE_FUNCTION(GetHttpTimeout)
LUA_GLUE_FUNCTION(SetHttpTimeout)
LUA_GLUE_FUNCTION(GetHttpConnectionTimeout)
LUA_GLUE_FUNCTION(GetHttpReceiveTimeout)
LUA_GLUE_FUNCTION(GetHttpMaxConnectionsPerServer)
LUA_GLUE_FUNCTION(GetHttpSendTimeout)
LUA_GLUE_FUNCTION(GetMaxReadBufferSize)
LUA_GLUE_FUNCTION(SetMaxReadBufferSize)
LUA_GLUE_FUNCTION(IsHttpEnabled)
LUA_GLUE_FUNCTION(ToggleNullHttp)
LUA_GLUE_FUNCTION(GetHttpDelayTime)
LUA_GLUE_FUNCTION(SetHttpDelayTime)
LUA_GLUE_FUNCTION(GetHttpThreadActiveFrameTimeInSeconds)
LUA_GLUE_FUNCTION(SetHttpThreadActiveFrameTimeInSeconds)
LUA_GLUE_FUNCTION(GetHttpThreadActiveMinimumSleepTimeInSeconds)
LUA_GLUE_FUNCTION(SetHttpThreadActiveMinimumSleepTimeInSeconds)
LUA_GLUE_FUNCTION(GetHttpThreadIdleFrameTimeInSeconds)
LUA_GLUE_FUNCTION(SetHttpThreadIdleFrameTimeInSeconds)
LUA_GLUE_FUNCTION(GetHttpThreadIdleMinimumSleepTimeInSeconds)
LUA_GLUE_FUNCTION(SetHttpThreadIdleMinimumSleepTimeInSeconds)
LUA_GLUE_END()

#include "tableutil.h"
#include "AIRequestID.lua.h"
#include "AIMoveRequest.lua.h"
#include "PawnActionEvent.lua.h"
#include "PawnActionStack.lua.h"
#include "GenericTeamId.lua.h"
#include "BlackboardKeySelector.lua.h"
#include "AIDataProviderValue.lua.h"
#include "AIDataProviderTypedValue.lua.h"
#include "AIDataProviderStructValue.lua.h"
#include "AIDataProviderIntValue.lua.h"
#include "AIDataProviderFloatValue.lua.h"
#include "AIDataProviderBoolValue.lua.h"
#include "EnvNamedValue.lua.h"
#include "EnvDirection.lua.h"
#include "EnvTraceData.lua.h"
#include "EnvOverlapData.lua.h"
#include "EnvQueryResult.lua.h"
#include "AIDynamicParam.lua.h"
#include "EQSParametrizedQueryExecutionRequest.lua.h"
#include "EnvQueryRequest.lua.h"
#include "CrowdAvoidanceConfig.lua.h"
#include "CrowdAvoidanceSamplingPattern.lua.h"
#include "AIStimulus.lua.h"
#include "AISenseAffiliationFilter.lua.h"
#include "AIDamageEvent.lua.h"
#include "AINoiseEvent.lua.h"
#include "AIPredictionEvent.lua.h"
#include "AISightEvent.lua.h"
#include "AITeamStimulusEvent.lua.h"
#include "AITouchEvent.lua.h"
struct lua_static_load_AIModule_ustruct_all_struct
{
	static void load()
	{
		UTableUtil::loadstruct(FAIRequestID_Lib, "FAIRequestID");
		UTableUtil::loadstruct(FAIMoveRequest_Lib, "FAIMoveRequest");
		UTableUtil::loadstruct(FPawnActionEvent_Lib, "FPawnActionEvent");
		UTableUtil::loadstruct(FPawnActionStack_Lib, "FPawnActionStack");
		UTableUtil::loadstruct(FGenericTeamId_Lib, "FGenericTeamId");
		UTableUtil::loadstruct(FBlackboardKeySelector_Lib, "FBlackboardKeySelector");
		UTableUtil::loadstruct(FAIDataProviderValue_Lib, "FAIDataProviderValue");
		UTableUtil::loadstruct(FAIDataProviderTypedValue_Lib, "FAIDataProviderTypedValue");
		UTableUtil::loadstruct(FAIDataProviderStructValue_Lib, "FAIDataProviderStructValue");
		UTableUtil::loadstruct(FAIDataProviderIntValue_Lib, "FAIDataProviderIntValue");
		UTableUtil::loadstruct(FAIDataProviderFloatValue_Lib, "FAIDataProviderFloatValue");
		UTableUtil::loadstruct(FAIDataProviderBoolValue_Lib, "FAIDataProviderBoolValue");
		UTableUtil::loadstruct(FEnvNamedValue_Lib, "FEnvNamedValue");
		UTableUtil::loadstruct(FEnvDirection_Lib, "FEnvDirection");
		UTableUtil::loadstruct(FEnvTraceData_Lib, "FEnvTraceData");
		UTableUtil::loadstruct(FEnvOverlapData_Lib, "FEnvOverlapData");
		UTableUtil::loadstruct(FEnvQueryResult_Lib, "FEnvQueryResult");
		UTableUtil::loadstruct(FAIDynamicParam_Lib, "FAIDynamicParam");
		UTableUtil::loadstruct(FEQSParametrizedQueryExecutionRequest_Lib, "FEQSParametrizedQueryExecutionRequest");
		UTableUtil::loadstruct(FEnvQueryRequest_Lib, "FEnvQueryRequest");
		UTableUtil::loadstruct(FCrowdAvoidanceConfig_Lib, "FCrowdAvoidanceConfig");
		UTableUtil::loadstruct(FCrowdAvoidanceSamplingPattern_Lib, "FCrowdAvoidanceSamplingPattern");
		UTableUtil::loadstruct(FAIStimulus_Lib, "FAIStimulus");
		UTableUtil::loadstruct(FAISenseAffiliationFilter_Lib, "FAISenseAffiliationFilter");
		UTableUtil::loadstruct(FAIDamageEvent_Lib, "FAIDamageEvent");
		UTableUtil::loadstruct(FAINoiseEvent_Lib, "FAINoiseEvent");
		UTableUtil::loadstruct(FAIPredictionEvent_Lib, "FAIPredictionEvent");
		UTableUtil::loadstruct(FAISightEvent_Lib, "FAISightEvent");
		UTableUtil::loadstruct(FAITeamStimulusEvent_Lib, "FAITeamStimulusEvent");
		UTableUtil::loadstruct(FAITouchEvent_Lib, "FAITouchEvent");
	}
	lua_static_load_AIModule_ustruct_all_struct(){UTableUtil::GetInitDelegates().AddStatic(&load);}
};
static lua_static_load_AIModule_ustruct_all_struct lua_AIModule_ustruct_all_static_var;
void FixLinkFunc_AIModule_ustruct_all(){};

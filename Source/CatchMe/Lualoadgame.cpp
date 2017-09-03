#include "CatchMe.h"
#include "lualoadgame.h"
#include "tableutil.h"
#include "gameheader.inl"
#include "allweakclass.script.h"
#include "Actor.script.h"
#include "AnimInstance.script.h"
#include "GameInstance.script.h"
#include "Controller.script.h"
#include "Info.script.h"
#include "Pawn.script.h"
#include "PlayerController.script.h"
#include "GameModeBase.script.h"
#include "GameMode.script.h"
#include "Character.script.h"
#include "DefaultPawn.script.h"
#include "CatchMeAIController.script.h"
#include "CatchMeGameInstance.script.h"
#include "CMCharacterBase.script.h"
#include "CMFogMgr.script.h"
#include "CMGameModeBase.script.h"
#include "CMPlayerControllerBase.script.h"
#include "CMSpectatorPawn.script.h"
#include "Delegate_ACatchMeAIController_ReceiveMoveCompleted.script.h"
#include "LuaAnimInstance.script.h"
#include "Luautils.script.h"
#include "CatchMeCharacter.script.h"
#include "CatchMeGameMode.script.h"
#include "CatchMePlayerController.script.h"
#include "CharacterInfo.script.h"
#include "SkillInfo.script.h"
#include "ReplifetimeCond.script.h"

void ULuaLoadGame::LoadAll(lua_State* L)
{
	UTableUtil::loadlib(CatchMeAIController_Lib, "ACatchMeAIController");
	UTableUtil::loadlib(CatchMeGameInstance_Lib, "UCatchMeGameInstance");
	UTableUtil::loadlib(CMCharacterBase_Lib, "ACMCharacterBase");
	UTableUtil::loadlib(CMFogMgr_Lib, "UCMFogMgr");
	UTableUtil::loadlib(CMGameModeBase_Lib, "ACMGameModeBase");
	UTableUtil::loadlib(CMPlayerControllerBase_Lib, "ACMPlayerControllerBase");
	UTableUtil::loadlib(CMSpectatorPawn_Lib, "ACMSpectatorPawn");
	UTableUtil::loadlib(Delegate_ACatchMeAIController_ReceiveMoveCompleted_Lib, "UDelegate_ACatchMeAIController_ReceiveMoveCompleted");
	UTableUtil::loadlib(LuaAnimInstance_Lib, "ULuaAnimInstance");
	UTableUtil::loadlib(Luautils_Lib, "ULuautils");
	UTableUtil::loadlib(CatchMeCharacter_Lib, "ACatchMeCharacter");
	UTableUtil::loadlib(CatchMeGameMode_Lib, "ACatchMeGameMode");
	UTableUtil::loadlib(CatchMePlayerController_Lib, "ACatchMePlayerController");
	UTableUtil::loadlib(FCharacterInfo_Lib, "FCharacterInfo");
	UTableUtil::loadlib(FSkillInfo_Lib, "FSkillInfo");
	UTableUtil::loadlib(FReplifetimeCond_Lib, "FReplifetimeCond");
}


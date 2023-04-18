#ifndef BATTLE_ROYALE_MODE_H
#define BATTLE_ROYALE_MODE_H

#include "command_type.h"
#include <queue>

extern bool _battle_royale;
extern std::queue<CompanyID> _eliminated_companies;

void BrmLoadResetMode(bool new_value);

void BrmProcessBuyCompanyShare(CompanyID target_company);

void BrmProcessGameTick();

void CcBrmAnnounce(Commands cmd, const CommandCost &result,
				   const std::string&, const std::string&, CompanyID id);

CommandCost CmdBattleRoyaleModeCountdown(DoCommandFlag flags, byte timeout);
DEF_CMD_TRAIT(CMD_BATTLE_ROYALE_MODE_COUNTDOWN, CmdBattleRoyaleModeCountdown, CMD_SERVER | CMD_NO_EST, CMDT_SERVER_SETTING)

CommandCost CmdEnterBattleRoyaleMode(DoCommandFlag flags, bool mode);
DEF_CMD_TRAIT(CMD_ENTER_BATTLE_ROYALE_MODE, CmdEnterBattleRoyaleMode, CMD_SERVER | CMD_NO_EST, CMDT_SERVER_SETTING)

#endif // BATTLE_ROYALE_MODE_H

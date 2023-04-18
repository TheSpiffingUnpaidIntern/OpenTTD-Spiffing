#include "stdafx.h"
#include "network/network_base.h"
#include "network/network_server.h"
#include "network/network_admin.h"
#include "command_type.h"
#include "company_cmd.h"
#include "company_base.h"
#include "news_type.h"
#include "news_func.h"
#include "ai/ai.hpp"
#include "game/game.hpp"
#include "story_base.h"
#include "goal_base.h"
#include "window_func.h"
#include "strings_func.h"
#include "misc_cmd.h"
#include "error.h"
#include "battle_royale_mode.h"

#include <chrono>

bool _battle_royale;
std::queue<CompanyID> _eliminated_companies;

static bool showed_game_over = false;
static int timeout = -1;
static std::chrono::steady_clock::time_point brm_start = {};

void BrmLoadResetMode(bool new_value)
{
	_battle_royale = new_value;
	if (_battle_royale) {
		showed_game_over = false;
	}
}

CommandCost CmdEnterBattleRoyaleMode(DoCommandFlag flags, bool mode)
{
	if (_battle_royale == mode) {
		return CommandCost();
	}
	_battle_royale = mode;
	timeout = _battle_royale?10:-1;
	brm_start = std::chrono::steady_clock::now();
	if (_battle_royale) {
		showed_game_over = false;
	}
	return CommandCost();
}

CommandCost CmdBattleRoyaleModeCountdown(DoCommandFlag flags, byte timeout)
{
	if (timeout == 0) {
		HideActiveErrorMessage();
		return CommandCost();
	}
	std::string message;
	if (timeout < 2) {
		message = "GO!";
	} else if (timeout < 5) {
		message = "Get set";
	} else {
		message = "On your marks";
	}
	SetDParamStr(0, message);
	ShowErrorMessage(STR_BATTLE_ROYALE_COUNTDOWN_CAPTION, STR_BATTLE_ROYALE_COUNTDOWN_MESSAGE1, WL_INFO);
	return CommandCost();
}

void BrmProcessBuyCompanyShare(CompanyID target_company)
{
	if (_networking && ! _network_server) {
		if (_local_company == target_company) {
			std::string res;
			res += std::string("Company ") + std::to_string(_current_company) + " bought your shares.";
			Command<CMD_ANNOUNCE>::Do(DC_EXEC, "Shares Bought", res, _local_company);
		}
		return;
	}
	if (!_battle_royale) {
		return;
	}
	Company *c = Company::GetIfValid(target_company);
	if (c == nullptr) {
		return;
	}

	uint count = 0;

	for (auto &&so : c->share_owners) {
		if (so != INVALID_OWNER) {
			count++;
		}
	}
	const float percentage = (float)count/(float)MAX_COMPANY_SHARE_OWNERS; // who knows...

	if (percentage < 0.75) {
		return;
	}

	_eliminated_companies.push(target_company);
}

static int announced = 0;
void CcBrmAnnounce(Commands cmd, const CommandCost &result,
				   const std::string &caption, const std::string &message, CompanyID id)
{
	announced = _frame_counter_max + 1;
}

void BrmProcessGameTick()
{
	static std::chrono::steady_clock::duration last_tick = {};
	if (_networking && !_network_server) {
		return;
	}
	if (_battle_royale && Company::GetNumItems() == 1) {
		if (!showed_game_over) {
			Command<CMD_PAUSE>::Post(PM_PAUSED_NORMAL, true);
			Command<CMD_ANNOUNCE>::Post("Game Over",
										"Only one company left",
										INVALID_COMPANY);
			showed_game_over = true;
		}
	}
	while (!_eliminated_companies.empty()) {
		auto target_company = _eliminated_companies.front();
		static bool posted = false;
		if (!posted) {
			std::string res;
			res += std::string("Company ") + std::to_string(target_company+1) + " has been eliminated.";
			Command<CMD_ANNOUNCE>::Post("Company Eliminated", res, INVALID_COMPANY);
			Command<CMD_ANNOUNCE>::Post(CcBrmAnnounce, std::string("You are out"), std::string("Your company was eliminated!"), target_company);
			announced = -1;
			posted = true;
			return;
		}
		if (_network_server && posted && (announced == -1 || _frame_counter <= announced)) {
			return;
		}
		posted = false;
		_eliminated_companies.pop();
		if (_network_server) {
			for (const NetworkClientInfo *ci : NetworkClientInfo::Iterate()) {
				if (ci->client_playas != target_company) {
					continue;
				}
				NetworkServerDoMove(ci->client_id, COMPANY_SPECTATOR);
			}
		}
		Command<CMD_COMPANY_CTRL>::Post(CCA_DELETE, target_company, CRR_MANUAL, INVALID_CLIENT_ID);
	}
	if (!_networking) {
		return;
	}
	if (timeout == -1) {
		return;
	}

	std::chrono::steady_clock::duration tick = std::chrono::steady_clock::now() - brm_start;
	if (std::chrono::duration_cast<std::chrono::seconds>(last_tick) == std::chrono::duration_cast<std::chrono::seconds>(tick)) return;
	last_tick = tick;
	timeout--;

	Command<CMD_BATTLE_ROYALE_MODE_COUNTDOWN>::Post(timeout);
	if (timeout == 0) {
		timeout = -1;
		Command<CMD_PAUSE>::Post(PM_PAUSED_NORMAL, false);
		return;
	}
}

void BrmProcessGameEnd(bool last_company)
{

}

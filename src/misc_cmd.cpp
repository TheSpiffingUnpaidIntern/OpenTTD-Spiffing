/*
 * This file is part of OpenTTD.
 * OpenTTD is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * OpenTTD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with OpenTTD. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file misc_cmd.cpp Some misc functions that are better fitted in other files, but never got moved there... */

#include "stdafx.h"
#include "command_func.h"
#include "economy_func.h"
#include "window_func.h"
#include "textbuf_gui.h"
#include "network/network.h"
#include "network/network_func.h"
#include "strings_func.h"
#include "company_func.h"
#include "company_gui.h"
#include "company_base.h"
#include "tile_map.h"
#include "texteff.hpp"
#include "core/backup_type.hpp"
#include "misc_cmd.h"
#include "error.h"

#include "table/strings.h"

#include "safeguards.h"

/**
 * Increase the loan of your company.
 * @param flags operation to perform
 * @param cmd when LoanCommand::Interval: loans LOAN_INTERVAL,
 *            when LoanCommand::Max: loans the maximum loan permitting money (press CTRL),
 *            when LoanCommand::Amount: loans the amount specified in \c amount
 * @param amount amount to increase the loan with, multitude of LOAN_INTERVAL. Only used when cmd == LoanCommand::Amount.
 * @return the cost of this operation or an error
 */
CommandCost CmdIncreaseLoan(DoCommandFlag flags, LoanCommand cmd, Money amount)
{
	Company *c = Company::Get(_current_company);

	if (c->current_loan >= _economy.max_loan) {
		SetDParam(0, _economy.max_loan);
		return_cmd_error(STR_ERROR_MAXIMUM_PERMITTED_LOAN);
	}

	Money loan;
	switch (cmd) {
		default: return CMD_ERROR; // Invalid method
		case LoanCommand::Interval: // Take some extra loan
			loan = LOAN_INTERVAL;
			break;
		case LoanCommand::Max: // Take a loan as big as possible
			loan = _economy.max_loan - c->current_loan;
			break;
		case LoanCommand::Amount: // Take the given amount of loan
			loan = amount;
			if (loan < LOAN_INTERVAL || c->current_loan + loan > _economy.max_loan || loan % LOAN_INTERVAL != 0) return CMD_ERROR;
			break;
	}

	/* In case adding the loan triggers the overflow protection of Money,
	 * we would essentially be losing money as taking and repaying the loan
	 * immediately would not get us back to the same bank balance anymore. */
	if (c->money > Money::max() - loan) return CMD_ERROR;

	if (flags & DC_EXEC) {
		c->money        += loan;
		c->current_loan += loan;
		InvalidateCompanyWindows(c);
	}

	return CommandCost(EXPENSES_OTHER);
}

/**
 * Decrease the loan of your company.
 * @param flags operation to perform
 * @param cmd when LoanCommand::Interval: pays back LOAN_INTERVAL,
 *            when LoanCommand::Max: pays back the maximum loan permitting money (press CTRL),
 *            when LoanCommand::Amount: pays back the amount specified in \c amount
 * @param amount amount to decrease the loan with, multitude of LOAN_INTERVAL. Only used when cmd == LoanCommand::Amount.
 * @return the cost of this operation or an error
 */
CommandCost CmdDecreaseLoan(DoCommandFlag flags, LoanCommand cmd, Money amount)
{
	Company *c = Company::Get(_current_company);

	if (c->current_loan == 0) return_cmd_error(STR_ERROR_LOAN_ALREADY_REPAYED);

	Money loan;
	switch (cmd) {
		default: return CMD_ERROR; // Invalid method
		case LoanCommand::Interval: // Pay back one step
			loan = std::min(c->current_loan, (Money)LOAN_INTERVAL);
			break;
		case LoanCommand::Max: // Pay back as much as possible
			loan = std::max(std::min(c->current_loan, c->money), (Money)LOAN_INTERVAL);
			loan -= loan % LOAN_INTERVAL;
			break;
		case LoanCommand::Amount: // Repay the given amount of loan
			loan = amount;
			if (loan % LOAN_INTERVAL != 0 || loan < LOAN_INTERVAL || loan > c->current_loan) return CMD_ERROR; // Invalid amount to loan
			break;
	}

	if (c->money < loan) {
		SetDParam(0, loan);
		return_cmd_error(STR_ERROR_CURRENCY_REQUIRED);
	}

	if (flags & DC_EXEC) {
		c->money        -= loan;
		c->current_loan -= loan;
		InvalidateCompanyWindows(c);
	}
	return CommandCost();
}

/**
 * In case of an unsafe unpause, we want the
 * user to confirm that it might crash.
 * @param w         unused
 * @param confirmed whether the user confirmed their action
 */
static void AskUnsafeUnpauseCallback(Window *w, bool confirmed)
{
	if (confirmed) {
		Command<CMD_PAUSE>::Post(PM_PAUSED_ERROR, false);
	}
}

/**
 * Pause/Unpause the game (server-only).
 * Set or unset a bit in the pause mode. If pause mode is zero the game is
 * unpaused. A bitset is used instead of a boolean value/counter to have
 * more control over the game when saving/loading, etc.
 * @param flags operation to perform
 * @param mode the pause mode to change
 * @param pause true pauses, false unpauses this mode
 * @return the cost of this operation or an error
 */
CommandCost CmdPause(DoCommandFlag flags, PauseMode mode, bool pause)
{
	switch (mode) {
		case PM_PAUSED_SAVELOAD:
		case PM_PAUSED_ERROR:
		case PM_PAUSED_NORMAL:
		case PM_PAUSED_GAME_SCRIPT:
		case PM_PAUSED_LINK_GRAPH:
			break;

		case PM_PAUSED_JOIN:
		case PM_PAUSED_ACTIVE_CLIENTS:
			if (!_networking) return CMD_ERROR;
			break;

		default: return CMD_ERROR;
	}
	if (flags & DC_EXEC) {
		if (mode == PM_PAUSED_NORMAL && _pause_mode & PM_PAUSED_ERROR) {
			ShowQuery(
				STR_NEWGRF_UNPAUSE_WARNING_TITLE,
				STR_NEWGRF_UNPAUSE_WARNING,
				nullptr,
				AskUnsafeUnpauseCallback
			);
		} else {
			PauseMode prev_mode = _pause_mode;

			if (pause) {
				_pause_mode |= mode;
			} else {
				_pause_mode &= ~mode;
			}

			NetworkHandlePauseChange(prev_mode, mode);
		}

		SetWindowDirty(WC_STATUS_BAR, 0);
		SetWindowDirty(WC_MAIN_TOOLBAR, 0);
	}
	return CommandCost();
}

/**
 * Change the financial flow of your company.
 * @param flags operation to perform
 * @param amount the amount of money to receive (if positive), or spend (if negative)
 * @return the cost of this operation or an error
 */
CommandCost CmdMoneyCheat(DoCommandFlag flags, Money amount)
{
	return CommandCost(EXPENSES_OTHER, -amount);
}

/**
 * Change the bank bank balance of a company by inserting or removing money without affecting the loan.
 * @param flags operation to perform
 * @param tile tile to show text effect on (if not 0)
 * @param delta the amount of money to receive (if positive), or spend (if negative)
 * @param company the company ID.
 * @param expenses_type the expenses type which should register the cost/income @see ExpensesType.
 * @return zero cost or an error
 */
CommandCost CmdChangeBankBalance(DoCommandFlag flags, TileIndex tile, Money delta, CompanyID company, ExpensesType expenses_type)
{
	if (!Company::IsValidID(company)) return CMD_ERROR;
	if (expenses_type >= EXPENSES_END) return CMD_ERROR;
	if (_current_company != OWNER_DEITY) return CMD_ERROR;

	if (flags & DC_EXEC) {
		/* Change company bank balance of company. */
		Backup<CompanyID> cur_company(_current_company, company, FILE_LINE);
		SubtractMoneyFromCompany(CommandCost(expenses_type, -delta));
		cur_company.Restore();

		if (tile != 0) {
			ShowCostOrIncomeAnimation(TileX(tile) * TILE_SIZE, TileY(tile) * TILE_SIZE, GetTilePixelZ(tile), -delta);
		}
	}

	/* This command doesn't cost anything for deity. */
	CommandCost zero_cost(expenses_type, 0);
	return zero_cost;
}

#include "window_type.h"
#include "window_gui.h"
#include "window_func.h"
#include "string_func.h"
#include "core/geometry_type.hpp"
#include "guitimer_func.h"
#include "landscape.h"
#include "zoom_func.h"
enum AnnouncementWidgets {
	WID_AM_CAPTION, ///< Caption of the window.
	WID_AM_MESSAGE, ///< Error message.
};
static const NWidgetPart _nested_announce_widgets[] = {
	NWidget(NWID_HORIZONTAL),
		NWidget(WWT_CLOSEBOX, COLOUR_BLUE),
		NWidget(WWT_CAPTION, COLOUR_BLUE, WID_AM_CAPTION), SetDataTip(STR_ERROR_MESSAGE_CAPTION, STR_NULL),
	EndContainer(),
	NWidget(WWT_PANEL, COLOUR_BLUE),
		NWidget(WWT_EMPTY, COLOUR_BLUE, WID_AM_MESSAGE), SetPadding(WidgetDimensions::unscaled.modalpopup), SetFill(1, 0), SetMinimalSize(236, 0),
	EndContainer(),
};

static WindowDesc _announce_desc(
	WDP_MANUAL, "error", 0, 0,
	WC_ANNOUNCEMENT, WC_NONE,
	0,
	_nested_announce_widgets, lengthof(_nested_announce_widgets)
);

class AnnouncementData {
protected:
	GUITimer display_timer;         ///< Timer before closing the message.
	uint64 decode_params[20];       ///< Parameters of the message strings.
	const char *strings[20];        ///< Copies of raw strings that were used.
	StringID summary_msg;           ///< General error message showed in first line. Must be valid.
	StringID detailed_msg;          ///< Detailed error message showed in second line. Can be #INVALID_STRING_ID.
	Point position;                 ///< Position of the error message window.

public:
	AnnouncementData(const AnnouncementData &data);
	~AnnouncementData();
	AnnouncementData(StringID summary_msg, StringID detailed_msg, uint duration = 0, int x = 0, int y = 0);

	/* Remove the copy assignment, as the default implementation will not do the right thing. */
	AnnouncementData &operator=(AnnouncementData &rhs) = delete;

	void SetDParam(uint n, uint64 v);
	void SetDParamStr(uint n, const char *str);
	void SetDParamStr(uint n, const std::string &str);

	void CopyOutDParams();
};

/**
 * Copy the given data into our instance.
 * @param data The data to copy.
 */
AnnouncementData::AnnouncementData(const AnnouncementData &data) :
	display_timer(data.display_timer), summary_msg(data.summary_msg),
	detailed_msg(data.detailed_msg), position(data.position)
{
	memcpy(this->decode_params, data.decode_params, sizeof(this->decode_params));
	memcpy(this->strings,       data.strings,       sizeof(this->strings));
	for (size_t i = 0; i < lengthof(this->strings); i++) {
		if (this->strings[i] != nullptr) {
			this->strings[i] = stredup(this->strings[i]);
			this->decode_params[i] = (size_t)this->strings[i];
		}
	}
}

/** Free all the strings. */
AnnouncementData::~AnnouncementData()
{
	for (size_t i = 0; i < lengthof(this->strings); i++) free(this->strings[i]);
}

/**
 * Display an error message in a window.
 * @param summary_msg  General error message showed in first line. Must be valid.
 * @param detailed_msg Detailed error message showed in second line. Can be INVALID_STRING_ID.
 * @param duration     The amount of time to show this error message.
 * @param x            World X position (TileVirtX) of the error location. Set both x and y to 0 to just center the message when there is no related error tile.
 * @param y            World Y position (TileVirtY) of the error location. Set both x and y to 0 to just center the message when there is no related error tile.
 * @param textref_stack_grffile NewGRF that provides the #TextRefStack for the error message.
 * @param textref_stack_size Number of uint32 values to put on the #TextRefStack for the error message; 0 if the #TextRefStack shall not be used.
 * @param textref_stack Values to put on the #TextRefStack.
 */
AnnouncementData::AnnouncementData(StringID summary_msg, StringID detailed_msg, uint duration, int x, int y) :
	summary_msg(summary_msg),
	detailed_msg(detailed_msg)
{
	this->position.x = x;
	this->position.y = y;

	memset(this->decode_params, 0, sizeof(this->decode_params));
	memset(this->strings, 0, sizeof(this->strings));

	assert(summary_msg != INVALID_STRING_ID);

	this->display_timer.SetInterval(duration * 3000);
}

/**
 * Copy error parameters from current DParams.
 */
void AnnouncementData::CopyOutDParams()
{
	/* Reset parameters */
	for (size_t i = 0; i < lengthof(this->strings); i++) free(this->strings[i]);
	memset(this->decode_params, 0, sizeof(this->decode_params));
	memset(this->strings, 0, sizeof(this->strings));

	/* Get parameters using type information */
	CopyOutDParam(&this->decode_params[0], &this->strings[0], this->detailed_msg, 2);
// 	CopyOutDParam(&this->decode_params[1], &this->strings[1], this->summary_msg, 1);
}

/**
 * Set a error string parameter.
 * @param n Parameter index
 * @param v Parameter value
 */
void AnnouncementData::SetDParam(uint n, uint64 v)
{
	this->decode_params[n] = v;
}

/**
 * Set a rawstring parameter.
 * @param n Parameter index
 * @param str Raw string
 */
void AnnouncementData::SetDParamStr(uint n, const char *str)
{
	free(this->strings[n]);
	this->strings[n] = stredup(str);
}

/**
 * Set a rawstring parameter.
 * @param n Parameter index
 * @param str Raw string
 */
void AnnouncementData::SetDParamStr(uint n, const std::string &str)
{
	this->SetDParamStr(n, str.c_str());
}


struct AnnouncementWindow : public Window, AnnouncementData {
private:
	uint height_summary;            ///< Height of the #summary_msg string in pixels in the #WID_AM_MESSAGE widget.
	uint height_detailed;           ///< Height of the #detailed_msg string in pixels in the #WID_AM_MESSAGE widget.

public:
	AnnouncementWindow(const AnnouncementData &data) : Window(&_announce_desc), AnnouncementData(data)
	{
		this->InitNested();
	}

	void UpdateWidgetSize(int widget, Dimension *size, const Dimension &padding, Dimension *fill, Dimension *resize) override
	{
		switch (widget) {
			case WID_AM_MESSAGE: {
				CopyInDParam(0, &this->decode_params[0], 1);

				this->height_summary = GetStringHeight(this->summary_msg, size->width);
				CopyInDParam(0, &this->decode_params[1], 1);
				this->height_detailed = (this->detailed_msg == INVALID_STRING_ID) ? 0 : GetStringHeight(this->detailed_msg, size->width);

				uint panel_height = this->height_summary;
				if (this->detailed_msg != INVALID_STRING_ID) panel_height += this->height_detailed + WidgetDimensions::scaled.vsep_wide;

				size->height = std::max(size->height, panel_height);
				break;
			}
		}
	}

	Point OnInitialPosition(int16 sm_width, int16 sm_height, int window_number) override
	{
		/* Position (0, 0) given, center the window. */
		if (this->position.x == 0 && this->position.y == 0) {
			Point pt = {(_screen.width - sm_width) >> 1, (_screen.height - sm_height) >> 1};
			return pt;
		}

		/* Find the free screen space between the main toolbar at the top, and the statusbar at the bottom.
		 * Add a fixed distance 20 to make it less cluttered.
		 */
		int scr_top = GetMainViewTop() + 20;
		int scr_bot = GetMainViewBottom() - 20;

		Point pt = RemapCoords(this->position.x, this->position.y, GetSlopePixelZOutsideMap(this->position.x, this->position.y));
		const Viewport *vp = FindWindowById(WC_MAIN_WINDOW, 0)->viewport;
		/* move x pos to opposite corner */
		pt.x = UnScaleByZoom(pt.x - vp->virtual_left, vp->zoom) + vp->left;
		pt.x = (pt.x < (_screen.width >> 1)) ? _screen.width - sm_width - 20 : 20; // Stay 20 pixels away from the edge of the screen.

		/* move y pos to opposite corner */
		pt.y = UnScaleByZoom(pt.y - vp->virtual_top, vp->zoom) + vp->top;
		pt.y = (pt.y < (_screen.height >> 1)) ? scr_bot - sm_height : scr_top;

		return pt;
	}

	/**
	 * Some data on this window has become invalid.
	 * @param data Information about the changed data.
	 * @param gui_scope Whether the call is done from GUI scope. You may not do everything when not in GUI scope. See #InvalidateWindowData() for details.
	 */
	void OnInvalidateData(int data = 0, bool gui_scope = true) override
	{
		/* If company gets shut down, while displaying an error about it, remove the error message. */
		this->Close();
	}

	void SetStringParameters(int widget) const override
	{
		if (widget == WID_AM_CAPTION) CopyInDParam(0, this->decode_params, lengthof(this->decode_params));
	}

	void DrawWidget(const Rect &r, int widget) const override
	{
		switch (widget) {

			case WID_AM_MESSAGE:
				CopyInDParam(0, &this->decode_params[0], 1);

				if (this->detailed_msg == INVALID_STRING_ID) {
					DrawStringMultiLine(r, this->summary_msg, TC_FROMSTRING, SA_CENTER);
				} else {
					/* Extra space when message is shorter than company face window */
					int extra = (r.Height() - this->height_summary - this->height_detailed - WidgetDimensions::scaled.vsep_wide) / 2;

					/* Note: NewGRF supplied error message often do not start with a colour code, so default to white. */
					DrawStringMultiLine(r.WithHeight(this->height_summary + extra, false), this->summary_msg, TC_WHITE, SA_CENTER);
				CopyInDParam(0, &this->decode_params[1], 1);
					DrawStringMultiLine(r.WithHeight(this->height_detailed + extra, true), this->detailed_msg, TC_WHITE, SA_CENTER);
				}

				break;

			default:
				break;
		}
	}

	void OnMouseLoop() override
	{
		/* Disallow closing the window too easily, if timeout is disabled */
		if (_right_button_down && !this->display_timer.HasElapsed()) this->Close();
	}

	void OnRealtimeTick(uint delta_ms) override
	{
		if (this->display_timer.CountElapsed(delta_ms) == 0) return;

		this->Close();
	}
};

void ShowAnnouncement(StringID summary_msg, const std::string summary,
					  StringID detailed_msg, std::string detailed,
					  int x = 0, int y = 0)
{
	if (_settings_client.gui.errmsg_duration == 0) return;
	AnnouncementData data(summary_msg, detailed_msg,  _settings_client.gui.errmsg_duration, x, y);
	data.SetDParamStr(0, summary);
	data.SetDParamStr(1, detailed);
// 	data.CopyOutDParams();
	AnnouncementWindow *w = (AnnouncementWindow*)FindWindowById(WC_ANNOUNCEMENT, 0);
	if (w != nullptr) {
		w->Close();
	}
	new AnnouncementWindow(data);
}

CommandCost CmdAnnounce(DoCommandFlag, const std::string &caption, const std::string &message, CompanyID target_company)
{
	if (_network_server) {
		return CommandCost(); // no need to push the message to ourselves
	}
	if (target_company != INVALID_COMPANY && target_company != _local_company) {
		return CommandCost();
	}
	SetDParamStr(0, caption);
	SetDParamStr(1, message);
	ShowAnnouncement(STR_WHITE_RAW_STRING, caption, STR_ANNOUNCEMENT_TEXT, message);
	return CommandCost();
}

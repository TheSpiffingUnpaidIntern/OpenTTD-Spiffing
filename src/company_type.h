/*
 * This file is part of OpenTTD.
 * OpenTTD is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * OpenTTD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with OpenTTD. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file company_type.h Types related to companies. */

#ifndef COMPANY_TYPE_H
#define COMPANY_TYPE_H

#include "core/enum_type.hpp"
#include "core/bitmath_func.hpp"

/**
 * Enum for all companies/owners.
 */
enum Owner : uint16 {
	/* All companies below MAX_COMPANIES are playable
	 * companies, above, they are special, computer controlled 'companies' */
	OWNER_BEGIN     = 0x00, ///< First owner
	COMPANY_FIRST   = 0x00, ///< First company, same as owner
	MAX_COMPANIES   = 0x1F5, ///< Maximum number of companies
	OLD_MAX_COMPANIES   = 0x0F, ///< Maximum number of companies
	OWNER_TOWN      = 0x20F, ///< A town owns the tile, or a town is expanding
	OLD_OWNER_TOWN      = 0x0F,
	OWNER_NONE      = 0x210, ///< The tile has no ownership
	OWNER_WATER     = 0x211, ///< The tile/execution is done by "water"
	OWNER_DEITY     = 0x212, ///< The object is owned by a superuser / goal script
	OWNER_END,              ///< Last + 1 owner
	INVALID_OWNER   = 0x2FF, ///< An invalid owner
	INVALID_COMPANY = 0x2FF, ///< An invalid company

	/* 'Fake' companies used for networks */
	COMPANY_INACTIVE_CLIENT = 0x2FD, ///< The client is joining
	COMPANY_NEW_COMPANY     = 0x2FE, ///< The client wants a new company
	COMPANY_SPECTATOR       = 0x2FF, ///< The client is spectating
};
DECLARE_POSTFIX_INCREMENT(Owner)

static const uint MAX_LENGTH_PRESIDENT_NAME_CHARS = 32; ///< The maximum length of a president name in characters including '\0'
static const uint MAX_LENGTH_COMPANY_NAME_CHARS   = 32; ///< The maximum length of a company name in characters including '\0'

static const uint MAX_HISTORY_QUARTERS            = 24; ///< The maximum number of quarters kept as performance's history
static const uint MAX_COMPANY_SHARE_OWNERS        =  4; ///< The maximum number of shares of a company that can be owned by another company.

/** Define basic enum properties */
template <> struct EnumPropsT<Owner> : MakeEnumPropsT<Owner, uint16, OWNER_BEGIN, OWNER_END, INVALID_OWNER> {};

typedef Owner CompanyID;

typedef Bitset<MAX_COMPANIES> CompanyMask;

struct Company;
typedef uint32 CompanyManagerFace; ///< Company manager face bits, info see in company_manager_face.h

/** The reason why the company was removed. */
enum CompanyRemoveReason : uint8 {
	CRR_MANUAL,    ///< The company is manually removed.
	CRR_AUTOCLEAN, ///< The company is removed due to autoclean.
	CRR_BANKRUPT,  ///< The company went belly-up.

	CRR_END,       ///< Sentinel for end.

	CRR_NONE = CRR_MANUAL, ///< Dummy reason for actions that don't need one.
};

/** The action to do with CMD_COMPANY_CTRL. */
enum CompanyCtrlAction : uint8 {
	CCA_NEW,    ///< Create a new company.
	CCA_NEW_AI, ///< Create a new AI company.
	CCA_DELETE, ///< Delete a company.

	CCA_END,    ///< Sentinel for end.
};

#endif /* COMPANY_TYPE_H */

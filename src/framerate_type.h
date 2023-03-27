/*
* This file is part of OpenTTD.
* OpenTTD is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
* OpenTTD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
* See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with OpenTTD. If not, see <http://www.gnu.org/licenses/>.
*/

/** @file framerate_type.h
 * Types for recording game performance data.
 *
 * @par Adding new measurements
 * Adding a new measurement requires multiple steps, which are outlined here.
 * The first thing to do is add a new member of the #PerformanceElement enum.
 * It must be added before \c PFE_MAX and should be added in a logical place.
 * For example, an element of the game loop would be added next to the other game loop elements, and a rendering element next to the other rendering elements.
 *
 * @par
 * Second is adding a member to the \link anonymous_namespace{framerate_gui.cpp}::_pf_data _pf_data \endlink array, in the same position as the new #PerformanceElement member.
 *
 * @par
 * Third is adding strings for the new element. There is an array in #ConPrintFramerate with strings used for the console command.
 * Additionally, there are two sets of strings in \c english.txt for two GUI uses, also in the #PerformanceElement order.
 * Search for \c STR_FRAMERATE_GAMELOOP and \c STR_FRAMETIME_CAPTION_GAMELOOP in \c english.txt to find those.
 *
 * @par
 * Last is actually adding the measurements. There are two ways to measure, either one-shot (a single function/block handling all processing),
 * or as an accumulated element (multiple functions/blocks that need to be summed across each frame/tick).
 * Use either the PerformanceMeasurer or the PerformanceAccumulator class respectively for the two cases.
 * Either class is used by instantiating an object of it at the beginning of the block to be measured, so it auto-destructs at the end of the block.
 * For PerformanceAccumulator, make sure to also call PerformanceAccumulator::Reset once at the beginning of a new frame. Usually the StateGameLoop function is appropriate for this.
 *
 * @see framerate_gui.cpp for implementation
 */

#ifndef FRAMERATE_TYPE_H
#define FRAMERATE_TYPE_H

#include "stdafx.h"
#include "core/enum_type.hpp"

/**
 * Elements of game performance that can be measured.
 *
 * @note When adding new elements here, make sure to also update all other locations depending on the length and order of this enum.
 * See <em>Adding new measurements</em> above.
 */
enum PerformanceElement {
	PFE_FIRST = 0,
	PFE_GAMELOOP = 0,  ///< Speed of gameloop processing.
	PFE_GL_ECONOMY,    ///< Time spent processing cargo movement
	PFE_GL_TRAINS,     ///< Time spent processing trains
	PFE_GL_ROADVEHS,   ///< Time spend processing road vehicles
	PFE_GL_SHIPS,      ///< Time spent processing ships
	PFE_GL_AIRCRAFT,   ///< Time spent processing aircraft
	PFE_GL_LANDSCAPE,  ///< Time spent processing other world features
	PFE_GL_LINKGRAPH,  ///< Time spent waiting for link graph background jobs
	PFE_DRAWING,       ///< Speed of drawing world and GUI.
	PFE_DRAWWORLD,     ///< Time spent drawing world viewports in GUI
	PFE_VIDEO,         ///< Speed of painting drawn video buffer.
	PFE_SOUND,         ///< Speed of mixing audio samples
	PFE_ALLSCRIPTS,    ///< Sum of all GS/AI scripts
	PFE_GAMESCRIPT,    ///< Game script execution
	PFE_AI0,           ///< AI execution for player slot 1
	PFE_AI1,           ///< AI execution for player slot 2
	PFE_AI2,           ///< AI execution for player slot 3
	PFE_AI3,           ///< AI execution for player slot 4
	PFE_AI4,           ///< AI execution for player slot 5
	PFE_AI5,           ///< AI execution for player slot 6
	PFE_AI6,           ///< AI execution for player slot 7
	PFE_AI7,           ///< AI execution for player slot 8
	PFE_AI8,           ///< AI execution for player slot 9
	PFE_AI9,           ///< AI execution for player slot 10
	PFE_AI10,          ///< AI execution for player slot 11
	PFE_AI11,          ///< AI execution for player slot 12
	PFE_AI12,          ///< AI execution for player slot 13
	PFE_AI13,          ///< AI execution for player slot 14
	PFE_AI14,          ///< AI execution for player slot 15
	PFE_AI15,
	PFE_AI16,
	PFE_AI17,
	PFE_AI18,
	PFE_AI19,
	PFE_AI20,
	PFE_AI21,
	PFE_AI22,
	PFE_AI23,
	PFE_AI24,
	PFE_AI25,
	PFE_AI26,
	PFE_AI27,
	PFE_AI28,
	PFE_AI29,
	PFE_AI30,
	PFE_AI31,
	PFE_AI32,
	PFE_AI33,
	PFE_AI34,
	PFE_AI35,
	PFE_AI36,
	PFE_AI37,
	PFE_AI38,
	PFE_AI39,
	PFE_AI40,
	PFE_AI41,
	PFE_AI42,
	PFE_AI43,
	PFE_AI44,
	PFE_AI45,
	PFE_AI46,
	PFE_AI47,
	PFE_AI48,
	PFE_AI49,
	PFE_AI50,
	PFE_AI51,
	PFE_AI52,
	PFE_AI53,
	PFE_AI54,
	PFE_AI55,
	PFE_AI56,
	PFE_AI57,
	PFE_AI58,
	PFE_AI59,
	PFE_AI60,
	PFE_AI61,
	PFE_AI62,
	PFE_AI63,
	PFE_AI64,
	PFE_AI65,
	PFE_AI66,
	PFE_AI67,
	PFE_AI68,
	PFE_AI69,
	PFE_AI70,
	PFE_AI71,
	PFE_AI72,
	PFE_AI73,
	PFE_AI74,
	PFE_AI75,
	PFE_AI76,
	PFE_AI77,
	PFE_AI78,
	PFE_AI79,
	PFE_AI80,
	PFE_AI81,
	PFE_AI82,
	PFE_AI83,
	PFE_AI84,
	PFE_AI85,
	PFE_AI86,
	PFE_AI87,
	PFE_AI88,
	PFE_AI89,
	PFE_AI90,
	PFE_AI91,
	PFE_AI92,
	PFE_AI93,
	PFE_AI94,
	PFE_AI95,
	PFE_AI96,
	PFE_AI97,
	PFE_AI98,
	PFE_AI99,
	PFE_AI100,
	PFE_AI101,
	PFE_AI102,
	PFE_AI103,
	PFE_AI104,
	PFE_AI105,
	PFE_AI106,
	PFE_AI107,
	PFE_AI108,
	PFE_AI109,
	PFE_AI110,
	PFE_AI111,
	PFE_AI112,
	PFE_AI113,
	PFE_AI114,
	PFE_AI115,
	PFE_AI116,
	PFE_AI117,
	PFE_AI118,
	PFE_AI119,
	PFE_AI120,
	PFE_AI121,
	PFE_AI122,
	PFE_AI123,
	PFE_AI124,
	PFE_AI125,
	PFE_AI126,
	PFE_AI127,
	PFE_AI128,
	PFE_AI129,
	PFE_AI130,
	PFE_AI131,
	PFE_AI132,
	PFE_AI133,
	PFE_AI134,
	PFE_AI135,
	PFE_AI136,
	PFE_AI137,
	PFE_AI138,
	PFE_AI139,
	PFE_AI140,
	PFE_AI141,
	PFE_AI142,
	PFE_AI143,
	PFE_AI144,
	PFE_AI145,
	PFE_AI146,
	PFE_AI147,
	PFE_AI148,
	PFE_AI149,
	PFE_AI150,
	PFE_AI151,
	PFE_AI152,
	PFE_AI153,
	PFE_AI154,
	PFE_AI155,
	PFE_AI156,
	PFE_AI157,
	PFE_AI158,
	PFE_AI159,
	PFE_AI160,
	PFE_AI161,
	PFE_AI162,
	PFE_AI163,
	PFE_AI164,
	PFE_AI165,
	PFE_AI166,
	PFE_AI167,
	PFE_AI168,
	PFE_AI169,
	PFE_AI170,
	PFE_AI171,
	PFE_AI172,
	PFE_AI173,
	PFE_AI174,
	PFE_AI175,
	PFE_AI176,
	PFE_AI177,
	PFE_AI178,
	PFE_AI179,
	PFE_AI180,
	PFE_AI181,
	PFE_AI182,
	PFE_AI183,
	PFE_AI184,
	PFE_AI185,
	PFE_AI186,
	PFE_AI187,
	PFE_AI188,
	PFE_AI189,
	PFE_AI190,
	PFE_AI191,
	PFE_AI192,
	PFE_AI193,
	PFE_AI194,
	PFE_AI195,
	PFE_AI196,
	PFE_AI197,
	PFE_AI198,
	PFE_AI199,
	PFE_AI200,
	PFE_AI201,
	PFE_AI202,
	PFE_AI203,
	PFE_AI204,
	PFE_AI205,
	PFE_AI206,
	PFE_AI207,
	PFE_AI208,
	PFE_AI209,
	PFE_AI210,
	PFE_AI211,
	PFE_AI212,
	PFE_AI213,
	PFE_AI214,
	PFE_AI215,
	PFE_AI216,
	PFE_AI217,
	PFE_AI218,
	PFE_AI219,
	PFE_AI220,
	PFE_AI221,
	PFE_AI222,
	PFE_AI223,
	PFE_AI224,
	PFE_AI225,
	PFE_AI226,
	PFE_AI227,
	PFE_AI228,
	PFE_AI229,
	PFE_AI230,
	PFE_AI231,
	PFE_AI232,
	PFE_AI233,
	PFE_AI234,
	PFE_AI235,
	PFE_AI236,
	PFE_AI237,
	PFE_AI238,
	PFE_AI239,
	PFE_AI240,
	PFE_AI241,
	PFE_AI242,
	PFE_AI243,
	PFE_AI244,
	PFE_AI245,
	PFE_AI246,
	PFE_AI247,
	PFE_AI248,
	PFE_AI249,
	PFE_AI250,
	PFE_AI251,
	PFE_AI252,
	PFE_AI253,
	PFE_AI254,
	PFE_AI255,
	PFE_AI256,
	PFE_AI257,
	PFE_AI258,
	PFE_AI259,
	PFE_AI260,
	PFE_AI261,
	PFE_AI262,
	PFE_AI263,
	PFE_AI264,
	PFE_AI265,
	PFE_AI266,
	PFE_AI267,
	PFE_AI268,
	PFE_AI269,
	PFE_AI270,
	PFE_AI271,
	PFE_AI272,
	PFE_AI273,
	PFE_AI274,
	PFE_AI275,
	PFE_AI276,
	PFE_AI277,
	PFE_AI278,
	PFE_AI279,
	PFE_AI280,
	PFE_AI281,
	PFE_AI282,
	PFE_AI283,
	PFE_AI284,
	PFE_AI285,
	PFE_AI286,
	PFE_AI287,
	PFE_AI288,
	PFE_AI289,
	PFE_AI290,
	PFE_AI291,
	PFE_AI292,
	PFE_AI293,
	PFE_AI294,
	PFE_AI295,
	PFE_AI296,
	PFE_AI297,
	PFE_AI298,
	PFE_AI299,
	PFE_AI300,
	PFE_AI301,
	PFE_AI302,
	PFE_AI303,
	PFE_AI304,
	PFE_AI305,
	PFE_AI306,
	PFE_AI307,
	PFE_AI308,
	PFE_AI309,
	PFE_AI310,
	PFE_AI311,
	PFE_AI312,
	PFE_AI313,
	PFE_AI314,
	PFE_AI315,
	PFE_AI316,
	PFE_AI317,
	PFE_AI318,
	PFE_AI319,
	PFE_AI320,
	PFE_AI321,
	PFE_AI322,
	PFE_AI323,
	PFE_AI324,
	PFE_AI325,
	PFE_AI326,
	PFE_AI327,
	PFE_AI328,
	PFE_AI329,
	PFE_AI330,
	PFE_AI331,
	PFE_AI332,
	PFE_AI333,
	PFE_AI334,
	PFE_AI335,
	PFE_AI336,
	PFE_AI337,
	PFE_AI338,
	PFE_AI339,
	PFE_AI340,
	PFE_AI341,
	PFE_AI342,
	PFE_AI343,
	PFE_AI344,
	PFE_AI345,
	PFE_AI346,
	PFE_AI347,
	PFE_AI348,
	PFE_AI349,
	PFE_AI350,
	PFE_AI351,
	PFE_AI352,
	PFE_AI353,
	PFE_AI354,
	PFE_AI355,
	PFE_AI356,
	PFE_AI357,
	PFE_AI358,
	PFE_AI359,
	PFE_AI360,
	PFE_AI361,
	PFE_AI362,
	PFE_AI363,
	PFE_AI364,
	PFE_AI365,
	PFE_AI366,
	PFE_AI367,
	PFE_AI368,
	PFE_AI369,
	PFE_AI370,
	PFE_AI371,
	PFE_AI372,
	PFE_AI373,
	PFE_AI374,
	PFE_AI375,
	PFE_AI376,
	PFE_AI377,
	PFE_AI378,
	PFE_AI379,
	PFE_AI380,
	PFE_AI381,
	PFE_AI382,
	PFE_AI383,
	PFE_AI384,
	PFE_AI385,
	PFE_AI386,
	PFE_AI387,
	PFE_AI388,
	PFE_AI389,
	PFE_AI390,
	PFE_AI391,
	PFE_AI392,
	PFE_AI393,
	PFE_AI394,
	PFE_AI395,
	PFE_AI396,
	PFE_AI397,
	PFE_AI398,
	PFE_AI399,
	PFE_AI400,
	PFE_AI401,
	PFE_AI402,
	PFE_AI403,
	PFE_AI404,
	PFE_AI405,
	PFE_AI406,
	PFE_AI407,
	PFE_AI408,
	PFE_AI409,
	PFE_AI410,
	PFE_AI411,
	PFE_AI412,
	PFE_AI413,
	PFE_AI414,
	PFE_AI415,
	PFE_AI416,
	PFE_AI417,
	PFE_AI418,
	PFE_AI419,
	PFE_AI420,
	PFE_AI421,
	PFE_AI422,
	PFE_AI423,
	PFE_AI424,
	PFE_AI425,
	PFE_AI426,
	PFE_AI427,
	PFE_AI428,
	PFE_AI429,
	PFE_AI430,
	PFE_AI431,
	PFE_AI432,
	PFE_AI433,
	PFE_AI434,
	PFE_AI435,
	PFE_AI436,
	PFE_AI437,
	PFE_AI438,
	PFE_AI439,
	PFE_AI440,
	PFE_AI441,
	PFE_AI442,
	PFE_AI443,
	PFE_AI444,
	PFE_AI445,
	PFE_AI446,
	PFE_AI447,
	PFE_AI448,
	PFE_AI449,
	PFE_AI450,
	PFE_AI451,
	PFE_AI452,
	PFE_AI453,
	PFE_AI454,
	PFE_AI455,
	PFE_AI456,
	PFE_AI457,
	PFE_AI458,
	PFE_AI459,
	PFE_AI460,
	PFE_AI461,
	PFE_AI462,
	PFE_AI463,
	PFE_AI464,
	PFE_AI465,
	PFE_AI466,
	PFE_AI467,
	PFE_AI468,
	PFE_AI469,
	PFE_AI470,
	PFE_AI471,
	PFE_AI472,
	PFE_AI473,
	PFE_AI474,
	PFE_AI475,
	PFE_AI476,
	PFE_AI477,
	PFE_AI478,
	PFE_AI479,
	PFE_AI480,
	PFE_AI481,
	PFE_AI482,
	PFE_AI483,
	PFE_AI484,
	PFE_AI485,
	PFE_AI486,
	PFE_AI487,
	PFE_AI488,
	PFE_AI489,
	PFE_AI490,
	PFE_AI491,
	PFE_AI492,
	PFE_AI493,
	PFE_AI494,
	PFE_AI495,
	PFE_AI496,
	PFE_AI497,
	PFE_AI498,
	PFE_AI499,
	PFE_AI500,
	PFE_MAX,           ///< End of enum, must be last.
};
DECLARE_POSTFIX_INCREMENT(PerformanceElement)

/** Type used to hold a performance timing measurement */
typedef uint64 TimingMeasurement;

/**
 * RAII class for measuring simple elements of performance.
 * Construct an object with the appropriate element parameter when processing begins,
 * time is automatically taken when the object goes out of scope again.
 *
 * Call Paused at the start of a frame if the processing of this element is paused.
 */
class PerformanceMeasurer {
	PerformanceElement elem;
	TimingMeasurement start_time;
public:
	PerformanceMeasurer(PerformanceElement elem);
	~PerformanceMeasurer();
	void SetExpectedRate(double rate);
	static void SetInactive(PerformanceElement elem);
	static void Paused(PerformanceElement elem);
};

/**
 * RAII class for measuring multi-step elements of performance.
 * At the beginning of a frame, call Reset on the element, then construct an object in the scope where
 * each processing cycle happens. The measurements are summed between resets.
 *
 * Usually StateGameLoop is an appropriate function to place Reset calls in, but for elements with
 * more isolated scopes it can also be appropriate to Reset somewhere else.
 * An example is the CallVehicleTicks function where all the vehicle type elements are reset.
 *
 * The PerformanceMeasurer::Paused function can also be used with elements otherwise measured with this class.
 */
class PerformanceAccumulator {
	PerformanceElement elem;
	TimingMeasurement start_time;
public:
	PerformanceAccumulator(PerformanceElement elem);
	~PerformanceAccumulator();
	static void Reset(PerformanceElement elem);
};

void ShowFramerateWindow();

#endif /* FRAMERATE_TYPE_H */

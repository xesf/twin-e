/** @file main.h
	@brief
	This file contains the main engine functions. It also contains
	configurations file definitions.

	Prequengine: a Little Big Adventure engine

	Copyright (C) 2008 Prequengine team \n
	Copyright (C) 2002-2007 The TwinEngine team \n

	This program is free software; you can redistribute it and/or \n
	modify it under the terms of the GNU General Public License \n
	as published by the Free Software Foundation; either version 2 \n
	of the License, or (at your option) any later version. \n

	This program is distributed in the hope that it will be useful, \n
	but WITHOUT ANY WARRANTY; without even the implied warranty of \n
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the \n
	GNU General Public License for more details. \n

	You should have received a copy of the GNU General Public License \n
	along with this program; if not, write to the Free Software \n
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

	$URL$
	$Id$
*/

#ifndef MAIN_H
#define MAIN_H

#include "sys.h"

/** Original screen width */
#define DEFAULT_SCREEN_WIDTH			640
/** Original screen height */
#define DEFAULT_SCREEN_HEIGHT			480
/** Scale screen to double size */
#define SCALE					1
/** Original screen width */
#define SCREEN_WIDTH			DEFAULT_SCREEN_WIDTH * SCALE
/** Original screen height */
#define SCREEN_HEIGHT			DEFAULT_SCREEN_HEIGHT * SCALE
/** Original FLA screen width */
#define FLASCREEN_WIDTH			320
/** Original FLA screen height */
#define FLASCREEN_HEIGHT		200
/** Default frames per second */
#define DEFAULT_FRAMES_PER_SECOND	20

/** Number of colors used in the game */
#define NUMOFCOLORS		256


/** Configuration file structure

	Used in the engine to load/use certain parts of code according with
	this settings. Check \a lba.cfg file for valid values for each settings.\n
	All the settings with (*) means they are new and only exist in this engine. */
typedef struct ConfigFile {
	/** Language name */
	int8 Language[10];
	/** Language CD name */
	int8 LanguageCD[10];
	/** Enable/Disable game dialogues */
	int8 FlagDisplayText[3];
	/** Save voice files on hard disk */
	int8 FlagKeepVoice[3];
	/** Wave volume */
	int32 WaveVolume;
	/** Chacters voices volume */
	int32 VoiceVolume;
	/** Music volume */
	int32 MusicVolume;
	/** CD volume */
	int32 CDVolume;
	/** Line-In volume */
	int32 LineVolume;
	/** Main volume controller */
	int32 MasterVolume;
	/** *Game version */
	int32 Version;
	/** Language Identification according with Language setting. */
	int32 LanguageId;
	/** Language Identification according with Language setting. */
	int32 LanguageCDId;
	/** To allow fullscreen or window mode. */
	int32 FullScreen;
	/** To run the game without sound */
	int32 NoSound;
	/** If you want to use the LBA CD or not */
	int32 UseCD;
	/** If you want to use high quality sounds */
	int32 UseHQSnd;
	/** If you want to use MP3 or OGG files. UseHQSnd must be set */
	int32 UseMP3;
	/** To keep FLA movies aspect ratio or to strech it to
		fullscreen like in the original game */
	int32 FLAwide;
	/** To use FLA PCX images instead of FLA movies */
	int32 UseFLAPCX;
	/** Use command format movies like AVIs instead of FLA movies */
	int32 UseAVI;
	/** Use cross fade effect while changing images, or be as the original */
	int32 CrossFade;
	/** Flag to quit the game */
	int32 Quit;
	/** Flag used to keep the game frames per second */
	int32 Fps;
	/** Flag to display game debug */
	int32 Debug;
	/** Use original autosaving system or save when you want */
	int32 UseAutoSaving;
	/** Shadow mode type */
	int32 ShadowMode;
} ConfigFile;

/** Configuration file structure

	Contains all the data used in the engine to configurated the game in particulary ways.\n
	A detailled information of all types are in \a main.h header file. */
ConfigFile cfgfile;

/** CD Game directory */
int8 * cdDir;

/** Initialize main engine variables */
void init_vars();

#endif

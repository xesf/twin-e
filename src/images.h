/** @file images.h
	@brief
	This file contains image processing.

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

#ifndef IMAGES_H
#define IMAGES_H

#include "sys.h"
#include "main.h"

/** In-game palette (should not be used, except in special case. otherwise use other images functions instead) */
uint8 palette[NUMOFCOLORS * 3];

/** SDL converted in-game palette */
uint8 paletteRGBA[NUMOFCOLORS * 4];

/** SDL converted custom palette */
uint8 paletteRGBACustom[NUMOFCOLORS * 4];

/** flag to check if a custom palette is in use */
int16 palCustom;

/** flag to check in the game palette was changed */
int16 palReseted;

/** Load and display Adeline Logo */
void adeline_logo();

/** Load a custom palette
	@param index \a RESS.HQR entry index (starting from 0) */
void load_custom_palette(int32 index);

/** Load and display Main Menu image */
void load_menu_image(int16 fade_in);

/** Load and display a particulary image on \a RESS.HQR file with cross fade effect
	@param index \a RESS.HQR entry index (starting from 0)
	@param fade_in if we fade in before using the palette */
void load_image(int32 index, int16 fade_in);

/** Load and display a particulary image on \a RESS.HQR file with cross fade effect and delay
	@param index \a RESS.HQR entry index (starting from 0)
	@param time number of seconds to delay */
void load_image_delay(int32 index, int32 time);

/** Converts in-game palette to SDL palette
	@param palSource palette source with RGB
	@param palDest palette destination with RGBA */
void convert_pal_2_RGBA(uint8 * palSource, uint8 * palDest);

/** Fade image in
	@param palette current palette to fade in */
void fade_in(uint8 * palette);

/** Fade image out
	@param palette current palette to fade out */
void fade_out(uint8 * palette);

/** Calculate a new color component according with an intensity
	@param modifier color compenent
	@param color color value
	@param param unknown
	@param intensity intensity value to adjust
	@return new color component*/
int32 cross_dot(int32 modifier, int32 color, int32 param, int32 intensity);

/** Adjust palette intensity
	@param R red component of color
	@param G green component of color
	@param B blue component of color
	@param palette palette to adjust
	@param intensity intensity value to adjust */
void adjust_palette(uint8 R, uint8 G, uint8 B, uint8 * palette, int32 intensity);

/** Fade image to black
	@param palette current palette to fade */
void fade_2_black(uint8 *palette);

/** Fade image with another palette source
	@param palette current palette to fade */
void fade_2_pal(uint8 *palette);

/** Fade black palette to white palette */
void black_2_white();

/** Resets both in-game and sdl palettes */
void set_back_pal();

/** Copy a determinate screen buffer to another
	@param source screen buffer
	@param destination screen buffer */
void copy_screen(uint8 * source, uint8 * destination);

/** Clear front buffer screen */
void clear_screen();

/** Init palettes */
void init_palettes();

#endif
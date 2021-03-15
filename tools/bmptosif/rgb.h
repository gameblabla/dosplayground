/* rgb.h, Macros and constants for colour conversions for the x68Launcher.
 Copyright (C) 2020  John Snowdon
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#define r_mask555 ((1 << 5) - 1) << 10
#define g_mask555 ((1 << 5) - 1) << 5
#define b_mask555 ((1 << 5) - 1) << 0

#define r_mask565 ((1 << 5) - 1) << 11
#define g_mask565 ((1 << 6) - 1) << 5
#define b_mask565 ((1 << 5) - 1) << 0

// Macro taken from https://sourceforge.net/projects/x68000-doom/
// Merge 3x 8bit values to 15bit + intensity in GRB format 
// Keep only the 5 msb of each value 
#define rgb888_2grb(r, g, b, i) ( ((b&0xF8)>>2) | ((g&0xF8)<<8) | ((r&0xF8)<<3) | i )

// Merge 3x 8bit values to 15bit + intensity in RGB format
// Keep only the 5 msb of each value
#define rgb888_2rgb(r, g, b, i) ( ((r&0xF8)<<8) | ((g&0xF8)<<3) | ((b&0xF8)>>2) | i )

// Merge 1x 16bit value (in 565 format) and intensity to a 15bit + intensity in GRBI format
//#define rgb565_2grb(rgb, i) (rgb888_2grb((rgb & r_mask565), (rgb & g_mask565), (rgb & b_mask565), i))

// Merge 1x 16bit value (in 565 format) and intensity to a 15bit + intensity in RGBI format
//#define rgb565_2rgb(rgb, i) (rgb888_2rgb((rgb & r_mask565), (rgb & g_mask565), (rgb & b_mask565), i))

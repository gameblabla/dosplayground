/* input.h, User input routines for the pc98Launcher.
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

#define KB_DELAY					100		// 60ms delay before reading keyboard state

// Input codes as returned to main()
#define input_none				0x0000
#define input_select				0x1C0D // Enter
#define input_cancel				0x011B // Escape
#define input_switch				0x0F09 // Tab
#define input_up					0x48E0
#define input_down				0x50E0
#define input_left				0x4BE0
#define input_right				0x4DE0
#define input_scroll_up			0x49E0 // Page up
#define input_scroll_down		0x51E0 // Page down
#define input_quit				0x1051 // Q == Quit
#define input_quit_lower			0x1071 // Q == Quit
#define input_filter				0x2146 // F == Filter
#define input_filter_lower		0x2166 // F == Filter
#define input_help				0x2348 // H == Help
#define input_help_lower			0x2369 // H == Help

// Function prototypes
int	input_get();
int input_test();
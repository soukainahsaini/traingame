//
//  Copyright (C) 2009  Nick Gasson
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef INC_GAME_SCREENS_HPP
#define INC_GAME_SCREENS_HPP

#include "IScreen.hpp"
#include "IMap.hpp"
#include "IWindow.hpp"

// Create the various screens
// These may be called multiple times
IScreenPtr make_editor_screen(IMapPtr a_map);
IScreenPtr make_game_screen(IMapPtr a_map);
IScreenPtr makeUIDemo();
IScreenPtr makeLTreeDemo();

// Access to the window the game is running in
IWindowPtr get_game_window();

// Add editor GUI controls
void add_editorGUI();

#endif

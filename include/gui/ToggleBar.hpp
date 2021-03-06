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

#ifndef INC_TOGGLEBAR_HPP
#define INC_TOGGLEBAR_HPP

#include "Platform.hpp"
#include "gui/ContainerWidget.hpp"

namespace gui {

   // Like a toolbar but one item is always shown selected
   class ToggleBar : public ContainerWidget {
   public:
      ToggleBar(const AttributeSet& attrs);

      void render(RenderContext& rc) const;
      bool handle_click(int x, int y);
   private:
      void child_added(Widget* w);

      int nextX;
      int button_width, button_height;
   };
   
}

#endif

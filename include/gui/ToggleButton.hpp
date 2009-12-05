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

#ifndef INC_TOGGLE_BUTTON_HPP
#define INC_TOGGLE_BUTTON_HPP

#include "Platform.hpp"
#include "ITexture.hpp"
#include "gui/Widget.hpp"

namespace gui {

   // A special sort of button that only appears in toggle bars
   class ToggleButton : public Widget {
   public:
      ToggleButton(const AttributeSet& attrs);

      void render(RenderContext& rc) const;
      bool handleClick(int x, int y);

      void on() { enabled = true; }
      void off() { enabled = false; }
   private:
      ITexturePtr texture;
      bool enabled;
   };
}

#endif

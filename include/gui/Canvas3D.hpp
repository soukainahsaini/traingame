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

#ifndef INC_CANVAS3D_HPP
#define INC_CANVAS3D_HPP

#include "Platform.hpp"
#include "gui/Widget.hpp"

namespace gui {

   // A canvas which arbitrary 3D scenes can be rendered on
   class Canvas3D : public Widget {
   public:
      Canvas3D(const AttributeSet& attrs);

      void render(RenderContext& rc) const;
   private:
      bool clear;
   };
}

#endif

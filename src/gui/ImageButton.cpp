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

#include "gui/ImageButton.hpp"

using namespace gui;

ImageButton::ImageButton(const AttributeSet& attrs)
   : Widget(attrs)
{
   texture = load_texture(attrs.get<string>("image"));
}

void ImageButton::render(RenderContext& rc) const
{
   rc.image(x(), y(), width(), height(), texture);
}

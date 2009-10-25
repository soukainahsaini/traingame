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

#include "gui2/Label.hpp"
#include "ILogger.hpp"

#include <cstdarg>
#include <cstdio>

using namespace gui;

Label::Label(const AttributeSet& attrs)
   : Widget(attrs),
     text_(attrs.get<string>("text"))
{
   
}

void Label::render(RenderContext& rc) const
{
   rc.print(rc.theme().normal_font(), x(), y(), text_);
}

void Label::adjust_for_theme(Theme& theme)
{
   width(theme.normal_font()->text_width(text_));
   height(theme.normal_font()->height());
}

void Label::format(const char* fmt, ...)
{
   va_list ap;
   va_start(ap, fmt);

   char* buf;
   vasprintf(&buf, fmt, ap);
   text(buf);
   
   free(buf);
   va_end(ap);
}


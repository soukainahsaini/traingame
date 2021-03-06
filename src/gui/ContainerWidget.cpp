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

#include "gui/ContainerWidget.hpp"
#include "ILogger.hpp"

using namespace gui;

ContainerWidget::ContainerWidget(const AttributeSet& attrs)
   : Widget(attrs)
{

}

void ContainerWidget::render(RenderContext& rc) const
{
   for (ChildList::const_iterator it = const_begin();
        it != const_end(); ++it) {
      if ((*it)->visible()) {
         rc.scissor(*it);
         (*it)->render(rc);
      }
   }
}

void ContainerWidget::add_child(Widget* w)
{
   children.push_back(w);
   child_added(w);
}

void ContainerWidget::adjust_for_theme(const Theme& theme)
{
   for (ChildList::const_iterator it = const_begin();
        it != const_end(); ++it)
      (*it)->adjust_for_theme(theme);
}

bool ContainerWidget::handle_click(int x, int y)
{
   if (!visible())
      return false;
 
   bool accepted = false;
  
   for (ChildList::const_iterator it = const_begin();
        it != const_end(); ++it) {
      Widget& w = **it;

      if (w.x() <= x && x < w.x() + w.width()
         && w.y() <= y && y < w.y() + w.height())
         accepted |= w.handle_click(x - w.x(), y - w.y());
   }

   bool in_container = this->x() <= x && x < this->x() + this->width()
      && this->y() <= y && y < this->y() + this->height();
   accepted |= in_container;

   return accepted;
}

int ContainerWidget::count_children()
{
   return children.size();
}


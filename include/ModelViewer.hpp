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

#ifndef INC_MODELVIEWER_HPP
#define INC_MODELVIEWER_HPP

#include "Platform.hpp"
#include "IModel.hpp"

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>

// FLTK widget for displaying models
class ModelViewer : public Fl_Gl_Window {
public:
   ModelViewer(int x, int y, int w, int h);
   ~ModelViewer();

   void setModel(IModelPtr aModel);
   void rotate(float anAngle);
   float angle() const { return myRotation; }

   // Fl_Gl_Window interface
   void draw();
   int handle(int anEvent);
private:
   IModelPtr myModel;
   float myRotation;
};

#endif

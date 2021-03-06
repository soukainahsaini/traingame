//
//  Copyright (C) 2010  Nick Gasson
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

#include "ClipVolume.hpp"
#include "OpenGLHelper.hpp"

ClipVolume::ClipVolume(float x, float w, float z, float d)
{
   glPushMatrix();
   {
      const float overlap = 0.25f;
      
      glTranslatef(x - overlap, 0.0f, 0.0f);
      
      GLdouble eqn0[4] = { 1.0, 0.0, 0.0, 0.0 };
      glClipPlane(GL_CLIP_PLANE0, eqn0);
      glEnable(GL_CLIP_PLANE0);
      
      glTranslatef(w + overlap * 2.0f, 0.0f, 0.0f);

      GLdouble eqn1[4] = { -1.0, 0.0, 0.0, 0.0 };
      glClipPlane(GL_CLIP_PLANE1, eqn1);
      glEnable(GL_CLIP_PLANE1);
      
      glTranslatef(0.0f, 0.0f, z - overlap);
   
      GLdouble eqn2[4] = { 0.0, 0.0, 1.0, 0.0 };
      glClipPlane(GL_CLIP_PLANE2, eqn2);
      glEnable(GL_CLIP_PLANE2);
      
      glTranslatef(0.0f, 0.0f, d + overlap * 2.0f);

      GLdouble eqn3[4] = { 0.0, 0.0, -1.0, 0.0 };
      glClipPlane(GL_CLIP_PLANE3, eqn3);
      glEnable(GL_CLIP_PLANE3);
   }
   glPopMatrix();
}

ClipVolume::~ClipVolume()
{
   glDisable(GL_CLIP_PLANE0);
   glDisable(GL_CLIP_PLANE1);
   glDisable(GL_CLIP_PLANE2);
   glDisable(GL_CLIP_PLANE3);
}

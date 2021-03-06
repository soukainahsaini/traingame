//
//  Copyright (C) 2009-2010  Nick Gasson
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

#ifndef INC_IMODEL_HPP
#define INC_IMODEL_HPP

#include "Platform.hpp"
#include "Maths.hpp"
#include "IResource.hpp"
#include "IMesh.hpp"

#include <string>

struct IModel {
   virtual ~IModel() {}
   
   virtual void render() const = 0;
   virtual void cache() = 0;
   virtual void merge(IMeshBufferPtr buf,
      Vector<float> off, float y_angle=0.0f) const = 0;
   virtual Vector<float> dimensions() const = 0;
};

typedef shared_ptr<IModel> IModelPtr;

// Load a model from a WaveFront .obj file
IModelPtr load_model(IResourcePtr a_res,
                     const string& a_file_name,
                     float a_scale = 1.0f,
                     Vector<float> shift = make_vector(0.0f, 0.0f, 0.0f));

#endif

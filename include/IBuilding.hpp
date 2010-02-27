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

#ifndef INC_IBUILDING_HPP
#define INC_IBUILDING_HPP

#include "Platform.hpp"
#include "IModel.hpp"
#include "IXMLSerialisable.hpp"

#include <string>

// Interface to buildings and other bits of scenery
struct IBuilding : IXMLSerialisable {
   virtual ~IBuilding() {}

   virtual const string& name() const = 0;
   virtual void render() const = 0;
   virtual void setAngle(float a) = 0;
   virtual void setPosition(float x, float y, float z) = 0;
};

typedef shared_ptr<IBuilding> IBuildingPtr;

class AttributeSet;

IBuildingPtr loadBuilding(const string& aResId, float angle);
IBuildingPtr loadBuilding(const AttributeSet& attrs);

#endif

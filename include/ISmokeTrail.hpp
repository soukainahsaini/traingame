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

#ifndef INC_ISMOKE_TRAIL_HPP
#define INC_ISMOKE_TRAIL_HPP

#include "Platform.hpp"

// Smoke and steam effects
struct ISmokeTrail {
   virtual ~ISmokeTrail() {}

   // Move and generate new particles
   virtual void update(int a_delta) = 0;
   
   // Draw all the particles
   virtual void render() const = 0;

   // Change the position where new particles are generated
   virtual void set_position(float x, float y, float z) = 0;

   // Change the initial velocity of new particles
   virtual void set_velocity(float x, float y, float z) = 0;
   
   // Change the rate at which particles are created
   // Delay is in milliseconds
   virtual void set_delay(int a_delay) = 0;
};

typedef shared_ptr<ISmokeTrail> ISmokeTrailPtr;

ISmokeTrailPtr make_smoke_trail();

#endif

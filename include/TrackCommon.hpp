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

#ifndef INC_TRACK_COMMON_HPP
#define INC_TRACK_COMMON_HPP

#include "ITrackSegment.hpp"
#include "IMesh.hpp"
#include "BezierCurve.hpp"

class StraightTrackHelper {
public:
   void mergeStraightRail(IMeshBufferPtr buf,
      Vector<float> off, float yAngle) const;
   
private:
   void mergeOneRail(IMeshBufferPtr buf,
      Vector<float> off, float yAngle) const;
      
   static IMeshBufferPtr railBuf;
};

class SleeperHelper {
public:
   void mergeSleeper(IMeshBufferPtr buf,
      Vector<float> off, float yAngle) const;
   
private:
   static IMeshBufferPtr sleeperBuf;
};

class BezierHelper {
public:
   IMeshBufferPtr makeBezierRailMesh(const BezierCurve<float>& func) const;
};

// Common track rendering functions
void renderSleeper();
void renderStraightRail();

void renderCurvedTrack(int baseRadius, track::Angle startAngle,
                       track::Angle endAngle);
void renderRailMesh(IMeshPtr aMesh);
void transformToOrigin(int baseRadius, track::Angle startAngle);
IMeshPtr makeBezierRailMesh(const BezierCurve<float>& aFunc);


// Track constants
namespace track {
   const float RAIL_HEIGHT = 0.1f;
}

#endif

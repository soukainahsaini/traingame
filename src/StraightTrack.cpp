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

#include "ITrackSegment.hpp"
#include "TrackCommon.hpp"
#include "ILogger.hpp"
#include "XMLBuilder.hpp"

#include <cassert>
#include <stdexcept>

#include <GL/gl.h>
#include <GL/glu.h>
#include <boost/lexical_cast.hpp>

using namespace placeholders;
using namespace boost;
using namespace track;

// Concrete implementation of straight-line pieces of track
class StraightTrack : public ITrackSegment,
                      public enable_shared_from_this<StraightTrack> {
public:
   StraightTrack(const Direction& aDirection);
   ~StraightTrack();
   
   void render() const;
   
   void setOrigin(int x, int y) { origin.x = x; origin.y = y; }
   double segmentLength(const track::TravelToken& aToken) const { return 1.0; }

   Vector<double> offsetForDelta(double aDelta) const;
   Connection nextPosition(const track::TravelToken& aDirection) const;
   bool isValidDirection(const Direction& aDirection) const;
   void getEndpoints(list<Point<int> >& aList) const;
   
   ITrackSegmentPtr mergeExit(const Point<int>& aPoint,
      const track::Direction& aDirection);
   track::TravelToken getTravelToken(track::Position aPosition,
      track::Direction aDirection) const;

   bool hasMultipleStates() const { return false; }
   void nextState() {}
   void prevState() {}
   void setStateRenderHint() {}

   // IXMLSerialisable interface
   xml::element toXml() const;
   
private:
   void transform(const track::TravelToken& aToken, double aDelta) const;
   void ensureValidDirection(const track::Direction& aDirection) const;
   
   Point<int> origin;  // Absolute position
   Direction direction;
};

StraightTrack::StraightTrack(const Direction& aDirection)
   : direction(aDirection)
{
   
}

StraightTrack::~StraightTrack()
{
   
}

track::TravelToken
StraightTrack::getTravelToken(track::Position aPosition,
   track::Direction aDirection) const
{
   ensureValidDirection(aDirection);

   track::TravelToken tok = {
      aDirection,
      aPosition,
      bind(&StraightTrack::transform, this, _1, _2),
      1
   };
   return tok;
}

void StraightTrack::transform(const track::TravelToken& aToken,
   double aDelta) const
{
   assert(aDelta < 1.0);

   if (aToken.direction == -direction)
      aDelta = 1.0 - aDelta;

   const double xTrans = direction == axis::X ? aDelta : 0;
   const double yTrans = direction == axis::Y ? aDelta : 0;

   glTranslated(static_cast<double>(origin.x) + xTrans,
      0.0,
      static_cast<double>(origin.y) + yTrans);

   if (direction == axis::Y)
      glRotated(-90.0, 0.0, 1.0, 0.0);

   glTranslated(-0.5, 0.0, 0.0);
   
   if (aToken.direction == -direction)
      glRotated(-180.0, 0.0, 1.0, 0.0);
}

ITrackSegmentPtr StraightTrack::mergeExit(const Point<int>& aPoint,
   const track::Direction& aDirection)
{
   debug() << "mergeExit aPoint=" << aPoint
           << " aDirection=" << aDirection
           << " me=" << origin;

   // See if this is already a valid exit
   if (isValidDirection(aDirection) && aPoint == origin)
      return shared_from_this();

   // See if we can make this a crossover track
   if (direction != aDirection && aPoint == origin)
      return makeCrossoverTrack();

   // See if we can make some points
   if (isValidDirection(aDirection)) {
      // X-aligned points
      if (aPoint == origin + makePoint(-2, 1))
         return makePoints(-axis::X, true);
      else if (aPoint == origin + makePoint(-2, -1))
         return makePoints(-axis::X, false);
      else if (aPoint == origin + makePoint(2, 1))
         return makePoints(axis::X, false);
      else if (aPoint == origin + makePoint(2, -1))
         return makePoints(axis::X, true);

      // Y-aligned points
      if (aPoint == origin + makePoint(1, -2))
         return makePoints(-axis::Y, false);
      else if (aPoint == origin + makePoint(-1, -2))
         return makePoints(-axis::Y, true);
      else if (aPoint == origin + makePoint(1, 2))
         return makePoints(axis::Y, true);
      else if (aPoint == origin + makePoint(-1, 2))
         return makePoints(axis::Y, false);
   }
   
   // Not possible to merge
   return ITrackSegmentPtr();
}

bool StraightTrack::isValidDirection(const Direction& aDirection) const
{
   if (direction == axis::X)
      return aDirection == axis::X || -aDirection == axis::X;
   else
      return aDirection == axis::Y || -aDirection == axis::Y;
}

void StraightTrack::getEndpoints(list<Point<int> >& aList) const
{
   aList.push_back(origin);
}

void StraightTrack::ensureValidDirection(const Direction& aDirection) const
{
   if (!isValidDirection(aDirection))
      throw runtime_error
         ("Invalid direction on straight track: "
            + lexical_cast<string>(aDirection)
            + " (should be parallel to "
            + lexical_cast<string>(direction) + ")");
}

Connection StraightTrack::nextPosition(const track::TravelToken& aToken) const
{
   ensureValidDirection(aToken.direction);

   if (aToken.direction == axis::X)
      return make_pair(makePoint(origin.x + 1, origin.y), axis::X);
   else if (aToken.direction == -axis::X)
      return make_pair(makePoint(origin.x - 1, origin.y), -axis::X);
   else if (aToken.direction == axis::Y)
      return make_pair(makePoint(origin.x, origin.y + 1), axis::Y);
   else if (aToken.direction == -axis::Y)
      return make_pair(makePoint(origin.x, origin.y - 1), -axis::Y);
   else
      assert(false);
}

void StraightTrack::render() const
{
   glPushMatrix();

   if (direction == axis::X)
      glRotated(90.0, 0.0, 1.0, 0.0);
   
   renderStraightRail();
   
   // Draw the sleepers
   glRotated(90.0, 0.0, 1.0, 0.0);
   glTranslated(-0.4, 0.0, 0.0);

   for (int i = 0; i < 4; i++) {
      renderSleeper();
      glTranslated(0.25, 0.0, 0.0);
   }
   
   glPopMatrix();
}

xml::element StraightTrack::toXml() const
{
   return xml::element("straightTrack")
      .addAttribute("align", direction == axis::X ? "x" : "y");
}

ITrackSegmentPtr makeStraightTrack(const Direction& aDirection)
{
   Direction realDir(aDirection);
   
   // Direction must either be along axis::X or axis::Y but we
   // allow the opositite direction here too
   if (realDir == -axis::X || realDir == -axis::Y)
      realDir = -realDir;

   if (realDir != axis::X && realDir != axis::Y)
      throw runtime_error("Illegal straight track direction: "
         + lexical_cast<string>(aDirection));
   
   return ITrackSegmentPtr(new StraightTrack(realDir));
}

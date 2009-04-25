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

#include <cmath>
#include <cassert>
#include <stdexcept>

#include <GL/gl.h>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace std::tr1;
using namespace std::tr1::placeholders;
using namespace Track;
using namespace boost;

// Concrete implementation of curved pieces of track
class CurvedTrack : public ITrackSegment {
public:
   CurvedTrack(Track::Angle aStartAngle, Track::Angle aFinishAngle,
               int aRadius);
   ~CurvedTrack();

   void render() const;

   void setOrigin(int x, int y) { myX = x; myY = y; }
   double segmentLength() const;

   Connection nextPosition(const Vector<int>& aDirection) const;
   TransformFunc transformFunc(const Track::Direction& aDirection) const;
   bool isValidDirection(const Direction& aDirection) const;
   void getEndpoints(list<Point<int> >& aList) const;
private:
   void transform(const Track::Direction& aDirection, double aDelta) const;
   Vector<int> cwEntryVector() const;
   Vector<int> ccwEntryVector() const;
   void ensureValidDirection(const Direction& aDirection) const;
   
   int myX, myY, myBaseRadius;
   Track::Angle myStartAngle, myFinishAngle;
};

CurvedTrack::CurvedTrack(Track::Angle aStartAngle,
                         Track::Angle aFinishAngle,
                         int aRadius)
   : myX(0), myY(0), myBaseRadius(aRadius),
     myStartAngle(aStartAngle), myFinishAngle(aFinishAngle)
{
   
}

CurvedTrack::~CurvedTrack()
{

}

void CurvedTrack::transform(const Track::Direction& aDirection, double aDelta) const
{
   assert(aDelta < segmentLength());
   
   glTranslated(static_cast<double>(myX),
                0.0,
                static_cast<double>(myY));

   transformToOrigin(myBaseRadius, degToRad(myStartAngle));

   bool backwards = aDirection == cwEntryVector();
   
   double ratio = aDelta / segmentLength();
   if (backwards)
      ratio = 1.0 - ratio;
      
   double angle = myStartAngle + (90.0 * ratio);

   glRotated(angle, 0.0, 1.0, 0.0);
   glTranslated(0.0, 0.0, static_cast<double>(myBaseRadius - 0.5));

   if (backwards)
      glRotatef(180.0, 0, 1, 0);
}

double CurvedTrack::segmentLength() const
{
   // Assume curve is only through 90 degrees
   return M_PI * (static_cast<double>(myBaseRadius) - 0.5) / 2.0;
}

ITrackSegment::TransformFunc
CurvedTrack::transformFunc(const Track::Direction& aDirection) const
{
   ensureValidDirection(aDirection);
   
   return bind(&CurvedTrack::transform, this, aDirection, _1);
}

//
// Imagine the train is travelling in a circle like this:
//
// (0, 0)
//              180
//             [-1 0]
//        <-------------^
//        |             |
//        |             |
// [0 1]  |             | [0 -1] 90
// 270    |             |
//        |             |
//        V------------>|
//             [1 0]
//               0
//
// Above are the vectors for /counter/-clockwise movement
//

// The vector the train is moving on if it enters clockwise
Vector<int> CurvedTrack::cwEntryVector() const
{
   return makeVector<int>(-cos(degToRad(myFinishAngle)), 0,
                          sin(degToRad(myFinishAngle)));
}

// The vector the train is moving on if it enters counter-clockwise
Vector<int> CurvedTrack::ccwEntryVector() const
{
   return makeVector<int>(cos(degToRad(myStartAngle)), 0.0,
                          -sin(degToRad(myStartAngle)));
}

void CurvedTrack::ensureValidDirection(const Direction& aDirection) const
{
   if (!isValidDirection(aDirection))
      throw runtime_error
         ("Invalid direction on curved track from "
          + lexical_cast<string>(myStartAngle) + " to "
          + lexical_cast<string>(myFinishAngle) + " degrees: "
          + lexical_cast<string>(aDirection)
          + " (should be "
          + lexical_cast<string>(cwEntryVector()) + " or "
          + lexical_cast<string>(ccwEntryVector()) + ")");
}

bool CurvedTrack::isValidDirection(const Direction& aDirection) const
{
   return aDirection == cwEntryVector() || aDirection == ccwEntryVector();
}

Connection CurvedTrack::nextPosition(const Vector<int>& aDirection) const
{
   ensureValidDirection(aDirection);

   bool backwards;
   Vector<int> nextDir;
   if (aDirection == cwEntryVector()) {
      nextDir = -ccwEntryVector();
      log() << "nextDir = " << nextDir;
      backwards = true;
   }
   else if (aDirection == ccwEntryVector()) {
      nextDir = -cwEntryVector();
      backwards = false;
   }
   else
      assert(false);

   // Assuming 90 degree curves again
   const int cosEnd = static_cast<int>(cos(degToRad(myFinishAngle)));
   const int cosStart = static_cast<int>(cos(degToRad(myStartAngle)));
   const int sinEnd = static_cast<int>(sin(degToRad(myFinishAngle)));
   const int sinStart = static_cast<int>(sin(degToRad(myStartAngle)));

   int xDelta, yDelta;

   if (backwards)
      xDelta = yDelta = 0;
   else {
      xDelta = (myBaseRadius - 1) * (sinEnd - sinStart);
      yDelta = (myBaseRadius - 1) * (cosEnd - cosStart);
   }
   
   return make_pair(makePoint(myX + xDelta + nextDir.x,
                              myY + yDelta + nextDir.z),
                    nextDir);
}

void CurvedTrack::getEndpoints(list<Point<int> >& aList) const
{
   aList.push_back(makePoint(myX, myY));

   // Assuming 90 degree curves again
   const int cosEnd = static_cast<int>(cos(degToRad(myFinishAngle)));
   const int cosStart = static_cast<int>(cos(degToRad(myStartAngle)));
   const int sinEnd = static_cast<int>(sin(degToRad(myFinishAngle)));
   const int sinStart = static_cast<int>(sin(degToRad(myStartAngle)));
   
   const int xDelta = (myBaseRadius - 1) * (sinEnd - sinStart);
   const int yDelta = (myBaseRadius - 1) * (cosEnd - cosStart);
   
   aList.push_back(makePoint(myX + xDelta, myY + yDelta));
}

void CurvedTrack::render() const
{
   renderCurvedTrack(myBaseRadius,
                     (static_cast<double>(myStartAngle)*M_PI)/180.0,
                     (static_cast<double>(myFinishAngle)*M_PI)/180.0);
}

ITrackSegmentPtr makeCurvedTrack(Track::Angle aStartAngle,
                                 Track::Angle aFinishAngle, int aRadius)
{
   assert(aStartAngle < aFinishAngle);
   
   return ITrackSegmentPtr
      (new CurvedTrack(aStartAngle, aFinishAngle, aRadius));
}

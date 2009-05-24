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

#include "GameScreens.hpp"
#include "ILogger.hpp"
#include "IModel.hpp"
#include "IMap.hpp"
#include "Maths.hpp"
#include "ILight.hpp"
#include "gui/IContainer.hpp"

#include <algorithm>

#include <GL/gl.h>

using namespace std;
using namespace gui;

// Concrete editor class
class Editor : public IScreen {
public:
   Editor(IMapPtr aMap, const string& aFileName);
   ~Editor();
   
   void display(IGraphicsPtr aContext) const;
   void overlay() const;
   void update(IPickBufferPtr aPickBuffer, int aDelta);
   void onKeyDown(SDLKey aKey);
   void onKeyUp(SDLKey aKey);
   void onMouseMove(IPickBufferPtr aPickBuffer, int x, int y, int xrel, int yrel);
   void onMouseClick(IPickBufferPtr aPickBuffer, int x, int y,
                     MouseButton aButton);
   void onMouseRelease(IPickBufferPtr aPickBuffer, int x, int y,
                       MouseButton aButton);
private:
   void buildGUI();
   void drawDraggedTrack();
   bool drawTrackTile(const Point<int>& aPoint, const track::Direction& anAxis);
   void drawDraggedStraight(const track::Direction& anAxis, int aLength);
   void drawDraggedCurve(int xLength, int yLength);
   bool canConnect(const Point<int>& aFirstPoint,
                   const Point<int>& aSecondPoint) const;
   void dragBoxBounds(int& xMin, int& xMax, int &yMin, int& yMax) const;
   void raiseTerrain();
   void lowerTerrain();
   void levelTerrain();
   void deleteObjects();
   
   // Signal handlers
   void onRaiseTerrainSelect();
   void onTrackSelect();
   void onLowerTerrainSelect();
   void onDeleteSelect();
   void onLevelTerrainSelect();
   
   IMapPtr myMap;
   
   ILightPtr mySun;
   Vector<double> myPosition;

   string myFileName;
   bool amScrolling;

   // Variables for dragging track segments
   Point<int> myDragBegin, myDragEnd;
   bool amDragging;

   // Different tools the user can be using
   enum Tool {
      TRACK_TOOL, RAISE_TOOL, LOWER_TOOL, DELETE_TOOL,
      LEVEL_TOOL
   };
   Tool myTool;

   // GUI variables
   IContainerPtr myToolbar;
};

Editor::Editor(IMapPtr aMap, const string& aFileName)
   : myMap(aMap), myPosition(4.5, -17.5, -21.5),
     myFileName(aFileName), amScrolling(false),
     amDragging(false), myTool(TRACK_TOOL)
{
   mySun = makeSunLight();

   // Build the GUI
   myToolbar = makeFlowBox(FLOW_BOX_HORIZ, false);

   IButtonPtr trackButton = makeButton("data/images/track_icon.png");
   trackButton->onClick(bind(&Editor::onTrackSelect, this));
   myToolbar->addChild(trackButton);

   IButtonPtr raiseButton = makeButton("data/images/raise_icon.png");
   raiseButton->onClick(bind(&Editor::onRaiseTerrainSelect, this));
   myToolbar->addChild(raiseButton);

   IButtonPtr lowerButton = makeButton("data/images/lower_icon.png");
   lowerButton->onClick(bind(&Editor::onLowerTerrainSelect, this));
   myToolbar->addChild(lowerButton);

   IButtonPtr levelButton = makeButton("data/images/level_icon.png");
   levelButton->onClick(bind(&Editor::onLevelTerrainSelect, this));
   myToolbar->addChild(levelButton);
   
   IButtonPtr deleteButton = makeButton("data/images/delete_icon.png");
   deleteButton->onClick(bind(&Editor::onDeleteSelect, this));
   myToolbar->addChild(deleteButton);
      
   IButtonPtr startButton = makeButton("data/images/start_icon.png");
   //startButton->onClick(bind(&Editor::onLowerTerrainSelect, this));
   myToolbar->addChild(startButton);

   myMap->setGrid(true);

   log() << "Editing " << aFileName;
}

Editor::~Editor()
{
   
}

// Calculate the bounds of the drag box accounting for the different
// possible directions of dragging
void Editor::dragBoxBounds(int& xMin, int& xMax, int &yMin, int& yMax) const
{
   xMin = min(myDragBegin.x, myDragEnd.x);
   xMax = max(myDragBegin.x, myDragEnd.x);

   yMin = min(myDragBegin.y, myDragEnd.y);
   yMax = max(myDragBegin.y, myDragEnd.y); 
}
   
// Render the next frame
void Editor::display(IGraphicsPtr aContext) const
{  
   aContext->setCamera(myPosition, makeVector(45.0, 45.0, 0.0));
 
   mySun->apply();
   
   myMap->render(aContext);

   // Draw the highlight if we are dragging track
   if (amDragging) {
      int xmin, xmax, ymin, ymax;
      dragBoxBounds(xmin, xmax, ymin, ymax);
      
      for (int x = xmin; x <= xmax; x++) {
         for (int y = ymin; y <= ymax; y++)
            myMap->highlightTile(aContext, makePoint(x, y));
      }         
   }
}

// Render the overlay
void Editor::overlay() const
{
   myToolbar->render();
}

// Prepare the next frame
void Editor::update(IPickBufferPtr aPickBuffer, int aDelta)
{
   
}

// True if the `aFirstPoint' is a valid track segment and it can
// connect to `aSecondPoint'
bool Editor::canConnect(const Point<int>& aFirstPoint,
                        const Point<int>& aSecondPoint) const
{
   if (!myMap->isValidTrack(aFirstPoint))
      return false;

   ITrackSegmentPtr track = myMap->trackAt(aFirstPoint);
   
   Vector<int> dir = makeVector
      (aFirstPoint.x - aSecondPoint.x, 0,
       aFirstPoint.y - aSecondPoint.y).normalise();

   return track->isValidDirection(dir)
      || track->isValidDirection(-dir);
}

// Draw a single tile of straight track and check for collisions
// Returns `false' if track cannot be placed here
bool Editor::drawTrackTile(const Point<int>& aPoint, const track::Direction& anAxis)
{
   if (myMap->isValidTrack(aPoint)) {
      ITrackSegmentPtr merged = myMap->trackAt(aPoint)->mergeExit(aPoint, anAxis);
      if (merged) {
         myMap->setTrackAt(aPoint, merged);
         return true;
      }
      else {
         warn() << "Cannot merge track";
         return false;
      }
   }
   else {
      myMap->setTrackAt(aPoint, makeStraightTrack(anAxis));
      return true;
   }
}

// Special case where the user drags a rectangle of width 1
// This just draws straight track along the rectangle
void Editor::drawDraggedStraight(const track::Direction& anAxis, int aLength)
{
   Point<int> where = myDragBegin;

   for (int i = 0; i < aLength; i++) {
      drawTrackTile(where, anAxis);
      
      where.x += anAxis.x;
      where.y += anAxis.z;
   }
}

// Called when the user has finished dragging a rectangle for track
// Connect the beginning and end up in the simplest way possible
void Editor::drawDraggedTrack()
{
   track::Direction straight;  // Orientation for straight track section
   
   int xmin, xmax, ymin, ymax;
   dragBoxBounds(xmin, xmax, ymin, ymax);
   
   int xlen = abs(xmax - xmin) + 1;
   int ylen = abs(ymax - ymin) + 1;
   // Normalise the coordinates so the start is always the one with
   // the smallest x-coordinate
   if (myDragBegin.x > myDragEnd.x)
      swap(myDragBegin, myDragEnd);

   track::Direction startDir, endDir;
   bool startWasGuess = false;
   bool endWasGuess = false;

   // Try to work out the direction of the track start
   if (canConnect(myDragBegin.left(), myDragBegin)
       || canConnect(myDragBegin.right(), myDragBegin)) {
      startDir = Axis::X;
   }
   else if (canConnect(myDragBegin.up(), myDragBegin)
       || canConnect(myDragBegin.down(), myDragBegin)) {
      startDir = Axis::Y;
   }
   else
      startWasGuess = true;

   // Try to work out the direction of the track end
   if (canConnect(myDragEnd.left(), myDragEnd)
       || canConnect(myDragEnd.right(), myDragEnd)) {
      endDir = Axis::X;
   }
   else if (canConnect(myDragEnd.up(), myDragEnd)
       || canConnect(myDragEnd.down(), myDragEnd)) {
      endDir = Axis::Y;
   }
   else
      endWasGuess = true;

   // If we have to guess both orientations use a heuristic to decide
   // between S-bends and curves
   if (endWasGuess && startWasGuess) {
      if (min(xlen, ylen) <= 2) {
         if (xlen > ylen)
            startDir = endDir = Axis::X;
         else
            startDir = endDir = Axis::Y;
      }
      else {
         startDir = Axis::X;
         endDir = Axis::Y;
      }
   }
   // Otherwise always prefer curves to S-bends
   else if (startWasGuess)
      startDir = endDir == Axis::X ? Axis::Y : Axis::X;
   else if (endWasGuess)
      endDir = startDir == Axis::X ? Axis::Y : Axis::X;
   
   if (xlen == 1 && ylen == 1) {
      // A single tile
      drawTrackTile(myDragBegin, startDir);
   }
   else if (xlen == 1)
      drawDraggedStraight(myDragBegin.y < myDragEnd.y ? Axis::Y : -Axis::Y, ylen);
   else if (ylen == 1)
      drawDraggedStraight(Axis::X, xlen);
   else if (startDir == endDir) {
      // An S-bend (not implemented)
      warn() << "Sorry! No S-bends yet...";
   }
   else {
      // Curves at the moment cannot be ellipses so lay track down
      // until the dragged area is a square
      while (xlen != ylen) {
         if (xlen > ylen) {
            // One of the ends must lie along the x-axis since all
            // curves are through 90 degrees so extend that one
            if (startDir == Axis::X) {
               drawTrackTile(myDragBegin, Axis::X);
               myDragBegin.x++;
            }
            else {
               drawTrackTile(myDragEnd, Axis::X);
               myDragEnd.x--;
            }
            xlen--;
         }
         else {
            // Need to draw track along y-axis
            if (startDir == Axis::Y) {
               drawTrackTile(myDragBegin, Axis::Y);

               // The y-coordinate for the drag points is not guaranteed
               // to be sorted
               if (myDragBegin.y > myDragEnd.y)
                  myDragBegin.y--;
               else
                  myDragBegin.y++;
            }
            else {
               drawTrackTile(myDragEnd, Axis::Y);
                                    
               if (myDragBegin.y > myDragEnd.y)
                  myDragEnd.y++;
               else
                  myDragEnd.y--;
            }
            ylen--;
         }
      }

      float startAngle, endAngle;
      Point<int> where;

      if (startDir == Axis::X && endDir == Axis::Y) {
         if (myDragBegin.y < myDragEnd.y) {
            startAngle = 90;
            endAngle = 180;
            where = myDragEnd;
         }
         else {
            startAngle = 0;
            endAngle = 90;
            where = myDragBegin;
         }
      }
      else {
         if (myDragBegin.y < myDragEnd.y) {
            startAngle = 270;
            endAngle = 360;
            where = myDragBegin;
         }
         else {
            startAngle = 180;
            endAngle = 270;
            where = myDragEnd;
         }
      }

      ITrackSegmentPtr track = makeCurvedTrack(startAngle, endAngle, xlen);
      track->setOrigin(where.x, where.y);

      list<Point<int> > exits;
      track->getEndpoints(exits);

      bool ok = true;
      for (list<Point<int> >::iterator it = exits.begin();
           it != exits.end(); ++it) {
         if (myMap->isValidTrack(*it)) {
            warn() << "Cannot place curve here";
            ok = false;
            break;
         }
      }

      if (ok)
         myMap->setTrackAt(where, track);
   }
}

// Level off the terrain the user has dragged
void Editor::levelTerrain()
{
   myMap->levelArea(myDragBegin, myDragEnd);
}

// Raise the terrain the user has dragged
void Editor::raiseTerrain()
{
   myMap->raiseArea(myDragBegin, myDragEnd);
}

// Lower the terrain the user has dragged
void Editor::lowerTerrain()
{
   myMap->lowerArea(myDragBegin, myDragEnd);
}

// Delete all objects in the area selected by the user
void Editor::deleteObjects()
{
   int xmin, xmax, ymin, ymax;
   dragBoxBounds(xmin, xmax, ymin, ymax);

   for (int x = xmin; x <= xmax; x++) {
      for (int y = ymin; y <= ymax; y++)
         myMap->eraseTile(x, y);
   }
}

void Editor::onMouseMove(IPickBufferPtr aPickBuffer, int x, int y,
                         int xrel, int yrel)
{   
   if (amDragging) {
      // Extend the selection rectangle
      myMap->setPickMode(true);
      IGraphicsPtr pickContext = aPickBuffer->beginPick(x, y);
      display(pickContext);
      int id = aPickBuffer->endPick();
      myMap->setPickMode(false);

      if (id > 0)
         myDragEnd = myMap->pickPosition(id);
   }
   else if (amScrolling) {
      const double speed = 0.05;
      
      myPosition.x -= xrel * speed;
      myPosition.z -= xrel * speed;
      
      myPosition.x += yrel * speed;
      myPosition.z -= yrel * speed;      
   }
}

// Change to the terrain raising mode
void Editor::onRaiseTerrainSelect()
{
   log() << "Raise terrain mode";
   myTool = RAISE_TOOL;
}

// Change to the terrain lowering mode
void Editor::onLowerTerrainSelect()
{
   log() << "Lower terrain mode";
   myTool = LOWER_TOOL;
}

// Change to the track placing mode
void Editor::onTrackSelect()
{
   log() << "Track placing mode";
   myTool = TRACK_TOOL;
}

void Editor::onLevelTerrainSelect()
{
   log() << "Level terrain mode";
   myTool = LEVEL_TOOL;
}

void Editor::onDeleteSelect()
{
   log() << "Delete mode";
   myTool = DELETE_TOOL;
}

void Editor::onMouseClick(IPickBufferPtr aPickBuffer, int x, int y,
                          MouseButton aButton)
{
   // See if the GUI can handle it
   if (myToolbar->handleClick(x, y))
      return;
   else if (aButton == MOUSE_RIGHT) {
      // Start scrolling
      amScrolling = true;
   }
   else if (aButton == MOUSE_LEFT) {
      // See if the user clicked on something in the map
      myMap->setPickMode(true);
      IGraphicsPtr pickContext = aPickBuffer->beginPick(x, y);
      display(pickContext);
      int id = aPickBuffer->endPick();
      myMap->setPickMode(false);

      if (id > 0) {
         // Begin dragging a selection rectangle
         Point<int> where = myMap->pickPosition(id);
         
         myDragBegin = myDragEnd = where;
         amDragging = true;
      }
   }
   else if (aButton == MOUSE_WHEEL_UP) {
      myPosition.y -= 0.5;
   }
   else if (aButton == MOUSE_WHEEL_DOWN) {
      myPosition.y += 0.5;
   }
      
}

void Editor::onMouseRelease(IPickBufferPtr aPickBuffer, int x, int y,
                            MouseButton aButton)
{
   // See if the GUI can handle it
   if (myToolbar->handleMouseRelease(x, y))
      return;
   else if (amDragging) {
      // Stop dragging and perform the action
      switch (myTool) {
      case TRACK_TOOL:
         drawDraggedTrack();
         break;
      case RAISE_TOOL:
         raiseTerrain();
         break;
      case LOWER_TOOL:
         lowerTerrain();
         break;
      case LEVEL_TOOL:
         levelTerrain();
         break;
      case DELETE_TOOL:
         deleteObjects();
         break;
      }
         
      amDragging = false;
   }
   else if (amScrolling) {
      amScrolling = false;
   }
}

void Editor::onKeyUp(SDLKey aKey)
{
   
}

void Editor::onKeyDown(SDLKey aKey)
{   
   switch (aKey) {
   case SDLK_x:
      // Write out to disk
      myMap->save(myFileName);
      break;
   case SDLK_g:
      // Toggle grid
      myMap->setGrid(true);
      break;
   case SDLK_p:
      // Switch to play mode
      {
         IScreenPtr game = makeGameScreen(myMap);
         getGameWindow()->switchScreen(game);
      }
      break;
   default:
      break;
   }
}

// Create an instance of the editor screen
IScreenPtr makeEditorScreen(IMapPtr aMap, const string& aFileName)
{
   return IScreenPtr(new Editor(aMap, aFileName));
}

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

#include "IScenery.hpp"
#include "IModel.hpp"
#include "OpenGLHelper.hpp"
#include "IResource.hpp"
#include "IXMLParser.hpp"
#include "ResourceCache.hpp"
#include "ILogger.hpp"
#include "XMLBuilder.hpp"
#include "Random.hpp"

#include <stdexcept>

#include <boost/cast.hpp>

// A tree which is just a 3D model
class Tree : public IScenery, public IXMLCallback {
public:
   Tree(IResourcePtr res);

   // IScenery interface
   void render() const;
   void setPosition(float x, float y, float z);
   void setAngle(float a) { angle = a; }
   const string& name() const { return name_; }

   // IXMLCallback interface
   void text(const string& localName, const string& content);

   // IXMLSerialisable interface
   xml::element toXml() const;
   
private:
   Vector<float> position;
   IModelPtr model;
   float angle;
   string name_;

   struct ParserState {
      string modelFile;
      float scale;
      IResourcePtr res;
   } *parserState;
};

Tree::Tree(IResourcePtr res)
{
   static IXMLParserPtr parser = makeXMLParser("schemas/tree.xsd");

   parserState = new ParserState;
   parserState->res = res;
   
   parser->parse(res->xmlFileName(), *this);

   model = loadModel(res, parserState->modelFile, parserState->scale);
   
   delete parserState;
}

void Tree::text(const string& localName, const string& content)
{
   if (localName == "model")
      parserState->modelFile = content;
   else if (localName == "scale")
      parserState->scale = boost::lexical_cast<float>(content);
   else if (localName == "name") {
      const string& expectedName = parserState->res->name();
      if (content != expectedName)
         throw runtime_error(
            "Expected tree name to be '" + expectedName
            + "' but found'" + content + "' in XML");
      else
         name_ = content;
   }
}

void Tree::setPosition(float x, float y, float z)
{
   position = makeVector(x, y, z);
}

void Tree::render() const
{
   glPushMatrix();

   gl::translate(position);
   glRotatef(angle, 0.0f, 1.0f, 0.0f);
   model->render();
   
   glPopMatrix();
}

xml::element Tree::toXml() const
{
   return xml::element("tree")
      .addAttribute("angle", angle)
      .addAttribute("name", name_);
}

namespace {
   Tree* loadTreeXml(IResourcePtr res)
   {
      log() << "Loading tree from " << res->xmlFileName();
   
      return new Tree(res);
   }

   shared_ptr<Tree> loadTreeFromCache(const string& name)
   {
      static ResourceCache<Tree> cache(loadTreeXml, "trees");
      return cache.loadCopy(name);
   }
}

ISceneryPtr loadTree(const string& name)
{
   shared_ptr<Tree> tree = loadTreeFromCache(name);
   
   // Randomise the new tree's angle
   static Uniform<float> angleRand(0.0f, 360.0f);
   tree.get()->setAngle(angleRand());

   return ISceneryPtr(tree);
}

ISceneryPtr loadTree(const AttributeSet& attrs)
{
   // Unserialise a tree
   float angle;
   string name;
   attrs.get("name", name);
   attrs.get("angle", angle);

   shared_ptr<Tree> tree = loadTreeFromCache(name);
   tree->setAngle(angle);

   return ISceneryPtr(tree);
}

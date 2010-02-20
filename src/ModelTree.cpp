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

#include <boost/cast.hpp>

// A tree which is just a 3D model
class ModelTree : public IScenery, public IXMLCallback {
public:
   ModelTree(IResourcePtr res);

   // IScenery interface
   void render() const;
   void setPosition(float x, float y, float z);

   // IXMLCallback interface
   void text(const string& localName, const string& content);
   
private:
   Vector<float> position;
   IModelPtr model;

   struct ParserState {
      string modelFile;
      float scale;
   } parserState;
};

ModelTree::ModelTree(IResourcePtr res)
{
   static IXMLParserPtr parser = makeXMLParser("schemas/tree.xsd");

   parser->parse(res->xmlFileName(), *this);
   
   model = loadModel(res, parserState.modelFile, parserState.scale);
}

void ModelTree::text(const string& localName, const string& content)
{
   if (localName == "model")
      parserState.modelFile = content;
   else if (localName == "scale")
      parserState.scale = boost::lexical_cast<float>(content);
}

void ModelTree::setPosition(float x, float y, float z)
{
   position = makeVector(x, y, z);
}

void ModelTree::render() const
{
   glPushMatrix();

   gl::translate(position);
   model->render();
   
   glPopMatrix();
}

namespace {
   ModelTree* loadTreeXml(IResourcePtr res)
   {
      log() << "Loading tree from " << res->xmlFileName();

      return new ModelTree(res);
   }
}

ISceneryPtr makeModelTree()
{
   static ResourceCache<ModelTree> cache(loadTreeXml, "trees");
   return cache.loadCopy("apple");
}


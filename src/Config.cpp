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

#include "IConfig.hpp"
#include "ILogger.hpp"
#include "IXMLParser.hpp"
#include "XMLBuilder.hpp"

#include <map>
#include <stdexcept>
#include <fstream>
#include <cstdlib>
#include <typeinfo>

#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

namespace {
   typedef tuple<const char*, IConfig::Option> Default;
   const Default END("", 0);
   
   // All valid options   
   Default theDefaults[] = {
      Default("XRes", 800),
      Default("YRes", 600),
      Default("NearClip", 0.1f),
      Default("FarClip", 70.0f),
      END
   };
}

// Concrete config file
class Config : public IConfig, public IXMLCallback {
public:
   Config();
   ~Config();

   // IConfig interface
   const IConfig::Option& get(const string& aKey) const;
   void set(const string& aKey, const IConfig::Option& aValue);
   void flush();

   // IXMLCallback interface
   void startElement(const string& localName, const AttributeSet& attrs);
   void text(const string& localName, const string& aString);
   
private:
   static string configFileName();

   template <class T>
   void setFromString(const string& aKey, const string& aString);
   
   template <class T>
   void bindNextOption(const AttributeSet& attrs);
   
   typedef map<string, IConfig::Option> ConfigMap;
   ConfigMap myConfigMap;

   string myConfigFile;
   bool amDirty;

   // Used by the XML parser
   function<void (const string&)> myOptionSetter;
};

// Read the config file from disk
Config::Config()
   : amDirty(false)
{
   for (Default* d = theDefaults; *::get<0>(*d) != '\0'; d++)
      myConfigMap[::get<0>(*d)] = ::get<1>(*d);

   myConfigFile = configFileName();

   if (boost::filesystem::exists(myConfigFile)) {
      log() << "Reading config from " << myConfigFile;

      IXMLParserPtr parser = makeXMLParser("schemas/config.xsd");
      parser->parse(myConfigFile, *this);

      // Ignore all the set() calls made by the XML parser
      amDirty = false;
   }
   else {
      warn() << "Config file not present: " << myConfigFile;

      // Write a default config file when we exit
      amDirty = true;
   }
}

Config::~Config()
{
   flush();
}

// Find the config file location on this platform
string Config::configFileName()
{   
#ifdef WIN32
   throw runtime_error("TODO: find config dir on Win32");

#else  // #ifdef WIN32

   // This is based on the XDG standard
   // See: http://standards.freedesktop.org/basedir-spec/latest/

   ostringstream ss;
   
   char* XDG_CONFIG_HOME = getenv("XDG_CONFIG_HOME");
   if (XDG_CONFIG_HOME == NULL || *XDG_CONFIG_HOME == '\0') {
      warn() << "XDG_CONFIG_HOME not set: using ~/.config";

      char* HOME = getenv("HOME");
      if (HOME == NULL)
         throw runtime_error("$HOME not set");

      ss << HOME << "/.config";
   }
   else
      ss << XDG_CONFIG_HOME;

   ss << "/TrainGame/config.xml";
            
   return ss.str();

#endif // #ifdef WIN32
}

void Config::startElement(const string& localName, const AttributeSet& attrs)
{
   if (localName == "stringOption")
      bindNextOption<string>(attrs);
   else if (localName == "intOption")
      bindNextOption<int>(attrs);
   else if (localName == "floatOption")
      bindNextOption<float>(attrs);
   else if (localName == "boolOption")
      bindNextOption<bool>(attrs);
}

void Config::text(const string& localName, const string& aString)
{
   myOptionSetter(aString);
}

template <class T>
void Config::bindNextOption(const AttributeSet& attrs)
{
   using namespace placeholders;
   
   myOptionSetter = bind(&Config::setFromString<T>, this,
      attrs.get<string>("name"), _1);
}

template <class T>
void Config::setFromString(const string& aKey, const string& aString)
{
   myConfigMap[aKey] = boost::lexical_cast<T>(aString);
}

// Write the config file back to disk
void Config::flush()
{
   using namespace boost::filesystem;
   using namespace boost;
   
   if (!amDirty)
      return;
   
   log() << "Saving config to " << myConfigFile;

   create_directories(path(myConfigFile).remove_filename());

   ofstream ofs(myConfigFile.c_str());
   if (!ofs.good())
      throw runtime_error("Failed to write to config file");

   xml::element root("config");
   for (ConfigMap::const_iterator it = myConfigMap.begin();
        it != myConfigMap.end(); ++it) {

      // We can only serialize some types
      const any& a = (*it).second;
      const type_info& t = a.type();
      string text, typeName;
      if (t == typeid(string)) {
         typeName = "string";
         text = any_cast<string>(a);
      }
      else if (t == typeid(int)) {
         typeName = "int";
         text = lexical_cast<string>(any_cast<int>(a));
      }
      else if (t == typeid(float)) {
         typeName = "float";
         text = lexical_cast<string>(any_cast<float>(a));
      }
      else if (t == typeid(bool)) {
         typeName = "bool";
         text = lexical_cast<string>(any_cast<bool>(a));
      }
      else
         throw runtime_error(
            "Cannot XMLify objects of type "
            + boost::lexical_cast<string>(t.name()));

      xml::element option(typeName + "Option");
      option.addAttribute("name", (*it).first);
      option.addText(text);
      
      root.addChild(option);
   }
   
   ofs << xml::document(root);

   amDirty = false;
}

// Read a single option
const IConfig::Option& Config::get(const string& aKey) const
{
   ConfigMap::const_iterator it = myConfigMap.find(aKey);
   if (it != myConfigMap.end())
      return (*it).second;
   else
      throw runtime_error("Bad config key " + aKey);
}

void Config::set(const string& aKey, const IConfig::Option& aValue)
{
   myConfigMap[aKey] = aValue;
   amDirty = true;
}

// Return the single config file instance
IConfigPtr getConfig()
{
   static IConfigPtr cfg(new Config);

   return cfg;
}


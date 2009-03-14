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

#include "ILogger.hpp"

#include <iostream>

using namespace std;
using namespace std::tr1;

// Concrete logger implementation
class LoggerImpl : public ILogger {
public:
   PrintLinePtr writeMsg(LogMsg::Type type);
};

PrintLinePtr LoggerImpl::writeMsg(LogMsg::Type type)
{
   switch (type) {
   case LogMsg::NORMAL:
      cout << "I) ";
      break;
   case LogMsg::DEBUG:
      cout << "D) ";
      break;
   case LogMsg::WARN:
      cout << "!) ";
      break;
   }
   return PrintLinePtr(new PrintLine(cout));
}

PrintLine::PrintLine(ostream& aStream)
   : stream(aStream)
{
   
}

PrintLine::~PrintLine()
{
   stream << endl;
}

// Return the single instance of Logger
ILoggerPtr getLogger()
{
   static ILoggerPtr logger(new LoggerImpl);
   return logger;
}

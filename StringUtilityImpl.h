// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __FabricSpliceImpl_STRINGUTILITYIMPL_H__
#define __FabricSpliceImpl_STRINGUTILITYIMPL_H__

#include <string>
#include <vector>
#include <map>

namespace FabricSpliceImpl
{
  typedef std::vector<std::string> stringVector;
  typedef std::map<std::string, std::string> stringMap;
  typedef stringMap::iterator stringIt;
  typedef stringMap::const_iterator stringConstIt;
  typedef std::pair<std::string, std::string> stringPair;
  typedef std::map<std::string, int> stringIntMap;
  typedef stringIntMap::iterator stringIntIt;
  typedef stringIntMap::const_iterator stringIntConstIt;
  typedef std::pair<std::string, int> stringIntPair;

  class StringUtilityImpl
  {
  public:
    static std::string getUniqueString(const std::string & name, const stringMap & names);
    static stringVector splitString(const std::string & name, char delimiter, bool stripSpaces = false);
    static std::string stripString(const std::string & name, char charToRemove = ' ');
    static stringVector partitionString(const std::string & name, char delimiter);
    static std::string replaceString(const std::string & name, char search, char replacement);
    static std::string removeCharFromString(const std::string & name, char search);
    static std::string removeDoubleSpacesFromString(const std::string & name);
    static bool startsWith(const std::string & name, const std::string & start);
    static bool endsWith(const std::string & name, const std::string & end);
    static std::string truncateLeft(const std::string & name, unsigned int charsToRemove);
    static std::string truncateRight(const std::string & name, unsigned int charsToRemove);
    static int findString(const std::string & name, const std::string & search);
  };
}

#endif

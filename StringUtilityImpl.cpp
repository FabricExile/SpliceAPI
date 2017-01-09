// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "StringUtilityImpl.h"
#include <sstream>
#include <stdio.h>
#include <stdlib.h>

using namespace FabricSpliceImpl;

std::string StringUtilityImpl::getUniqueString(const std::string & name, const stringMap & names)
{
  std::string key = name;
  char buffer[10];
  int suffixIndex = 1;

  while(names.find(key) != names.end())
  {
    sprintf(buffer, "%d", suffixIndex);
    suffixIndex++;
    key = name + buffer;
  }

  return key;
}

stringVector StringUtilityImpl::splitString(const std::string & name, char delimiter, bool stripSpaces)
{
  std::stringstream ss(name);

  // [andrew 20151111] when running under mayapy there is an invalid free() the first time getline()
  // is called unless 'item' has a non-empty value, i can only assume this is some kind of compiler
  // issue
  std::string item("empty");

  stringVector result;
  while(std::getline(ss, item, delimiter))
  {
    if(stripSpaces)
      result.push_back(stripString(item, ' '));
    else
      result.push_back(item);
  }
  return result;
}

stringVector StringUtilityImpl::partitionString(const std::string & name, char delimiter)
{
  stringVector result;
  size_t pos = name.find(delimiter);
  if(pos == std::string::npos)
  {
    result.push_back(name);
    result.push_back("");
    result.push_back("");
  }
  else
  {
    result.push_back(name.substr(0, pos));
    result.push_back(std::string()+delimiter);
    result.push_back(name.substr(pos + 1, name.length() - pos));
  }

  return result;
}

std::string StringUtilityImpl::stripString(const std::string & name, char charToRemove)
{
  if(name.length() == 0)
    return name;

  size_t start = 0;
  size_t end = name.length()-1;

  while(name[start] == charToRemove && start != end)
    start++;
  while(name[end] == charToRemove && start != end)
    end--;

  return name.substr(start, end - start + 1);
}

std::string StringUtilityImpl::replaceString(const std::string & name, char search, char replacement)
{
  std::string result = name;
  for(size_t i=0;i<result.length();i++)
  {
    if(result[i] == search)
      result[i] = replacement;
  }
  return result;
}

std::string StringUtilityImpl::removeCharFromString(const std::string & name, char search)
{
  std::string result;
  for(size_t i=0;i<name.length();i++)
  {
    if(name[i] == search)
      continue;
    result += name[i];
  }
  return result;
}

std::string StringUtilityImpl::removeDoubleSpacesFromString(const std::string & name)
{
  std::string result;
  for(size_t i=0;i<name.length();i++)
  {
    if(result.length() > 0)
    {
      if(name[i] == ' ' && result[result.length()-1] == ' ')
        continue;
    }
    result += name[i];
  }
  return result;
}

bool StringUtilityImpl::startsWith(const std::string & name, const std::string & start)
{
  if(name.length() < start.length() || start.length() == 0)
    return false;
  if(name == start)
    return true;
  return name.substr(0, start.length()) == start;
}

bool StringUtilityImpl::endsWith(const std::string & name, const std::string & end)
{
  if(name.length() < end.length() || end.length() == 0)
    return false;
  if(name == end)
    return true;
  return name.substr(name.length()-end.length(), end.length()) == end;
}

std::string StringUtilityImpl::truncateLeft(const std::string & name, unsigned int charsToRemove)
{
  if(charsToRemove > name.length())
    return "";
  return name.substr(charsToRemove, name.length() - charsToRemove);
}

std::string StringUtilityImpl::truncateRight(const std::string & name, unsigned int charsToRemove)
{
  if(charsToRemove > name.length())
    return "";
  return name.substr(0, name.length() - charsToRemove);
}

int StringUtilityImpl::findString(const std::string & name, const std::string & search)
{
  if(name.length() < search.length())
    return -1;
  if(name == search)
    return 0;

  for(size_t i=0;i<1 + name.length() - search.length();i++)
  {
    std::string segment = name.substr(i, search.length());
    if(segment == search)
      return i;
  }
  return -1;
}

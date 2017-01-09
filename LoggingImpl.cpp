// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "LoggingImpl.h"
#include "SceneManagementImpl.h"
#include <stdio.h>
#include <sstream>
#include <string>

#ifdef _WIN32
  #include <windows.h>
#else
  #include <sys/time.h>
#endif

using namespace FabricSpliceImpl;

LoggingFunc LoggingImpl::sLogFunc = NULL;
LoggingFunc LoggingImpl::sErrorLogFunc = NULL;
CompilerErrorFunc LoggingImpl::sCompilerErrorFunc = NULL;
LoggingFunc LoggingImpl::sKLReportFunc = NULL;
StatusFunc LoggingImpl::sKLStatusFunc = NULL;
SlowOperationFunc LoggingImpl::sSlowOperationFunc = NULL;
std::string LoggingImpl::mErrorLog;
bool LoggingImpl::sTimersEnabled = false;
std::map<std::string, LoggingImpl::TimeInfo> LoggingImpl::sTimers;

#ifdef _WIN32
  long double gSecondsPerTick = 0.0;
#endif

void LoggingImpl::setLogFunc(LoggingFunc func)
{
  sLogFunc = func;
}

void LoggingImpl::setLogErrorFunc(LoggingFunc func)
{
  sErrorLogFunc = func;
}

void LoggingImpl::setCompilerErrorFunc(CompilerErrorFunc func)
{
  sCompilerErrorFunc = func;
}

void LoggingImpl::setKLReportFunc(LoggingFunc func)
{
  sKLReportFunc = func;
}

void LoggingImpl::setKLStatusFunc(StatusFunc func)
{
  sKLStatusFunc = func;
}

void LoggingImpl::setSlowOperationFunc(SlowOperationFunc func)
{
  sSlowOperationFunc = func;
}

void LoggingImpl::log(const std::string & message) 
{
  static bool haveEnabled = false;
  static bool enabled;
  if ( !haveEnabled )
  {
    haveEnabled = true;
    char const *envvar = getenv( "FABRIC_SPLICE_DISABLE_LOG" );
    bool disabled = envvar && atoi( envvar ) >= 1;
    enabled = !disabled;
  }

  if ( enabled )
  {
    if(sLogFunc)
      (*sLogFunc)(message.c_str(), message.length());
    else
      printf("%s\n", message.c_str());
  }
}

void LoggingImpl::logError(const std::string & message) 
{
  if(sErrorLogFunc)
    (*sErrorLogFunc)(message.c_str(), message.length());
  else
    printf("Error: %s\n", message.c_str());
  mErrorLog = message;
}

bool LoggingImpl::reportError(const std::string & message, std::string * errorOut)
{
  if(errorOut) {
    if((*errorOut).length() > 0)
      *errorOut += "\n";
    *errorOut += message;
  } else
    logError(message);
  mErrorLog = message;
  return false;
}

bool LoggingImpl::reportCompilerError(
  unsigned int row, 
  unsigned int col, 
  const std::string & file,
  const std::string & level,
  const std::string & desc
) {
  SceneManagementImpl::setErrorStatus(true);
  if(sCompilerErrorFunc)
    (*sCompilerErrorFunc)(row, col, file.c_str(), level.c_str(), desc.c_str());
  else
    printf("Compiler %s: %s, Line %d, Row %d: %s\n", level.c_str(), file.c_str(), (int)row, (int)col, desc.c_str());
  char buffer[2048];
#ifdef _WIN32
  sprintf_s(buffer, 2048, "Compiler %s: %s, Line %d, Row %d: %s\n", level.c_str(), file.c_str(), (int)row, (int)col, desc.c_str());
#else
  snprintf(buffer, 2048, "Compiler %s: %s, Line %d, Row %d: %s\n", level.c_str(), file.c_str(), (int)row, (int)col, desc.c_str());
#endif
  mErrorLog = buffer;
  return false;
}

void LoggingImpl::reportCallback(void * reportUserData, char const * message, unsigned int messageLength)
{
  log(message);
}

bool LoggingImpl::hasError()
{
  return mErrorLog.length() > 0;
}

const char * LoggingImpl::getError()
{
  return mErrorLog.c_str();
}

void LoggingImpl::clearError()
{
  mErrorLog = "";
}

void LoggingImpl::enableTimers()
{
  sTimersEnabled = true;
}

void LoggingImpl::disableTimers()
{
  sTimersEnabled = false;
}

void LoggingImpl::resetTimer(const char * name)
{
  if(!sTimersEnabled)
    return;
  std::map<std::string, TimeInfo>::iterator it = sTimers.find(name);
  if(it == sTimers.end())
    return;
  it->second.elapsed = 0;
  it->second.running = false;
}

void LoggingImpl::startTimer(const char * name)
{
  if(!sTimersEnabled)
    return;
  std::map<std::string, TimeInfo>::iterator it = sTimers.find(name);
  if(it == sTimers.end())
  {
    TimeInfo info;
    info.running = false;
    info.name = name;
    info.start = 0;
    info.elapsed = 0;
    sTimers.insert(std::pair<std::string, TimeInfo>(name, info));
    it = sTimers.find(name);
  }
  if(it->second.running)
    return;
  it->second.running = true;

  #ifdef _WIN32
    if(!gSecondsPerTick)
    {
      LARGE_INTEGER ticksPerSecond;
      QueryPerformanceFrequency( &ticksPerSecond );
      gSecondsPerTick = 1.0 / double(ticksPerSecond.QuadPart);
    }

    LARGE_INTEGER t;
    QueryPerformanceCounter(&t);
    it->second.start = double(t.QuadPart) * gSecondsPerTick;
  #else
    timeval t;
    gettimeofday(&t, NULL);
    it->second.start = t.tv_sec * 1000.0 + t.tv_usec / 1000.0;
  #endif
}

void LoggingImpl::stopTimer(const char * name)
{
  if(!sTimersEnabled)
    return;
  std::map<std::string, TimeInfo>::iterator it = sTimers.find(name);
  if(it == sTimers.end())
    return;
  if(!it->second.running)
    return;
  it->second.running = false;

  #ifdef _WIN32
    LARGE_INTEGER t;
    QueryPerformanceCounter(&t);
    long double portion = double(t.QuadPart) * gSecondsPerTick - it->second.start;
    it->second.elapsed += portion;
  #else
    timeval t;
    gettimeofday(&t, NULL);
    long double portion = t.tv_sec * 1000.0 + t.tv_usec / 1000.0 - it->second.start;
    it->second.elapsed += portion;
  #endif
}

void LoggingImpl::logTimer(const char * name)
{
  if(!sTimersEnabled)
    return;
  stopTimer(name);
  std::map<std::string, TimeInfo>::iterator it = sTimers.find(name);
  if(it == sTimers.end())
  {
    log("Timer "+std::string(name)+" not found!");
    return;
  }

  std::stringstream ss;
  ss << it->second.elapsed;
  log("Timer "+it->second.name+": "+ss.str()+"s.");
}

unsigned int LoggingImpl::getNbTimers()
{
  if(!sTimersEnabled)
    return 0;
  return sTimers.size();
}

char const * LoggingImpl::getTimerName(unsigned int index)
{
  if(!sTimersEnabled)
    return NULL;
  if(index >= sTimers.size())
    return NULL;
  std::map<std::string, TimeInfo>::iterator it = sTimers.begin();
  for(unsigned int i=0;i<index;i++)
    it++;
  return it->second.name.c_str();
}

LoggingImpl::AutoTimerImpl::AutoTimerImpl(const char * name) {
  mName = name;
  LoggingImpl::startTimer(mName);
}

LoggingImpl::AutoTimerImpl::~AutoTimerImpl() {
  LoggingImpl::stopTimer(mName);
}


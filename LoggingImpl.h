// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __FabricSpliceImpl__LOGGINGIMPL_H__
#define __FabricSpliceImpl__LOGGINGIMPL_H__

#include "StringUtilityImpl.h"
#include <FabricCore.h>

namespace FabricSpliceImpl
{
  /// a function used to receive a single message string
  typedef void(*LoggingFunc)(const char * message, unsigned int messageLength);

  /// a function used to receive a compiler error
  typedef void(*CompilerErrorFunc)(unsigned int row, unsigned int col, const char * file, const char * level, const char * desc);

  /// a function to receive information from a KL status message
  typedef void(*StatusFunc)(const char * topic, unsigned int topicLength, const char * message, unsigned int messageLength);

  /// a function to receive notification when a slow operation starting or finishing
  typedef void(*SlowOperationFunc)( const char *descCStr, uint32_t descLength );

  class LoggingImpl
  {
  public:

    /// sets the callback for generic log messages
    static void setLogFunc(LoggingFunc func);

    /// sets the callback for error log messages
    static void setLogErrorFunc(LoggingFunc func);

    /// sets the callback for KL compiler error messages
    static void setCompilerErrorFunc(CompilerErrorFunc func);

    /// sets the callback for KL report statements
    static void setKLReportFunc(LoggingFunc func);

    /// returns the callback for KL report statements
    static LoggingFunc getKLReportFunc() { return sKLReportFunc; }

    /// sets the callback for KL queueStatusMessage statements
    static void setKLStatusFunc(StatusFunc func);

    /// gets the callback for KL queueStatusMessage statements
    static StatusFunc getKLStatusFunc() { return sKLStatusFunc; }

    /// sets the callback for slow operations
    static void setSlowOperationFunc( SlowOperationFunc func );

    /// gets the callback for slow operations
    static SlowOperationFunc getSlowOperationFunc() { return sSlowOperationFunc; }

    /// logs to the logFunc callback
    static void log(const std::string & message);

    /// logs to the logErrorFunc callback
    static void logError(const std::string & message);

    /// reports an error to the logErrorFunc (or optionally to the provided string pointer)
    static bool reportError(const std::string & message, std::string * errorOut = NULL);

    /// reports a KL compiler error the klCompilerError callback
    static bool reportCompilerError(unsigned int row, unsigned int col, const std::string & file, const std::string & level, const std::string & desc);

    /// provides a callback to work with the core client directly
    static void reportCallback(void * reportUserData, const char * message, unsigned int messageLength);

    /// returns true if there is an error in the 
    static bool hasError();

    /// returns the latest error 
    static const char * getError();

    /// clears the error 
    static void clearError();

    /// enable timers
    static void enableTimers();

    /// disable timers
    static void disableTimers();

    /// reset a timer
    static void resetTimer(const char * name);

    /// start a timer
    static void startTimer(const char * name);

    /// stop a timer and accumulate the time
    static void stopTimer(const char * name);

    /// log a given timer
    static void logTimer(const char * name);

    /// return the number of existing timers
    static unsigned int getNbTimers();

    /// return the number of existing timers
    static char const * getTimerName(unsigned int index);

    /// a timer which records time on construction and destruction
    class AutoTimerImpl
    {
    public:
      AutoTimerImpl(const char * name);
      ~AutoTimerImpl();

    private:
      const char * mName;
    };

  private:
    static LoggingFunc sLogFunc;
    static LoggingFunc sErrorLogFunc;
    static CompilerErrorFunc sCompilerErrorFunc;
    static LoggingFunc sKLReportFunc;
    static StatusFunc sKLStatusFunc;
    static SlowOperationFunc sSlowOperationFunc;
    static std::string mErrorLog;
    struct TimeInfo {
      bool running;
      std::string name;
      long double start;
      long double elapsed;
    };
    static bool sTimersEnabled;
    static std::map<std::string, TimeInfo> sTimers;
  };
}

#endif

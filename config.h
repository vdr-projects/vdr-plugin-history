/*
 * config.h: Global configuration
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __HISTORY_CONFIG_H_
#define __HISTORY_CONFIG_H_

#include <vdr/tools.h>

class cHistorySetup
{
public:
  int allow_delete;
  int replay_history_size;
  cHistorySetup();
  bool SetupParse(const char *Name, const char *Value);
  bool ProcessArgs(int argc, char *argv[]);

protected:
  bool ProcessArg(const char *Name, const char *Value);
  static cString m_ProcessedArgs;
};

// Global instance
extern cHistorySetup HistorySetup;

#endif //__HISTORY_CONFIG_H_

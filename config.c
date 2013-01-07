/*
 * config.c: Global configuration
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include "config.h"
#include <string.h>

/* Global instance */
cHistorySetup HistorySetup;

cHistorySetup::cHistorySetup()
{
  allow_delete = 1;
  replay_history_size = 10;
}

cString cHistorySetup::m_ProcessedArgs;

bool cHistorySetup::ProcessArg(const char *Name, const char *Value)
{
  if (SetupParse(Name, Value)) {
     m_ProcessedArgs = cString::sprintf("%s%s ", *m_ProcessedArgs ? *m_ProcessedArgs : " ", Name);
     return true;
     }
  return false;
}

bool cHistorySetup::ProcessArgs(int argc, char *argv[])
{
  return true;
}

bool cHistorySetup::SetupParse(const char *Name, const char *Value)
{
  const char *pt;
  if (*m_ProcessedArgs && NULL != (pt = strstr(m_ProcessedArgs + 1, Name)) &&
      *(pt - 1) == ' ' && *(pt + strlen(Name)) == ' ') {
     dsyslog("Skipping configuration entry %s=%s (overridden in command line)", Name, Value);
     return true;
     }

  if (!strcasecmp(Name, "ReplayHistorySize"))                replay_history_size = atoi(Value);
  else if (!strcasecmp(Name, "AllowDelete"))                 allow_delete = atoi(Value);
  else
     return false;

  return true;
}

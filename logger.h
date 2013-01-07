/*
 * logger.h: Keep log of replayed recordings
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __HISTORY_LOGGER_H_
#define __HISTORY_LOGGER_H_

#include <time.h>
#include <vdr/status.h>
#include <vdr/tools.h>
#include "config.h"

class cHistoryRecordingItem: public cListObject
{
protected:
  char *filename;
  char *name;
  time_t replay_time;

public:
  cHistoryRecordingItem(const char *String);
  cHistoryRecordingItem(const char *Name, const char *FileName);
  ~cHistoryRecordingItem() { free(filename); free(name); }
  const char *GetFilename() { return filename; }
  const char *GetName() { return name; }
  time_t *GetReplayTime() { return &replay_time; }
  char *GetReplayTimeString();
};

class cHistoryLogger : public cStatus
{
public:
  cHistoryLogger();
  ~cHistoryLogger();
  void DeleteRecordingItem(int index);
  cHistoryRecordingItem *GetRecordingItem(int index);
  cList<cHistoryRecordingItem> *GetReplayHistory();
  void ClearReplayHistory();
  void LoadReplayHistory();
  void StoreReplayHistory();
  void SetReplayHistoryConfigFile(const char *FileName);

protected:
  virtual void Replaying(const cControl *Control, const char *Name, const char *FileName, bool On);

private:
  char *replay_history_filename;
  cList<cHistoryRecordingItem> *replay_history;
};

#endif //__HISTORY_LOGGER_H_

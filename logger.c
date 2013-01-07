/*
 * logger.c: Keep log of replayed recordings
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include "logger.h"
#include <string.h>

cHistoryRecordingItem::cHistoryRecordingItem(const char *String)
{
  filename = NULL;
  name = NULL;
  if (String && *String) {
     char *s = strdup(String);
     char *p = strchr(s, ':');
     if (p) {
        replay_time = atol(s);
        char *r = strchr(p, '=');
        if (r) {
           *r = 0;
           filename = strdup(p + 1);
           name = strdup(r + 1);
           }
        else
           filename = strdup(p + 1);
        }
     else {
        replay_time = 0;
        }
     free(s);
     }
}

// Use name only if not a VDR recording
cHistoryRecordingItem::cHistoryRecordingItem(const char *Name, const char *FileName)
{
  replay_time = time(NULL);
  filename = FileName ? strdup(FileName) : NULL;
  name = Name ? strdup(Name) : NULL;
}

char *cHistoryRecordingItem::GetReplayTimeString()
{
  struct tm *t = localtime(&replay_time);
  char *time;
  if (asprintf(&time, "%02d.%02d.%02d %02d:%02d",
                      t->tm_mday, t->tm_mon + 1, t->tm_year % 100,
                      t->tm_hour, t->tm_min))
     return time;
  else
     return NULL;
}

cHistoryLogger::cHistoryLogger()
{
  replay_history_filename = NULL;
  replay_history = new cList<cHistoryRecordingItem>();
}

cHistoryLogger::~cHistoryLogger()
{
  free(replay_history_filename);
  replay_history->Clear();
  delete replay_history;
}

void cHistoryLogger::Replaying(const cControl *Control, const char *Name, const char *FileName, bool On)
{
  if (On) {
     if (FileName) {
        // Check if file is a VDR recording
        cThreadLock RecordingsLock(&Recordings);
        cRecording *recording = Recordings.GetByName(FileName);
        // Use name only if not a VDR recording
        replay_history->Ins(new cHistoryRecordingItem(!recording ? Name : NULL, FileName));
        if (replay_history->Count() > HistorySetup.replay_history_size)
           replay_history->Del(replay_history->Last());
        StoreReplayHistory();
        }
     }
}

void cHistoryLogger::DeleteRecordingItem(int index)
{
  if (index >= 0 && index < replay_history->Count()) {
     replay_history->Del(replay_history->Get(index));
     StoreReplayHistory();
     }
}

cHistoryRecordingItem *cHistoryLogger::GetRecordingItem(int index)
{
  if (index >= 0 && index < replay_history->Count())
     return replay_history->Get(index);
  return NULL;
}

cList<cHistoryRecordingItem> *cHistoryLogger::GetReplayHistory()
{
  return replay_history;
}

void cHistoryLogger::ClearReplayHistory()
{
  replay_history->Clear();
  if (*replay_history_filename) {
     FILE *f = fopen(replay_history_filename, "w");
     if (f) {
        fclose(f);
        }
     else {
        LOG_ERROR_STR(replay_history_filename);
        }
     }
}

void cHistoryLogger::LoadReplayHistory()
{
  if (*replay_history_filename && access(replay_history_filename, F_OK) == 0) {
     FILE *f = fopen(replay_history_filename, "r");
     if (f) {
        char *s;
        int line = 0;
        cReadLine ReadLine;
        while ((s = ReadLine.Read(f)) != NULL) {
              ++line;
              if (!isempty(s)) {
                 replay_history->Add(new cHistoryRecordingItem(s));
                 }
              }
        fclose(f);
        }
     else {
        LOG_ERROR_STR(replay_history_filename);
        }
     }
}

void cHistoryLogger::StoreReplayHistory()
{
  if (*replay_history_filename) {
     FILE *f = fopen(replay_history_filename, "w");
     if (f) {
        cHistoryRecordingItem *item = replay_history->First();
        while (item != NULL) {
              if (item->GetName())
                 fprintf(f, "%li:%s=%s\n", *(item->GetReplayTime()), item->GetFilename(), item->GetName());
              else
                 fprintf(f, "%li:%s\n", *(item->GetReplayTime()), item->GetFilename());
              item = (cHistoryRecordingItem *)item->Next();
              }
        fclose(f);
        }
     else {
        LOG_ERROR_STR(replay_history_filename);
        }
     }
}

void cHistoryLogger::SetReplayHistoryConfigFile(const char *FileName)
{
  free(replay_history_filename);
  replay_history_filename = strdup(FileName);
}

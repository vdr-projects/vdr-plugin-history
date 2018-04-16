/*
 * menu.c: The actual menu implementations
 */

#include <stdlib.h>
#include <time.h>
#include <vdr/interface.h>
#include <vdr/plugin.h>
#include <vdr/remote.h>
#include <vdr/thread.h>
#include "config.h"
#include "logger.h"
#include "menu.h"
#include "tools.h"

enum {
  TYPE_NONE,
  TYPE_IMAGE,
  TYPE_VIDEO,
  TYPE_MUSIC
};

class cHistoryRecordingMenu : public cOsdMenu {
private:
  const cRecording *recording;

public:
  cHistoryRecordingMenu(const cRecording *Recording);
  virtual void Display(void);
  virtual eOSState ProcessKey(eKeys Key);
};

cHistoryRecordingMenu::cHistoryRecordingMenu(const cRecording *Recording)
:cOsdMenu(trVDR("Recording info"))
{
#if VDRVERSNUM >= 10728
  SetMenuCategory(mcRecording);
#endif
  recording = Recording;
  SetHelp(trVDR("Button$Play"),trVDR("Button$Rewind"));
}

void cHistoryRecordingMenu::Display(void)
{
  cOsdMenu::Display();
  DisplayMenu()->SetRecording(recording);
  if (recording->Info()->Description())
     cStatus::MsgOsdTextItem(recording->Info()->Description());
}

eOSState cHistoryRecordingMenu::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
     switch (Key) {
       case kPlay:
       case kRed:    cRemote::Put(Key, true);
       case kOk:     return osBack;
       default: break;
       }
     }
  return state;
}

class cHistoryRecordingMenuItem : public cOsdItem
{
private:
  char *filename;
  char *name;
  bool active;

public:
  cHistoryRecordingMenuItem(const char *Name, const char *Filename);
  ~cHistoryRecordingMenuItem() { free(filename); free(name); }
  const char *GetFilename() { return filename; }
  const char *GetName() { return name; }
  bool IsActive() { return active; }
};

// Use name only if not a VDR recording
cHistoryRecordingMenuItem::cHistoryRecordingMenuItem(const char *Name, const char *Filename)
{
  active = false;
  filename = Filename ? strdup(Filename) : NULL;
  name = Name ? strdup(Name) : NULL;
  if (filename) {
     if (!name) {
        const cRecording *recording;
#if VDRVERSNUM >= 20301
        {
        LOCK_RECORDINGS_READ;
        recording = Recordings->GetByName(filename);
        }
#else
        cThreadLock RecordingsLock(&Recordings);
        recording = Recordings.GetByName(filename);
#endif
        if (recording) {
           active = true;
           SetText(recording->Title('\t'));
           }
        else
           SetText(tr("Deleted or not available"));
        }
     else {
        active = true;
        SetText(name);
        }
     }
  else
     SetText(tr("Deleted or not available"));
}

cHistoryMainMenu::cHistoryMainMenu(cHistoryLogger *HistoryLogger) : cOsdMenu(tr("Replay history"), 9, 6, 6)
{
  history_logger = HistoryLogger;
  SetMenu();
  SetHelpKeys();
  Display();
}

void cHistoryMainMenu::SetHelpKeys()
{
  if (history_logger->GetReplayHistory()->Count() > 0) {
     int curr = Current();
     if (curr == -1) {
        for (cOsdItem *item = First(); item; item = Next(item)) {
            if (item->Selectable()) {
               curr = item->Index();
               break;
               }
            }
        }
     cHistoryRecordingMenuItem *ri = (cHistoryRecordingMenuItem *)Get(curr);
     if (ri && ri->IsActive() && !ri->GetName())
        SetHelp(trVDR("Button$Play"), trVDR("Button$Rewind"), trVDR("Button$Delete"), trVDR("Button$Info"));
     else if (ri && ri->IsActive())
        SetHelp(trVDR("Button$Play"), NULL, trVDR("Button$Delete"), NULL);
     else
        SetHelp(NULL, NULL, trVDR("Button$Delete"), NULL);
     }
}

void cHistoryMainMenu::SetMenu()
{
  Clear();
#if VDRVERSNUM >= 10728
  SetMenuCategory(mcRecording);
#endif
  cList<cHistoryRecordingItem> *replay_history = history_logger->GetReplayHistory();
  cHistoryRecordingItem *item = replay_history->First();
  while (item != NULL) {
        char *time = item->GetReplayTimeString();
        Add(new cOsdItem(time, osUnknown, false));
        free(time);
        Add(new cHistoryRecordingMenuItem(item->GetName(), item->GetFilename()));
        item = (cHistoryRecordingItem *)item->Next();
        }
}

#if VDRVERSNUM >= 20301
const cRecording *cHistoryMainMenu::GetRecordingRead(cHistoryRecordingMenuItem *Item)
{
  LOCK_RECORDINGS_READ;
  const cRecording *recording = Recordings->GetByName(Item->GetFilename());
  if (!recording)
     Skins.Message(mtError, trVDR("Error while accessing recording!"));
  return recording;
}
#endif

cRecording *cHistoryMainMenu::GetRecording(cHistoryRecordingMenuItem *Item)
{
#if VDRVERSNUM >= 20301
  LOCK_RECORDINGS_WRITE;
  cRecording *recording = Recordings->GetByName(Item->GetFilename());
#else
  cThreadLock RecordingsLock(&Recordings);
  cRecording *recording = Recordings.GetByName(Item->GetFilename());
#endif
  if (!recording)
     Skins.Message(mtError, trVDR("Error while accessing recording!"));
  return recording;
}

eOSState cHistoryMainMenu::Delete()
{
  cHistoryRecordingMenuItem *ri = (cHistoryRecordingMenuItem *)Get(Current());
  if (HistorySetup.allow_delete && ri && ri->IsActive() && ((!ri->GetName() && Interface->Confirm(tr("Delete also recording?"))) || (ri->GetName() && Interface->Confirm(tr("Delete also file?"))))) {
     if (!ri->GetName()) {
        cRecording *recording = GetRecording(ri);
        if (recording && !recording->Delete())
           Skins.Message(mtError, tr("Unable to delete recording"));
        }
     else
        if (!RemoveFileOrDir(ri->GetFilename()))
           Skins.Message(mtError, tr("Unable to delete file"));
     }
  history_logger->DeleteRecordingItem(Current()/2);
  return osContinue;
}

eOSState cHistoryMainMenu::Info()
{
  cHistoryRecordingMenuItem *ri = (cHistoryRecordingMenuItem *)Get(Current());
  if (ri) {
     const cRecording *recording;
#if VDRVERSNUM >= 20301
     recording = GetRecordingRead(ri);
#else
     recording = GetRecording(ri);
#endif
     if (recording && recording->Info()->Title())
        return AddSubMenu(new cHistoryRecordingMenu(recording));
     }
  return osContinue;
}

eOSState cHistoryMainMenu::Play()
{
  cHistoryRecordingMenuItem *ri = (cHistoryRecordingMenuItem *)Get(Current());
  if (!ri->GetName()) {
     cReplayControl::SetRecording(ri->GetFilename());
     return osReplay;
     }
  else {
     const char *filename = ri->GetFilename();
     int type;
     if (isvideo(filename))
        type = TYPE_VIDEO;
     else if (ismusic(filename))
        type = TYPE_MUSIC;
     else if (isimage(filename))
        type = TYPE_IMAGE;
     else
        type = TYPE_NONE;
     switch (type) {
        case TYPE_VIDEO:
             cPluginManager::CallFirstService("MediaPlayer-1.0", (void *)filename);
             break;
        case TYPE_MUSIC:
             cPluginManager::CallFirstService("MusicPlayer-1.0", (void *)filename);
             break;
        case TYPE_IMAGE:
             cPluginManager::CallFirstService("ImagePlayer-1.0", (void *)filename);
             break;
        case TYPE_NONE:
        default:
             break;
       }
       return osEnd;
     }
}

eOSState cHistoryMainMenu::Rewind()
{
  cHistoryRecordingMenuItem *ri = (cHistoryRecordingMenuItem *)Get(Current());
  cRecording *recording = GetRecording(ri);
  if (recording) {
     cDevice::PrimaryDevice()->StopReplay(); // must do this first to be able to rewind the currently replayed recording
     cResumeFile ResumeFile(ri->GetFilename(), recording->IsPesRecording());
     ResumeFile.Delete();
     return Play();
     }
  return osContinue;
}

eOSState cHistoryMainMenu::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);
  if (state == osUnknown) {
     cHistoryRecordingMenuItem *ri = (cHistoryRecordingMenuItem *)Get(Current());
     if (ri) {
        switch (int(Key)) {
          case kOk:
          case kPlay:
          case kRed:
            state = Play();
            break;
          case kGreen:
            if (!ri->GetName())
               state = Rewind();
            break;
          case kYellow:
            state = Delete();
            SetMenu();
            SetHelpKeys();
            Display();
            break;
          case kBlue:
          case kInfo:
            if (!ri->GetName())
               state = Info();
            break;
          default:
            break;
          }
        }
     }
  else {
     switch (int(Key)) {
       case kUp:
       case kDown:
       case kLeft:
       case kRight:
         SetHelpKeys();
         Display();
         break;
       default:
         break;
       }
     }
  return state;
}

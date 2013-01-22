/*
 * history.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include <vdr/plugin.h>
#include <vdr/i18n.h>
#include "config.h"
#include "logger.h"
#include "menu.h"
#include "setup_menu.h"

#ifndef GITVERSION
#define GITVERSION ""
#endif

static const char VERSION[]        = "0.0.3" GITVERSION;
static const char DESCRIPTION[]    = trNOOP("Show last replayed recordings");
static const char MAINMENUENTRY[]  = trNOOP("History");

class cPluginHistory : public cPlugin {
private:
  // Add any member variables or functions you may need here.
  cHistoryLogger *history_logger;
public:
  cPluginHistory(void);
  virtual ~cPluginHistory();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return tr(DESCRIPTION); }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual void Stop(void);
  virtual void Housekeeping(void);
  virtual void MainThreadHook(void);
  virtual cString Active(void);
  virtual time_t WakeupTime(void);
  virtual const char *MainMenuEntry(void) { return tr(MAINMENUENTRY); }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  virtual bool Service(const char *Id, void *Data = NULL);
  virtual const char **SVDRPHelpPages(void);
  virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);
  };

cPluginHistory::cPluginHistory(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
  history_logger = NULL;
}

cPluginHistory::~cPluginHistory()
{
  // Clean up after yourself!
  delete history_logger;
}

const char *cPluginHistory::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return NULL;
}

bool cPluginHistory::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return true;
}

bool cPluginHistory::Initialize(void)
{
  // Initialize any background activities the plugin shall perform.
  return true;
}

bool cPluginHistory::Start(void)
{
  // Start any background activities the plugin shall perform.
  history_logger = new cHistoryLogger;
  history_logger->SetReplayHistoryConfigFile(AddDirectory(cPlugin::ConfigDirectory(PLUGIN_NAME_I18N), "replay.log")); // allowed only via main thread!);
  history_logger->LoadReplayHistory();
  return true;
}

void cPluginHistory::Stop(void)
{
  // Stop any background activities the plugin is performing.
}

void cPluginHistory::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

void cPluginHistory::MainThreadHook(void)
{
  // Perform actions in the context of the main program thread.
  // WARNING: Use with great care - see PLUGINS.html!
}

cString cPluginHistory::Active(void)
{
  // Return a message string if shutdown should be postponed
  return NULL;
}

time_t cPluginHistory::WakeupTime(void)
{
  // Return custom wakeup time for shutdown script
  return 0;
}

cOsdObject *cPluginHistory::MainMenuAction(void)
{
  // Perform the action when selected from the main VDR menu.
  return new cHistoryMainMenu(history_logger);
}

cMenuSetupPage *cPluginHistory::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return new cHistorySetupMenu();
}

bool cPluginHistory::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
  return HistorySetup.SetupParse(Name, Value);
}

bool cPluginHistory::Service(const char *Id, void *Data)
{
  // Handle custom service requests from other plugins
  return false;
}

const char **cPluginHistory::SVDRPHelpPages(void)
{
  static const char *HelpPages[] = {
    "REPL\n"
    "    Show replay history.",
    "CLRR\n"
    "    Clear replay history.",
    NULL
    };
  return HelpPages;
}

cString cPluginHistory::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode)
{
  if (strcasecmp(Command, "REPL") == 0) {
     if (HistorySetup.replay_history_size > 0) {
        cString string = "";
        cList<cHistoryRecordingItem> *replay_history = history_logger->GetReplayHistory();
        cHistoryRecordingItem *item = replay_history->First();
        while (item != NULL) {
              char *time = item->GetReplayTimeString();
              string = cString::sprintf("%s%s\n", *string, time);
              free(time);
              cThreadLock RecordingsLock(&Recordings);
              cRecording *recording = Recordings.GetByName(item->GetFilename());
              if (recording)
                 string = cString::sprintf("%s%s\n", *string, recording->Title(' '));
              item = (cHistoryRecordingItem *)item->Next();
              }
        return string;
        }
     }
  else if (strcasecmp(Command, "CLRR") == 0) {
     if (HistorySetup.replay_history_size > 0) {
        history_logger->ClearReplayHistory();
        return cString("Replay history cleared");
        }
     }
  else {
        ReplyCode = 554; // Requested action failed
        return cString("Unable to show replay history");
     }
  return NULL;
}

VDRPLUGINCREATOR(cPluginHistory); // Don't touch this!

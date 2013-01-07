/*
 * setup_menu.h: Setup Menu
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __HISTORY_MENU_H
#define __HISTORY_MENU_H

#include <vdr/menu.h>
#include <vdr/menuitems.h>
#include <vdr/recording.h>

class cHistoryRecordingMenuItem;

class cHistoryMainMenu : public cOsdMenu
{
private:
  cHistoryLogger *history_logger;
  void SetHelpKeys();
  void SetMenu();
  eOSState Delete();
  eOSState Info();
  eOSState Play();
  eOSState Rewind();
  cRecording *GetRecording(cHistoryRecordingMenuItem *Item);

public:
  cHistoryMainMenu(cHistoryLogger *HistoryLogger);
  virtual eOSState ProcessKey(eKeys Key);
};

#endif //__HISTORY_MENU_H

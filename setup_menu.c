/*
 * setup_menu.c: Setup Menu
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include "setup_menu.h"
#include <vdr/config.h>
#include <vdr/i18n.h>

//--- cHistorySetupMenu ------------------------------------------------------

cHistorySetupMenu::cHistorySetupMenu(void)
{
  memcpy(&newconfig, &HistorySetup, sizeof(cHistorySetup));
  Set();
}

void cHistorySetupMenu::Set(void)
{
  Clear();
  Add(new cMenuEditBoolItem(tr("Allow file delete"),
                            &newconfig.allow_delete));
  Add(new cMenuEditIntItem(tr("Replay history size"),
                            &newconfig.replay_history_size), 0);
  Display();
}

void cHistorySetupMenu::Store(void)
{
  memcpy(&HistorySetup, &newconfig, sizeof(cHistorySetup));

  SetupStore("AllowDelete",                 HistorySetup.allow_delete);
  SetupStore("ReplayHistorySize",           HistorySetup.replay_history_size);

  Setup.Save();
}

eOSState cHistorySetupMenu::ProcessKey(eKeys Key)
{
  eOSState state = cMenuSetupPage::ProcessKey(Key);
  return state;
}

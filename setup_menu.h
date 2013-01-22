/*
 * setup_menu.h: Setup Menu
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __HISTORY_SETUP_MENU_H
#define __HISTORY_SETUP_MENU_H

#include <vdr/menu.h>
#include <vdr/menuitems.h>
#include "config.h"

class cHistorySetupMenu : public cMenuSetupPage
{
private:
  cHistorySetup newconfig;

protected:
  virtual void Store(void);
  void Set(void);

public:
  cHistorySetupMenu(void);
  virtual eOSState ProcessKey(eKeys Key);
};

#endif //__HISTORY_SETUP_MENU_H

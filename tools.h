/*
 * tools.h: History plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __HISTORY_TOOLS_H_
#define __HISTORY_TOOLS_H_

#include <vdr/tools.h>

bool isimage(const char *text);
bool isvideo(const char *text);
bool ismusic(const char *text);

#endif // __HISTORY_TOOLS_H_


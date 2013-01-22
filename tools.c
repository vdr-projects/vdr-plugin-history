/*
 * tools.c: History plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include "tools.h"

//
// File type detection code adapted from RSS Reader plugin for VDR
// http://www.saunalahti.fi/~rahrenbe/vdr/rssreader/
// by Rolf Ahrenberg
//

bool isimage(const char *text)
{
  if (endswith(text, ".jpg") || endswith(text, ".gif") || endswith(text, ".png"))
     return true; 
  return false;
}

bool isvideo(const char *text)
{
  if (endswith(text, ".mpg") || endswith(text, ".avi") || endswith(text, ".ts") ||
      endswith(text, ".mkv") || endswith(text, ".flv") || endswith(text, ".wmv") ||
      endswith(text, ".mp4"))
     return true;
  return false;
}

bool ismusic(const char *text)
{
  if (endswith(text, ".mp3") || endswith(text, ".wav") || endswith(text, ".ogg") ||
      endswith(text, ".flac"))
     return true;
  return false;
}

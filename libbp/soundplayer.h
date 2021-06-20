#ifndef SOUNDPLAYER_H
#define SOUNDPLAYER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bstypes.h"
#include "channel.h"
#include "module.h"

class SoundPlayer
{
public:
  SoundPlayer(Module* ptr);
  ~SoundPlayer();

  void CleanUp();
  bool Initialize(short nrofvoices);
  bool Play(short voicenr, BYTE instrument, int frequency, int volume);
  bool Stop(short voicenr);
  bool SetPan(short voicenr, int val);
  bool SetFrequency(short voicenr, int frequency);
  bool SetVolume(short voicenr, int volume);
  bool Update(short voicenr, BYTE* memptr, int offset, int numofbytes);
  int  Mix(BYTE* buf);

private:
  Module*  modptr;
  short    nrofvoices;
  Channel* channels;
  BYTE*    playbuffer;
};

#endif

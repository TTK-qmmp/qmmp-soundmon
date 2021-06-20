#ifndef PLAYER_H
#define PLAYER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bstypes.h"

struct BPCURRENT
{
  BYTE note;
  BYTE instrument;
  WORD period;
  BYTE option;
  BYTE optiondata;
  BYTE autoslide;
  BYTE autoarpeggio;
  BYTE vibrato;
  BYTE volume;
  WORD adsrptr;
  WORD egptr;
  WORD modptr;
  WORD lfoptr;
  BYTE adsrcount;
  BYTE egcount;
  BYTE modcount;
  BYTE lfocount;
  BYTE fxcount;
  BYTE adsrcontrol;
  BYTE lfocontrol;
  BYTE egcontrol;
  BYTE modcontrol;
  BYTE fxcontrol;
  bool restart;
  bool newnote;
};

class Module;
class SoundPlayer;

class Player
{
public:
  Player(BYTE* ptr, size_t size);
  ~Player();

  bool Load();
  bool Run();

  int Render(BYTE* buf);

  int  GetLength() const;
  void Seek(int ms);

private:
  void SetStep(int step);
  WORD PerToFreq(WORD per);

private:
  void BpNext();
  void BpFx();
  void BpProcess();
  void BpArpeggio(short voice, BYTE arpeggio);
  void BpAutoSlide(short voice, BYTE slide);
  void BpVibrato(short voice);
  void BpSynthFx(short voice);
  void BpAveraging(short voice);
  void BpTransform(short voice, BYTE table, BYTE delta, bool invert);
  void BpBackInversion(short voice, BYTE delta);
  void BpTransform(short voice, BYTE table, BYTE delta);

  Module*      modptr;
  SoundPlayer* soundplayer;

  bool      loop;
  int       patptr;
  int       stepptr;
  int       nextstep;
  short     speed;
  short     count;
  short     arpptr;
  short     vibrptr;
  BPCURRENT bpcurrent[VOICES];
  BYTE      synthbuf[VOICES * SYNTHFXSIZE];

  static short bpper[256];
  static short vibtable[8];
};

#endif

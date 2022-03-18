#ifndef CHANNEL_H
#define CHANNEL_H

#include "bstypes.h"

class Channel
{
public:
  Channel();

  inline void SetFrequency(int frequency)
  {
    if(frequency<=0)
      frequency = 1;
    freqrepeat = double(SAMPLERATE)/double(frequency);
  }

  inline void SetLeftRight(bool left)
  {
    leftchannel = left;
  }

  inline void SetVolume(int volume)
  {
    this->volume = volume;
  }

  inline void StopPlay()
  {
    playing = false;
  }

  inline void StartPlay()
  {
    playing = true;
  }

  void SetPlayBuffer(BYTE* memptr, WORD size, WORD repeat, WORD replen);
  void GetNextBytes(BYTE& left, BYTE& right);
  void Update(BYTE* memptr, WORD offset, WORD numofbytes);

private:
  BYTE*  memptr;
  BYTE*  altmemptr;
  WORD   offset;
  WORD   numofbytes;
  WORD   size;
  WORD   repeat;
  WORD   replen;
  int    volume;
  bool   leftchannel;
  WORD   playcursor;
  bool   playing;
  double count;
  double freqrepeat;
};

#endif

#include "soundplayer.h"
#include "string.h"

SoundPlayer::SoundPlayer(Module* ptr)
{
  modptr     = ptr;
  channels   = NULL;
  nrofvoices = 0;
  playbuffer = NULL;
}

SoundPlayer::~SoundPlayer()
{
  CleanUp();
}

void SoundPlayer::CleanUp()
{
  delete[] channels;
  delete[] playbuffer;
}

bool SoundPlayer::Initialize(short nrofvoices)
{
  if(modptr==NULL)
    return false;
  this->nrofvoices = nrofvoices;
  if((channels = new Channel[nrofvoices])==NULL)
    return false;
  if((playbuffer = new BYTE[BUFFERSIZE*2])==NULL)
    return false;
  return true;
}

bool SoundPlayer::Play(short voicenr, BYTE instrument, int frequency, int volume)
{
  WORD  size, repeat, replen, dummy;
  BYTE* sampptr;

  if(instrument<1 || instrument>15 || voicenr>=nrofvoices || voicenr<0)
    return false;
  if(volume>64)
    volume = 64;
  if(modptr->IsSynthInstrument(instrument))
  {
    modptr->GetSynthInstrument(instrument, size, sampptr);
    replen = size;
    repeat = 0;
  }
  else
  {
    modptr->GetSampledInstrument(instrument, size, repeat, replen, dummy, sampptr);
  }
  channels[voicenr].SetPlayBuffer(sampptr, size, repeat, replen);
  channels[voicenr].SetVolume(volume);
  channels[voicenr].SetFrequency(frequency);
  channels[voicenr].StartPlay();
  return true;
}

bool SoundPlayer::Stop(short voicenr)
{
  if(voicenr<0 || voicenr>=nrofvoices)
    return false;
  channels[voicenr].StopPlay();
  return true;
}

bool SoundPlayer::SetPan(short voicenr, int val)
{
  if(voicenr<0 || voicenr>=nrofvoices)
    return false;
  if(val<0)
    channels[voicenr].SetLeftRight(true);
  else
    channels[voicenr].SetLeftRight(false);
  return true;
}

bool SoundPlayer::SetFrequency(short voicenr, int frequency)
{
  if(voicenr<0 || voicenr>=nrofvoices)
    return false;
  channels[voicenr].SetFrequency(frequency);
  return true;
}

bool SoundPlayer::SetVolume(short voicenr, int volume)
{
  if(voicenr<0 || voicenr>=nrofvoices)
    return false;
  if(volume>64)
    volume = 64;
  channels[voicenr].SetVolume(volume);
  return true;
}

bool SoundPlayer::Update(short voicenr, BYTE* memptr, int offset, int numofbytes)
{
  if(voicenr<0 || voicenr>=nrofvoices)
    return false;
  channels[voicenr].Update(memptr, offset, numofbytes);
  return true;
}

int SoundPlayer::Mix(BYTE* buf)
{
  int   left = 0, right = 0;
  BYTE  lefttmp, righttmp;
  BYTE* ptr = playbuffer;

  for(int j = 0; j<BUFFERSIZE; j++)
  {
    for(short i = 0; i<nrofvoices; i++)
    {
      channels[i].GetNextBytes(lefttmp, righttmp);
      left  += (lefttmp-128);
      right += (righttmp-128);
    }
    left     = left/nrofvoices;
    right    = right/nrofvoices;
    *(ptr++) = left+128;
    *(ptr++) = right+128;
  }

  memcpy(buf, playbuffer, BUFFERSIZE * 2);
  return BUFFERSIZE * 2;
}

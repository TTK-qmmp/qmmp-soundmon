#include "player.h"
#include "module.h"
#include "soundplayer.h"
#include <memory.h>

short Player::bpper[] = { 6848, 6464, 6080, 5760, 5440, 5120, 4832, 4576, 4320, 4064, 3840, 3616,
                          3424, 3232, 3040, 2880, 2720, 2560, 2416, 2288, 2160, 2032, 1920, 1808,
                          1712, 1616, 1520, 1440, 1360, 1280, 1208, 1144, 1080, 1016, 960, 904,
                          856, 808, 760, 720, 680, 640, 604, 572, 540, 508, 480, 452,
                          428, 404, 380, 360, 340, 320, 302, 286, 270, 254, 240, 226,
                          214, 202, 190, 180, 170, 160, 151, 143, 135, 127, 120, 113,
                          107, 101, 95, 90, 85, 80, 76, 72, 68, 64, 60, 57 };
short Player::vibtable[] = { 0, 64, 128, 64, 0, -64, -128, -64 };

Player::Player(BYTE* ptr, size_t size)
{
  modptr      = new Module(ptr, size);
  soundplayer = new SoundPlayer(modptr);
  patptr      = 0;
  speed       = 6;
  count       = 0;
  stepptr     = 0;
  arpptr      = 0;
  vibrptr     = 0;
  nextstep    = -1;
  loop        = false;
  memset(bpcurrent, 0, sizeof(BPCURRENT)*VOICES);
}

Player::~Player()
{
  delete modptr;
  delete soundplayer;
}

bool Player::Load()
{
  if(!modptr->Init())
    return false;
  if(!soundplayer->Initialize(VOICES))
    return false;
  soundplayer->SetPan(0, DSBPAN_LEFT);
  soundplayer->SetPan(1, DSBPAN_RIGHT);
  soundplayer->SetPan(2, DSBPAN_RIGHT);
  soundplayer->SetPan(3, DSBPAN_LEFT);
  return true;
}

void Player::SetStep(int step)
{
  if(step>=modptr->GetNumSteps() || step<0)
    return;
  stepptr = step;
}

WORD Player::PerToFreq(WORD per)
{
  if(per!=0)
    return PERTOFREQCONST/per;
  else
    return 0;
}

int Player::Render(BYTE* buf)
{
  return soundplayer->Mix(buf);
}

int Player::GetLength() const
{
  int max = speed;
  for(int voice = 0; voice<VOICES; voice++)
  {
    WORD pattern = modptr->GetPattern(stepptr, voice);
    BYTE byte    = modptr->GetFXByte(pattern, patptr);
    if(max < byte)
    {
      max = byte;
    }
  }
  return modptr->GetNumSteps() * STEPSIZE * max / 50 * 1000;
}

void Player::Seek(int ms)
{
  const double rate = ms * 1.0 / GetLength();
  SetStep(rate * modptr->GetNumSteps());
}

bool Player::Run()
{
  BpFx();
  count++;
  if(count>=speed)
  {
    BpNext();
    BpProcess();
    count = 0;
    patptr++;
    if(patptr==NOTESPERPATTERN)
    {
      patptr = 0;
      if(nextstep!=-1)
      {
        stepptr  = nextstep;
        nextstep = -1;
      }
      else
        stepptr++;

      if(stepptr==modptr->GetNumSteps())
      {
        if(loop)
          stepptr = 0;
        else
          return false;
      }
    }
  }
  return true;
}

void Player::BpFx()
{
  arpptr++;
  if(arpptr==3)
    arpptr = 0;
  vibrptr = (vibrptr+1)&7;

  for(short voice = 0; voice<VOICES; voice++)
  {
    if(modptr->IsSynthInstrument(bpcurrent[voice].instrument))
      BpSynthFx(voice);
    if(bpcurrent[voice].vibrato!=0)
      BpVibrato(voice);
    if(bpcurrent[voice].autoarpeggio!=0)
      BpArpeggio(voice, bpcurrent[voice].autoarpeggio);
    else if(bpcurrent[voice].option==0 && bpcurrent[voice].optiondata!=0)
      BpArpeggio(voice, bpcurrent[voice].optiondata);
    if(bpcurrent[voice].autoslide!=0)
      BpAutoSlide(voice, bpcurrent[voice].autoslide);
  }
}

void Player::BpNext()
{
  WORD pattern;
  BYTE note;
  BYTE option;
  BYTE optiondata;
  BYTE instrument;
  WORD period;
  BYTE transpose;
  BYTE soundtranspose;

  for(short voice = 0; voice<VOICES; voice++)
  {
    pattern        = modptr->GetPattern(stepptr, voice);
    transpose      = modptr->GetTR(stepptr, voice);
    soundtranspose = modptr->GetST(stepptr, voice);
    note           = modptr->GetNote(pattern, patptr);
    option         = modptr->GetFX(pattern, patptr);
    optiondata     = modptr->GetFXByte(pattern, patptr);
    instrument     = modptr->GetInstrument(pattern, patptr);

    if(transpose!=0 && note!=0 && (option!=0xA || (optiondata&0xF0)==0))
      note += transpose;
    if(soundtranspose!=0 && instrument!=0 && (option!=0xA || (optiondata&0x0F)==0))
      instrument += soundtranspose;
    if(note!=0)
      period = GETFREQ(note);
    bpcurrent[voice].restart = false;
    if(note!=0)
    {
      if(instrument==0)
        instrument = bpcurrent[voice].instrument;
      bpcurrent[voice].autoslide    = 0;
      bpcurrent[voice].autoarpeggio = 0;
      bpcurrent[voice].vibrato      = 0;
      if(option!=0xF && option!=0xE && option!=0xD)
      {
        bpcurrent[voice].restart = true;
        soundplayer->Stop(voice);
        bpcurrent[voice].modptr    = 0;
        bpcurrent[voice].adsrptr   = 0;
        bpcurrent[voice].lfoptr    = 0;
        bpcurrent[voice].egptr     = 0;
        bpcurrent[voice].adsrcount = 1;
      }
// if(option==0xE || option==0xD)
// {
// bpcurrent[voice].adsrptr=0;
// bpcurrent[voice].adsrcount=1;
// }
    }
    bpcurrent[voice].newnote = false;
    if(note!=0)
    {
      bpcurrent[voice].newnote    = true;
      bpcurrent[voice].note       = note;
      bpcurrent[voice].period     = period;
      bpcurrent[voice].instrument = instrument;
      bpcurrent[voice].volume     = modptr->GetInstrumentVolume(instrument);
    }
    bpcurrent[voice].option     = option;
    bpcurrent[voice].optiondata = optiondata;
    switch(bpcurrent[voice].option)
    {
    case 0x1:
      bpcurrent[voice].volume = optiondata;
      soundplayer->SetVolume(voice, optiondata);
      break;
    case 0x2:
      speed = optiondata;
      break;
    case 0x4:
      bpcurrent[voice].period -= optiondata;
      soundplayer->SetFrequency(voice, PerToFreq(bpcurrent[voice].period));
      break;
    case 0x5:
      bpcurrent[voice].period += optiondata;
      soundplayer->SetFrequency(voice, PerToFreq(bpcurrent[voice].period));
      break;
    case 0x6:
      bpcurrent[voice].vibrato      = optiondata;
      bpcurrent[voice].autoslide    = 0;
      bpcurrent[voice].autoarpeggio = 0;
      break;
    case 0x9:
      bpcurrent[voice].vibrato      = 0;
      bpcurrent[voice].autoslide    = 0;
      bpcurrent[voice].autoarpeggio = optiondata;
      break;
    case 0x8:
      bpcurrent[voice].autoslide    = optiondata;
      bpcurrent[voice].autoarpeggio = 0;
      bpcurrent[voice].vibrato      = 0;
      break;
    case 0x7:
      nextstep = optiondata;
      break;
    case 0xB:
      bpcurrent[voice].fxcontrol = optiondata;
      break;
    case 0xD:
      bpcurrent[voice].fxcontrol = bpcurrent[voice].fxcontrol^1;
// bpcurrent[voice].autoarpeggio=optiondata;
// break;
    case 0xE:
      bpcurrent[voice].adsrptr   = 0;
      bpcurrent[voice].adsrcount = 1;
      if(bpcurrent[voice].adsrcontrol==0)
        bpcurrent[voice].adsrcontrol = 1;
    case 0xF:
      bpcurrent[voice].autoarpeggio = optiondata;
      break;
    default:
      break;
    }
// if(bpcurrent[voice].volume>64)
// bpcurrent[voice].volume=64;
  }
}

void Player::BpProcess()
{
  BYTE  volume;
  WORD  frequency = 0;
  WORD  size, adsrsize;
  BYTE* memptr;
  BYTE  egdelay, lfodelay, fxdelay, moddelay;
  BYTE  adsrtable, adsrspeed;

  for(short voice = 0; voice<VOICES; voice++)
  {
    if(bpcurrent[voice].restart)
    {
      if(modptr->IsSynthInstrument(bpcurrent[voice].instrument))
      {
        if(modptr->GetSynthInstrument(bpcurrent[voice].instrument, size, memptr))
        {
          // Make working copy of synth buffer
          memcpy(synthbuf+voice*SYNTHFXSIZE, memptr, SYNTHFXSIZE);
          // Update synth sound in directsound buffer
          soundplayer->Update(voice, memptr, 0, SYNTHFXSIZE);
          modptr->GetSynthDelays(bpcurrent[voice].instrument, egdelay, lfodelay, fxdelay, moddelay);
          modptr->GetSynthControls(bpcurrent[voice].instrument,
                                   bpcurrent[voice].adsrcontrol, bpcurrent[voice].lfocontrol,
                                   bpcurrent[voice].egcontrol, bpcurrent[voice].fxcontrol,
                                   bpcurrent[voice].modcontrol);

          if(bpcurrent[voice].option!=0xE && bpcurrent[voice].option!=0xF)
          {
            bpcurrent[voice].egcount  = egdelay+1;
            bpcurrent[voice].modcount = moddelay+1;
            bpcurrent[voice].lfocount = lfodelay+1;
            bpcurrent[voice].fxcount  = fxdelay+1;
          }
          modptr->GetSynthADSRData(bpcurrent[voice].instrument,
                                   adsrtable, adsrsize, adsrspeed);
          if(bpcurrent[voice].option!=0xF)
            bpcurrent[voice].adsrcount = adsrspeed;
          if(bpcurrent[voice].adsrcontrol!=0)
            volume = CALCVOLUME(bpcurrent[voice].volume, modptr->GetTableValue(adsrtable, 0));
          else
            volume = bpcurrent[voice].volume;
        }
      }
      else
        volume = bpcurrent[voice].volume;
      if(bpcurrent[voice].period!=0)
        frequency = PerToFreq(bpcurrent[voice].period);
      soundplayer->Play(voice, bpcurrent[voice].instrument, frequency, volume);
    }
    if(bpcurrent[voice].period!=0 && bpcurrent[voice].newnote)
    {
      soundplayer->SetFrequency(voice, PerToFreq(bpcurrent[voice].period));
      bpcurrent[voice].newnote = false;
    }
  }
}

void Player::BpArpeggio(short voice, BYTE arpeggio)
{
  BYTE note = 0;

  if(arpptr==1)
    note = (arpeggio&0x0F);
  else if(arpptr==2)
    note = (arpeggio&0xF0)>>4;
  note                   += bpcurrent[voice].note;
  bpcurrent[voice].period = GETFREQ(note);
  soundplayer->SetFrequency(voice, PerToFreq(bpcurrent[voice].period));
}

void Player::BpAutoSlide(short voice, BYTE slide)
{
  bpcurrent[voice].period += (short)((char)slide);
  soundplayer->SetFrequency(voice, PerToFreq(bpcurrent[voice].period));
}

void Player::BpVibrato(short voice)
{
  BYTE note = bpcurrent[voice].note;
  WORD period;

  if(note==0)
    return;
  period = bpcurrent[voice].period+vibtable[vibrptr]/bpcurrent[voice].vibrato;
  soundplayer->SetFrequency(voice, PerToFreq(period));
}

void Player::BpSynthFx(short voice)
{
  bool updatesound = false;
  WORD tmpsize;
  BYTE tmptable, tmpdepth, tmpspeed;
  BYTE tmp;
  WORD period;

  // Start with ADSR processing, the easy one.....
  if(bpcurrent[voice].adsrcontrol!=0 &&
     modptr->GetSynthADSRData(bpcurrent[voice].instrument, tmptable, tmpsize, tmpspeed))
  {
    bpcurrent[voice].adsrcount--;
    if(bpcurrent[voice].adsrcount==0)
    {
      bpcurrent[voice].adsrcount = tmpspeed;
      tmp                        = modptr->GetTableValue(tmptable, bpcurrent[voice].adsrptr);
      soundplayer->SetVolume(voice, CALCVOLUME(bpcurrent[voice].volume, tmp));
      bpcurrent[voice].adsrptr++;
      if(bpcurrent[voice].adsrptr==tmpsize)
      {
        bpcurrent[voice].adsrptr = 0;
        if(bpcurrent[voice].adsrcontrol==1)
          bpcurrent[voice].adsrcontrol = 0;
      }
    }
  }

  // Second in line... LFO processing
  if(bpcurrent[voice].lfocontrol!=0 &&
     modptr->GetSynthLFOData(bpcurrent[voice].instrument, tmptable, tmpsize, tmpspeed, tmpdepth))
  {
    bpcurrent[voice].lfocount--;
    if(bpcurrent[voice].lfocount==0)
    {
      bpcurrent[voice].lfocount = tmpspeed;
      if(tmpdepth==0)
        tmpdepth = 1;
      period = bpcurrent[voice].period+
               (short(modptr->GetTableValue(tmptable, bpcurrent[voice].lfoptr))-128)/tmpdepth;
      soundplayer->SetFrequency(voice, PerToFreq(period));
      bpcurrent[voice].lfoptr++;
      if(bpcurrent[voice].lfoptr==tmpsize)
      {
        bpcurrent[voice].lfoptr = 0;
        if(bpcurrent[voice].lfocontrol==1)
          bpcurrent[voice].lfocontrol = 0;
      }
    }
  }

  // EG
  if(bpcurrent[voice].egcontrol!=0 &&
     modptr->GetSynthEGData(bpcurrent[voice].instrument, tmptable, tmpsize, tmpspeed))
  {
    bpcurrent[voice].egcount--;
    if(bpcurrent[voice].egcount==0)
    {
      BYTE* ptr = synthbuf+voice*SYNTHFXSIZE;
      BYTE* ttable;
      WORD  size;

      bpcurrent[voice].egcount = tmpspeed;
      tmp                      = modptr->GetTableValue(tmptable, bpcurrent[voice].egptr);
      modptr->GetSynthInstrument(bpcurrent[voice].instrument, size, ttable);
      memcpy(ptr, ttable, SYNTHFXSIZE);                                 // restore original data
      ttable = ptr;
      for(short i = 0; i<(tmp>>3); i++)
      {
        (*ptr) = 255-(*ptr);
        ptr++;
      }
      updatesound = true;
      bpcurrent[voice].egptr++;
      if(bpcurrent[voice].egptr==tmpsize)
      {
        bpcurrent[voice].egptr = 0;
        if(bpcurrent[voice].egcontrol==1)
          bpcurrent[voice].egcontrol = 0;
      }
    }
  }

  // FX
  if(bpcurrent[voice].fxcontrol!=0 && modptr->GetSynthFXData(bpcurrent[voice].instrument, tmptable, tmpspeed))
  {
    bpcurrent[voice].fxcount--;
    if(bpcurrent[voice].fxcount==0)
    {
      bpcurrent[voice].fxcount = 1;
      updatesound              = true;
      switch(bpcurrent[voice].fxcontrol)
      {
      case 1:
        BpAveraging(voice);
        bpcurrent[voice].fxcount = tmpspeed;
        break;
      case 2:
        BpTransform(voice, tmptable, tmpspeed, true);
        break;
      case 3:
        BpTransform(voice, tmptable, tmpspeed, false);
        break;
      case 4:
        BpTransform(voice, tmptable+1, tmpspeed, false);
        break;
      case 5:
        BpTransform(voice, tmptable, tmpspeed, false);
        break;
      default:
        break;
      }
    }
  }

  // MOD
  if(bpcurrent[voice].modcontrol!=0 &&
     modptr->GetSynthMODData(bpcurrent[voice].instrument, tmptable, tmpsize, tmpspeed))
  {
    bpcurrent[voice].modcount--;
    if(bpcurrent[voice].modcount==0)
    {
      bpcurrent[voice].modcount = tmpspeed;
      BYTE* ptr = synthbuf+voice*SYNTHFXSIZE;
      *(ptr+SYNTHFXSIZE-1) = modptr->GetTableValue(tmptable, bpcurrent[voice].modptr);
      updatesound          = true;
      bpcurrent[voice].modptr++;
      if(bpcurrent[voice].modptr==tmpsize)
      {
        bpcurrent[voice].modptr = 0;
        if(bpcurrent[voice].modcontrol==1)
          bpcurrent[voice].modcontrol = 0;
      }
    }
  }
  BYTE* ptr = synthbuf+voice*SYNTHFXSIZE;
  if(updatesound)
    soundplayer->Update(voice, ptr, 0, SYNTHFXSIZE);
}

void Player::BpAveraging(short voice)
{
  BYTE* ptr     = synthbuf+voice*SYNTHFXSIZE;
  BYTE  lastval = *ptr;

  for(short i = 1; i<SYNTHFXSIZE-1; i++)
  {
    ptr++;
    lastval = (WORD(lastval)+WORD(*(ptr+1)))>>1;
    *(ptr)  = lastval;
  }
}

void Player::BpTransform(short voice, BYTE table, BYTE delta, bool invert)
{
  BYTE* ptr = synthbuf+voice*SYNTHFXSIZE;
  BYTE  tmp;

  for(short i = 0; i<SYNTHFXSIZE; i++)
  {
    tmp = modptr->GetTableValue(table, i);
    if(invert)
      tmp = 255-tmp;
    if((*ptr)>=tmp)
      (*ptr) -= delta;
    else if((*ptr)<=tmp)
      (*ptr) += delta;
    ptr++;
  }
}

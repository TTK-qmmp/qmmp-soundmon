//////////////////////////////////////////////////////////////////////
// 
// File:        bstypes.h
// Author:      Brian Postma
// Created:     30 December 1999
//
// Purpose:     All types and defines for the replayer.
//
//////////////////////////////////////////////////////////////////////

#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

#include <stdio.h>
#include <unistd.h>

// Platform independent types
typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;

// mixer settings
#define FRAGMENT    0x0008000c   // 4096 bytes per fragment, 8 fragments
#define FORMAT      AFMT_U8
#define SAMPLERATE  44100

// Soundmon modstruct settings
#define SONGDATASIZE	32
#define INSTRSIZE		32
#define NUMINSTR		15
#define HEADERSIZE		(NUMINSTR * INSTRSIZE) + SONGDATASIZE
#define VOICES			4
#define NOTESPERPATTERN	16
#define NOTESIZE		3
#define PATTERNSIZE		NOTESPERPATTERN * NOTESIZE
#define STEPVOICESIZE	4
#define STEPSIZE		VOICES * STEPVOICESIZE
#define TABLESIZE		64
#define SYNTHFXSIZE		32

#define PERTOFREQCONST  8192 * 428

#define DSBPAN_LEFT -1
#define DSBPAN_RIGHT 1

#define CALCVOLUME(x, y) BYTE((WORD(x) * WORD(y)) >> 8)
#define GETFREQ(x) bpper[char(x) + 36 - 1]

#define BUFFERSIZE SAMPLERATE / 50

#endif

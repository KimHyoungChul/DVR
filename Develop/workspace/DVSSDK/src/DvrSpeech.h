#pragma once
#ifndef _DVRSPEECH_H_
#define _DVRSPEECH_H_

#include "SpeechSocket.h"

enum AudioCodecID
{
	AUDIO_CODEC_OGG = 1,	
	AUDIO_CODEC_ADPCM = 2,
	AUDIO_CODEC_G711 = 11,
	AUDIO_CODEC_G721 = 21,
	AUDIO_CODEC_G723 = 23,
	AUDIO_CODEC_G726 = 26,
	AUDIO_CODEC_G729 = 29,
};

class DvrSpeech
{
public:
	DvrSpeech();
	~DvrSpeech();

	S32 Create(const S8 *ip, UNS16 port, WAVEFORMATEX format, S8 *ticket, S32 ticketlen);
	S32 Destroy();
	S32 SpeechStart();
	S32 SpeechStop();
	bool IsSpeech() { return !m_bEndSpeech;}

	SpeechFifo	*m_pFifo;
	MyThread	m_hSpeech;	

	SpeechSocket m_DataSocket;	
	WAVEFORMATEX m_AudioFormat;

	bool		m_bExit;
	bool		m_bEndSpeech;

private:
	MyThread	m_hPlayer;
};

#endif

#pragma once

#include <sndfile.h>
#include <AlternativeAudio\AudioSource\IAudioSource.h>

namespace AlternativeAudio_Libsndfile {
	/*
	Audio source that utilizes libsndfile to load an audio file completely into memory.
	*/
	class AudioSource_Libsnd_Memory : public AlternativeAudio::IAudioSourceLib {
	public:
		AudioSource_Libsnd_Memory(const char * filename);
		~AudioSource_Libsnd_Memory();
	public:
		bool Seek(long long position);
		long long GetFrames(long long framesToRead, float* buff);
		bool GetFrame(float* frame);
		double GetSampleRate() { return this->m_samplerate; }
		AlternativeAudio::AudioSourceTime GetLength() { return this->m_timeLength; }
		const AlternativeAudio::AudioFrame::Type GetFrameType();
		long long GetFrameLength() { return this->m_numFrames; }
	private:
		long long m_pos;
		long long  m_numFrames;
		double m_samplerate;
		int m_numChannels;
		AlternativeAudio::AudioFrame::Type m_format;
		AlternativeAudio::AudioSourceTime m_timeLength;
		float * m_buff;
		int m_flags;
	};
}
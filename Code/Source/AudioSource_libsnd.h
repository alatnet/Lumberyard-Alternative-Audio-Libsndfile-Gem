#pragma once

#include <sndfile.h>
#include <AlternativeAudio\AudioSource\IAudioSource.h>

namespace AlternativeAudio_Libsndfile{
	/*
	Audio source that utilizes libsndfile to stream audio files.
	*/
	class AudioSource_Libsnd : public AlternativeAudio::IAudioSourceLib {
	public:
		AudioSource_Libsnd(const char * filename);
		~AudioSource_Libsnd();
	public:
		bool Seek(long long position);
		long long GetFrames(long long framesToRead, float* buff);
		bool GetFrame(float* frame);
		double GetSampleRate() { return this->sfInfo.samplerate; }
		AlternativeAudio::AudioSourceTime GetLength() { return this->m_timeLength; }
		const AlternativeAudio::AudioFrame::Type GetFrameType();
		long long GetFrameLength() { return this->sfInfo.frames; }
	private:
		SNDFILE *sndFile;
		SF_INFO sfInfo;
		AlternativeAudio::AudioFrame::Type m_format;
		AlternativeAudio::AudioSourceTime m_timeLength;
		int m_flags;
	private:
		SF_VIRTUAL_IO vio;
		AZ::IO::HandleType handle;
	};
}
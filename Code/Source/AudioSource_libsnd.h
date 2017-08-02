#pragma once

#include <sndfile.h>
#include <AlternativeAudio\IAudioSource.h>

namespace AlternativeAudio_Libsndfile{
	/*
	Audio source that utilizes libsndfile to stream audio files.
	*/
	class AudioSource_Libsnd : public AlternativeAudio::IAudioSource {
	public:
		AudioSource_Libsnd(const char * filename);
		~AudioSource_Libsnd();
	public:
		static std::string GetVersion();
	public:
		bool Seek(long long position);
		long long GetFrames(long long framesToRead, float* buff);
		bool GetFrame(float* frame);
		double GetSampleRate() { return this->sfInfo.samplerate; }
		AlternativeAudio::AudioSourceTime GetLength() { return this->m_timeLength; }
		long long GetFrameLength() { return this->sfInfo.frames; }
		const AlternativeAudio::AudioFrame::Type GetFrameType();
	private:
		SNDFILE *sndFile;
		SF_INFO sfInfo;
		AlternativeAudio::AudioSourceTime m_timeLength;
		int m_flags;
	};
}
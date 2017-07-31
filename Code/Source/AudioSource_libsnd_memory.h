#pragma once

#include <sndfile.h>
#include <AlternativeAudio\IAudioSource.h>

namespace AlternativeAudio_Libsndfile {
	/*
	Audio source that utilizes libsndfile to load an audio file completely into memory.
	*/
	class AudioSource_Libsnd_Memory : public AlternativeAudio::IAudioSource {
	public:
		AudioSource_Libsnd_Memory(const char * filename);
		~AudioSource_Libsnd_Memory();
	public:
		static std::string GetVersion();
	public:
		bool Seek(long long position);
		long long GetFrames(long long framesToRead, float* buff);
		bool GetFrame(float* frame);
		double GetSampleRate() { return this->m_samplerate; }
		const AZ::Uuid& GetFrameType();
		AlternativeAudio::AudioSourceTime GetLength() { return this->m_timeLength; }
		long long GetFrameLength() { return this->m_numFrames; }
	private:
		long long m_pos;
		long long  m_numFrames;
		double m_samplerate;
		int m_numChannels;
		AlternativeAudio::AudioSourceTime m_timeLength;
		float * m_buff;
		int m_flags;
	private:
		bool m_hasError;
		AZStd::vector<AlternativeAudio::AudioSourceError> m_errors;
	};
}
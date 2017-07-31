#include <StdAfx.h>
#include "AudioSource_libsnd.h"

namespace AlternativeAudio_Libsndfile {
	AudioSource_Libsnd::AudioSource_Libsnd(const char * filename) : AlternativeAudio::IAudioSource() {
		AZ_Printf("AudioSource_libsnd", "[AudioSource_libsnd] Loading File : %s", filename);
		this->sndFile = nullptr;
		this->sndFile = sf_open(filename, SFM_READ, &this->sfInfo);

		if (!this->sndFile) {
			int err = sf_error(this->sndFile);
			pushError(err, sf_error_number(err));
			this->sndFile = nullptr;
			return;
		}

		int err = sf_error(this->sndFile);
		if (err != SF_ERR_NO_ERROR) {
			int err = sf_error(this->sndFile);
			pushError(err, sf_error_number(err));
			sf_close(this->sndFile); //close the libsndfile file.
			this->sndFile = nullptr;
			return;
		}
		
		AZ_Printf("AudioSource_libsnd", "[AudioSource_libsnd] Channels: %i", this->sfInfo.channels);
		AZ_Printf("AudioSource_libsnd", "[AudioSource_libsnd] Sample rate: %i", this->sfInfo.samplerate);
		AZ_Printf("AudioSource_libsnd", "[AudioSource_libsnd] Seekable: %i", this->sfInfo.seekable);

		this->m_timeLength.totalSec = (1.0 * this->sfInfo.frames) / this->sfInfo.samplerate;
		this->m_timeLength.hrs = (int)(this->m_timeLength.totalSec / 3600.0);
		this->m_timeLength.minutes = (int)((this->m_timeLength.totalSec - (this->m_timeLength.hrs * 3600.0)) / 60.0);
		this->m_timeLength.sec = this->m_timeLength.totalSec - (this->m_timeLength.hrs * 3600.0) - (this->m_timeLength.minutes * 60.0);

		AZ_Printf("AudioSource_libsnd", "[AudioSource_libsnd] File Length: %i:%i:%f - %f", this->m_timeLength.hrs, this->m_timeLength.minutes, this->m_timeLength.sec, this->m_timeLength.totalSec);
	}
	
	AudioSource_Libsnd::~AudioSource_Libsnd(){
		if (this->sndFile != NULL || this->sndFile != nullptr) {
			sf_close(this->sndFile); //close the libsndfile file.
			this->sndFile = nullptr;
		}
	}

	std::string AudioSource_Libsnd::GetVersion() {
		std::string ret = "[AudioSource_Libsnd] AudioSource - libsndfile version: ";
		ret += sf_version_string();
		return ret;
	}
	
	bool AudioSource_Libsnd::Seek(long long position){
		if (!this->m_hasError) {
			if (sf_seek(this->sndFile, position, SF_SEEK_SET) == -1) {
				int err = sf_error(this->sndFile);
				pushError(err, sf_error_number(err));
				return false;
			}
			return true;
		}
		return false;
	}
	
	long long AudioSource_Libsnd::GetFrames(long long framesToRead, float* buff){
		if (this->m_hasError) return 0;
		return sf_readf_float(this->sndFile, buff, framesToRead);
	}

	bool AudioSource_Libsnd::GetFrame(float* frame) {
		return sf_readf_float(this->sndFile, frame, this->sfInfo.channels) == 1;
	}

	const AZ::Uuid& AudioSource_Libsnd::GetFrameType() {
		switch (this->sfInfo.channels) {
		case 1:
			return AlternativeAudio::AudioFrame::af1::RTTI_Type();
		case 2:
			return AlternativeAudio::AudioFrame::af2::RTTI_Type();
		case 3:
			return AlternativeAudio::AudioFrame::af21::RTTI_Type();
		case 4:
			return AlternativeAudio::AudioFrame::af31::RTTI_Type();
		case 5:
			return AlternativeAudio::AudioFrame::af5::RTTI_Type();
		case 6:
			return AlternativeAudio::AudioFrame::af51::RTTI_Type();
		case 7:
			return AlternativeAudio::AudioFrame::af7::RTTI_Type();
		case 8:
			return AlternativeAudio::AudioFrame::af71::RTTI_Type();
		}

		return AlternativeAudio::AudioFrame::af1::RTTI_Type();
	}
}

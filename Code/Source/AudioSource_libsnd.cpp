#include <StdAfx.h>
#include "AudioSource_libsnd.h"

#include "AudioSource_libsnd_virtualio.h"

namespace AlternativeAudio_Libsndfile {
	//todo, use virutal io and gEnv->pFileIO instead of sf_open

	AudioSource_Libsnd::AudioSource_Libsnd(const char * filename) : AlternativeAudio::IAudioSourceLib() {
		AZ_Printf("AudioSource_libsnd", "[AudioSource_libsnd] Loading File : %s", filename);
		this->sndFile = nullptr;
		//this->sndFile = sf_open(filename, SFM_READ, &this->sfInfo);

		//resolve the path
		if (gEnv) {
			/*char * resolvedPath = new char[AZ_MAX_PATH_LEN];
			gEnv->pFileIO->ResolvePath(filename, resolvedPath, AZ_MAX_PATH_LEN);
			AZ_Printf("AudioSource_libsnd", "Resolved Audio Path - %s", resolvedPath);
			this->sndFile = sf_open(resolvedPath, SFM_READ, &sfInfo);
			delete resolvedPath;*/

			this->vio.get_filelen = VIO::get_filelen;
			this->vio.read = VIO::read;
			this->vio.seek = VIO::seek;
			this->vio.tell = VIO::tell;
			this->vio.write = VIO::write;

			this->handle = AZ::IO::InvalidHandle;

			if (gEnv->pFileIO->Open(filename, AZ::IO::OpenMode::ModeRead | AZ::IO::OpenMode::ModeBinary, this->handle) != AZ::IO::ResultCode::Success) {
				pushError(-1, "Cannot open file.");
				return;
			}

			if (this->handle != AZ::IO::InvalidHandle) this->sndFile = sf_open_virtual(&(this->vio), SFM_READ, &(this->sfInfo), (void*)(this->handle));
			else {
				pushError(-2, "Invalid File Handle.");
				return;
			}
		} else
			this->sndFile = sf_open(filename, SFM_READ, &sfInfo);

		if (!this->sndFile) {
			int err = sf_error(this->sndFile);
			pushError(err, sf_error_number(err));
			this->sndFile = nullptr;
			if (gEnv && handle != AZ::IO::InvalidHandle) gEnv->pFileIO->Close(handle);
			return;
		}

		int err = sf_error(this->sndFile);
		if (err != SF_ERR_NO_ERROR) {
			int err = sf_error(this->sndFile);
			pushError(err, sf_error_number(err));
			sf_close(this->sndFile); //close the libsndfile file.
			this->sndFile = nullptr;
			if (gEnv && handle != AZ::IO::InvalidHandle) gEnv->pFileIO->Close(handle);
			return;
		}
		
		this->m_format = this->GetFrameType();

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

		if (gEnv && this->handle != AZ::IO::InvalidHandle) gEnv->pFileIO->Close(this->handle);
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

		//long long ret = sf_readf_float(this->sndFile, buff, framesToRead);

		//process per source dsp effect
		//this->ProcessEffects(this->m_format, buff, ret, this);

		//return ret;
	}

	bool AudioSource_Libsnd::GetFrame(float* frame) {
		if (this->m_hasError) return false;

		return sf_readf_float(this->sndFile, frame, 1) == 1;

		//bool ret = sf_readf_float(this->sndFile, frame, 1) == 1;
		//bool ret = sf_read_float(this->sndFile, frame, this->sfInfo.channels) == 1;

		//process per source dsp effect
		//if (ret) this->ProcessEffects(this->m_format, frame, 1, this);

		//return ret;
	}

	const AlternativeAudio::AudioFrame::Type AudioSource_Libsnd::GetFrameType() {
		switch (this->sfInfo.channels) {
		case 1:
			return AlternativeAudio::AudioFrame::Type::eT_af1;
		case 2:
			return AlternativeAudio::AudioFrame::Type::eT_af2;
		case 3:
			return AlternativeAudio::AudioFrame::Type::eT_af21;
		case 4:
			return AlternativeAudio::AudioFrame::Type::eT_af31;
		case 5:
			return AlternativeAudio::AudioFrame::Type::eT_af5;
		case 6:
			return AlternativeAudio::AudioFrame::Type::eT_af51;
		case 7:
			return AlternativeAudio::AudioFrame::Type::eT_af7;
		case 8:
			return AlternativeAudio::AudioFrame::Type::eT_af71;
		}

		return AlternativeAudio::AudioFrame::Type::eT_af1;
	}
}

#include <StdAfx.h>
#include "AudioSource_libsnd_memory.h"
#include <AlternativeAudio\AlternativeAudioBus.h>

#include <AzCore\IO\SystemFile.h>

#include "AudioSource_libsnd_virtualio.h"

namespace AlternativeAudio_Libsndfile {
	AudioSource_Libsnd_Memory::AudioSource_Libsnd_Memory(const char * filename) : AlternativeAudio::IAudioSourceLib() {
		AZ_Printf("AudioSource_libsnd_memory", "[AudioSource_libsnd_memory] Loading File: %s", filename);
		
		//setup
		this->m_pos = this->m_numFrames = this->m_numChannels = 0;
		this->m_samplerate = 0.0f;
		this->m_buff = nullptr;

		//open the file
		SNDFILE *sndFile;
		SF_INFO sfInfo;
		//sndFile = sf_open(filename, SFM_READ, &sfInfo);

		SF_VIRTUAL_IO vio;
		AZ::IO::HandleType handle = AZ::IO::InvalidHandle;
		vio.get_filelen = VIO::get_filelen;
		vio.read = VIO::read;
		vio.seek = VIO::seek;
		vio.tell = VIO::tell;
		vio.write = VIO::write;

		//resolve the path
		if (gEnv) {
			/*char * resolvedPath = new char[AZ_MAX_PATH_LEN];
			gEnv->pFileIO->ResolvePath(filename, resolvedPath, AZ_MAX_PATH_LEN);
			AZ_Printf("AudioSource_libsnd", "Resolved Audio Path - %s", resolvedPath);
			sndFile = sf_open(resolvedPath, SFM_READ, &sfInfo);
			delete resolvedPath;*/

			if (gEnv->pFileIO->Open(filename, AZ::IO::OpenMode::ModeRead | AZ::IO::OpenMode::ModeBinary, handle) != AZ::IO::ResultCode::Success) {
				pushError(-1, "Cannot open file.");
				return;
			}

			if (handle != AZ::IO::InvalidHandle) sndFile = sf_open_virtual(&vio, SFM_READ, &sfInfo, (void*)handle);
			else {
				pushError(-2, "Invalid File Handle.");
				return;
			}
		} else
			sndFile = sf_open(filename, SFM_READ, &sfInfo);

		//make sure we have no error opening the file.
		if (!sndFile) {
			int err = sf_error(sndFile);
			pushError(err, sf_error_number(err));
			sndFile = nullptr;
			if (gEnv && handle != AZ::IO::InvalidHandle) gEnv->pFileIO->Close(handle);
			return;
		}

		//make sure we have no errors.
		int err = sf_error(sndFile);
		if (err != SF_ERR_NO_ERROR) {
			int err = sf_error(sndFile);
			pushError(err, sf_error_number(err));
			sf_close(sndFile); //close the libsndfile file.
			sndFile = nullptr;
			if (gEnv && handle != AZ::IO::InvalidHandle) gEnv->pFileIO->Close(handle);
			return;
		}

		//load the data
		this->m_numFrames = sfInfo.frames;
		this->m_samplerate = sfInfo.samplerate;
		this->m_numChannels = sfInfo.channels;

		this->m_format = this->GetFrameType();

		AZ_Printf("AudioSource_libsnd_memory", "[AudioSource_libsnd_memory] Channels: %i", sfInfo.channels);
		AZ_Printf("AudioSource_libsnd_memory", "[AudioSource_libsnd_memory] Sample rate: %i", sfInfo.samplerate);

		//calculate the length of the file into a human readable format.
		this->m_timeLength.totalSec = (1.0 * sfInfo.frames) / sfInfo.samplerate;
		this->m_timeLength.hrs = (int)(this->m_timeLength.totalSec / 3600.0);
		this->m_timeLength.minutes = (int)((this->m_timeLength.totalSec - (this->m_timeLength.hrs * 3600.0)) / 60.0);
		this->m_timeLength.sec = this->m_timeLength.totalSec - (this->m_timeLength.hrs * 3600.0) - (this->m_timeLength.minutes * 60.0);

		AZ_Printf("AudioSource_libsnd_memory","[AudioSource_libsnd_memory] File Length: %i:%i:%f - %f", this->m_timeLength.hrs, this->m_timeLength.minutes, this->m_timeLength.sec, this->m_timeLength.totalSec);

		//create the memory buffer
		this->m_buff = new float[sfInfo.frames * sfInfo.channels];

		//make sure we are at the beginning of the file.
		sf_seek(sndFile, 0, SF_SEEK_SET);

		//make sure we have no errors.
		err = sf_error(sndFile);
		if (err != SF_ERR_NO_ERROR) {
			int err = sf_error(sndFile);
			pushError(err, sf_error_number(err));
			sf_close(sndFile); //close the libsndfile file.
			sndFile = nullptr;
			if (gEnv && handle != AZ::IO::InvalidHandle) gEnv->pFileIO->Close(handle);
			return;
		}

		//read the entire file into memory.
		sf_readf_float(sndFile, this->m_buff, sfInfo.frames);

		//make sure we have no error.
		err = sf_error(sndFile);
		if (err != SF_ERR_NO_ERROR) {
			int err = sf_error(sndFile);
			pushError(err, sf_error_number(err));
		}

		//close the file.
		sf_close(sndFile);
		if (gEnv && handle != AZ::IO::InvalidHandle) gEnv->pFileIO->Close(handle);
	}

	AudioSource_Libsnd_Memory::~AudioSource_Libsnd_Memory() {
		//free up the memory.
		if (this->m_buff) delete[] this->m_buff;
	}

	bool AudioSource_Libsnd_Memory::Seek(long long position) {
		//make sure we are within bounds.
		if (position >= this->m_numFrames) position = this->m_numFrames;
		if (position <= 0) position = 0;

		//set internal position.
		this->m_pos = position;

		//return that the seek is good (because we have no errors and that the position is always set correctly).
		return !this->m_hasError;
	}

	long long AudioSource_Libsnd_Memory::GetFrames(long long framesToRead, float* buff) {
		//make sure we have no errors.
		if (this->m_hasError) return 0;

		//make sure we are not at the end of the buffer.
		if (this->m_pos >= this->m_numFrames) return 0;

		//make sure we are not trying to copy more than what the buffer holds
		if (this->m_pos + framesToRead > this->m_numFrames) framesToRead = (this->m_pos + framesToRead) - this->m_numFrames;

		//copy memory.
		for (long long i = (this->m_pos * this->m_numChannels), i2 = 0; i < ((framesToRead + this->m_pos) * this->m_numChannels); i++, i2++) {
			if (i > this->m_numFrames * this->m_numChannels) break;
			buff[i2] = this->m_buff[i];
		}

		//memcpy(buff, this->m_buff + (this->m_pos * this->m_numChannels), framesToRead * this->m_numChannels);

		//update internal position.
		this->m_pos += framesToRead;

		//process per source dsp effect
		//this->ProcessEffects(this->m_format, buff, framesToRead, this);

		//return the number of frames read.
		return framesToRead;
	}

	bool AudioSource_Libsnd_Memory::GetFrame(float* frame) {
		//make sure we have no errors.
		if (this->m_hasError) return false;

		//make sure we are not at the end of the buffer.
		if (this->m_pos >= this->m_numFrames) return false;

		//make sure that we are not trying to copy more than what the buffer holds.
		if (this->m_pos + 1 > this->m_numFrames) return false;

		//copy memory.
		for (long long i = (this->m_pos * this->m_numChannels), i2 = 0; i < (this->m_pos * this->m_numChannels); i++, i2++) {
			if (i > this->m_numFrames * this->m_numChannels) break;
			frame[i2] = this->m_buff[i];
		}

		//memcpy(frame, &this->m_buff[(this->m_pos * this->m_numChannels)], this->m_numChannels);

		//update internal position
		this->m_pos++;

		//process per source dsp effect
		//this->ProcessEffects(this->m_format, frame, 1, this);

		//return that we have read the frame.
		return true;
	}

	const AlternativeAudio::AudioFrame::Type AudioSource_Libsnd_Memory::GetFrameType() {
		switch (this->m_numChannels) {
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
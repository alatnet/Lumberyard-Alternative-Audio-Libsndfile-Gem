#include <StdAfx.h>
#include "AudioSource_libsnd_virtualio.h"

namespace AlternativeAudio_Libsndfile {
	namespace VIO {
		sf_count_t get_filelen(void *userdata) {
			AZ::u64 size;
			if (gEnv->pFileIO->Size((AZ::IO::HandleType)userdata, size) != AZ::IO::ResultCode::Success)
				return 0;

			return (sf_count_t)size;
		}

		sf_count_t seek(sf_count_t offset, int whence, void *user_data) {
			if (gEnv->pFileIO->Seek((AZ::IO::HandleType)user_data, offset, (AZ::IO::SeekType)whence) != AZ::IO::ResultCode::Success) return 0;
			return offset;
		}

		sf_count_t read(void *ptr, sf_count_t count, void *user_data) {
			AZ::u64 bytesRead;
			if (gEnv->pFileIO->Read((AZ::IO::HandleType)user_data, ptr, (AZ::u64)count, false, &bytesRead) != AZ::IO::ResultCode::Success) return 0;
			return bytesRead;
		}

		sf_count_t write(const void *ptr, sf_count_t count, void *user_data) {
			AZ::u64 bytesWritten;
			if (gEnv->pFileIO->Write((AZ::IO::HandleType)user_data, ptr, (AZ::u64)count, &bytesWritten) != AZ::IO::ResultCode::Success) return 0;
			return bytesWritten;
		}

		sf_count_t tell(void *user_data) {
			AZ::u64 offset;
			if (gEnv->pFileIO->Tell((AZ::IO::HandleType)user_data, offset) != AZ::IO::ResultCode::Success) return 0;
			return offset;
		}
	}
}
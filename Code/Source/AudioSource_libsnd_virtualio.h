#pragma once

#include <sndfile.h>

namespace AlternativeAudio_Libsndfile {
	namespace VIO{
		sf_count_t get_filelen(void *userdata);
		sf_count_t seek(sf_count_t offset, int whence, void *user_data);
		sf_count_t read(void *ptr, sf_count_t count, void *user_data);
		sf_count_t write(const void *ptr, sf_count_t count, void *user_data);
		sf_count_t tell(void *user_data);
	}
}
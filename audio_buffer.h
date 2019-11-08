#ifndef AUDIO_BUFFER_H
#define AUDIO_BUFFER_H

#include "typedefs.h"

class AudioBuffer {

	public:
		AudioBuffer(int size);
		~AudioBuffer();

		int setup(int insert_size, int read_size, int hop_size);
		int addAccessor();
		void addSamples(sample_t* moar_audios, int count);
		void getBuffer(const int& samples) const;

	protected:
		sample_t* buf;

		int input_head;

		int size = 0;

		int* accessors;
		int accessor_count = 0;

};

#endif //AUDIO_BUFFER_H

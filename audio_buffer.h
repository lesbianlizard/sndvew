#ifndef AUDIO_BUFFER_H
#define AUDIO_BUFFER_H

#include "typedefs.h"

class AudioBuffer {

	public:
		AudioBuffer(int size);
		~AudioBuffer();

		int setup(int insert_size, int history_size);

		void push(sample_t* moar_audios, int count);
		int pop(int handle, double* destination, int count);

		int unread(int handle) const;

		int addAccessor();

		void print() {
			for (int i=0; i<size; i++) {
				fprintf(stderr, "%.1f ", buf[i]);
			}
			fprintf(stderr, "\n");
		}

	protected:
		double* buf;

		int idx(int i) {
			return i%size;
		}

		int& acc_head(int a) const {
			if (a >= accessor_count) {
				fprintf(stderr, "accessor handle out of range\n");
			}
			return accessors[a];
		};

		int headroom(int accessor) {
			return size - acc_head(accessor);
		}

		void shift(int samples);

		int input_head = 0;
		int read_head = 0;

		int size = 0;

		int* accessors = nullptr;
		int accessor_count = 0;

};

#endif //AUDIO_BUFFER_H

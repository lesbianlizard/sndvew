#include "string.h"
#include "audio_buffer.h"

AudioBuffer::AudioBuffer(int size) : size(size) {
	this->buf = new sample_t[size];
}

AudioBuffer::~AudioBuffer() {
	delete[] buf;
}

int AudioBuffer::addAccessor() {
	int* temp = nullptr;

	if (accessors)
		temp = accessors;

	accessors = new int[++accessor_count];
	accessors[accessor_count-1] = 0;

	if (temp) {
		memcpy(accessors, temp, (accessor_count-1)*sizeof(int));
		delete[] temp;
	}

	return accessor_count;
}

int AudioBuffer::setup(int insert_size, int max_read_size, int hop_size) {
	// while (
	// size = 
	this->buf = new sample_t[insert_size];
	this->input_head = 0;
	return 0;
}

// [ x o o ]
// [ x x o ]
// [ x x x ]

void AudioBuffer::addSamples(sample_t* moar_audios, int count) {
	if (count >= size)
		count = size;

	// for (int i=0; i<count; i++) {
	// 	this->buf[i] = moar_audios[i];
	// 	if (this->
	// }

	if (this->input_head+count >= this->size) {
		this->input_head = 0;
	}
	memcpy(this->buf + sizeof(sample_t)*this->input_head, moar_audios, count*sizeof(sample_t));
}

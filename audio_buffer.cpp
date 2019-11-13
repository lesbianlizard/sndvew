#include <string.h>
#include <stdio.h>
#include "audio_buffer.h"

AudioBuffer::AudioBuffer(int size) : size(size) {
	this->buf = new double[size];
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

int AudioBuffer::setup(int insert_size, int history_size) {

	this->buf = new double[history_size];
	this->input_head = 0;
	this->size = history_size;

	return 0;
}

// [ x o o ]
// [ x x o ]
// [ x x x ]

void AudioBuffer::shift(int samples) {
	for (int i=0; i<this->size; i++) {
		if (i+samples >= this->size) {
			this->buf[i] = 0;
		}
		else {
			this->buf[i] = this->buf[i+samples];
		}
	}

	this->input_head -= samples;
	if (this->input_head < 0) {
		this->input_head = 0;
	}
}

void AudioBuffer::push(sample_t* moar_audios, int count) {
	if (count >= size) {
		fprintf(stderr, "Warning: attempt to insert more samples than buffer is large\n");
		count = size;
	}

	// if (idx(input_head+count) < read_head)
	// 	fprintf(stderr, "Warning: buffer overfilled, you yall gotta read faster\n");

	int index = input_head;
	for (int i=0; i<count; i++) {
		index = idx(input_head + i);
		this->buf[index] = moar_audios[i];
	}
	input_head = idx(index+1);

	// memcpy(this->buf + sizeof(sample_t)*this->input_head, moar_audios, count*sizeof(sample_t));
}


int AudioBuffer::pop(double* dest, int count) {
	if (count >= size) {
		fprintf(stderr, "Warning: attempt to read more samples than buffer is large\n");
		count = size;
	}

	// if (idx(read_head+count) < input_head)
	// 	fprintf(stderr, "Warning: buffer overread, you gotta write faster\n");

	// if we're straddling the end of the buffer we need to do two copies
	if (read_head+count >= size) {
		int right_side_samples = size-read_head;

		// copy the part on the right side of the buffer
		memcpy(dest, &buf[read_head], (right_side_samples)*sizeof(double));

		// copy the part on the left side
		memcpy(&dest[right_side_samples], buf, (count - right_side_samples)*sizeof(double));
	}
	// if we're not, then we only need one copy 
	else {
		memcpy(dest, &buf[read_head], count*sizeof(double));
	}

	read_head = idx(read_head+count);
	return count;
}

int AudioBuffer::unread() const {
	if (input_head < read_head)
		return (size+input_head - read_head);

	return input_head - read_head;
}

#include "spectrograph.h"

Spectrograph::Spectrograph() {
	this->width = 512;
	this->height = 512;

	this->fft_buffer = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * 1024);
	this->fft_plan = fftw_plan_dft_r2c_1d(1024, this->fft_buffer);
}

void Spectrograph::setup() {
    shader_program = common_get_shader_program(vertex_shader_source, fragment_shader_source);
}

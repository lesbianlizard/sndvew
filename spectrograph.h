#ifndef SNDVEW_SPECTROGRAPH_H
#define SNDVEW_SPECTROGRAPH_H

#include <fftw3.h>
#include <glm/glm.hpp>
#include "audio_buffer.h"
#include "graph.h"

class Spectrograph : public TextureGraph {
	public:
		Spectrograph();
		Spectrograph(glm::vec3 pos, glm::vec3 size);
		// ~Spectrograph();

		void setup(AudioBuffer*, const char*);
		void update();
		void draw();

	protected:

		int current_col=0;

		int handle;
		AudioBuffer* audio_buffer;

		double* fft_input_buffer;
		fftw_complex* fft_buffer;
		fftw_plan fft_plan;
		int fft_samples;

		GLuint shader;
		GLuint vbo;
		GLuint texture;

		GLfloat* texturebuf;

		GLuint gradient_tex;

	
		static const constexpr GLchar* vsh_source =
			"#version 120\n"
			"//precision mediump float;\n"
			// "uniform mat4 model;\n"
			"uniform mat4 proj;\n"
			"uniform mat4 view;\n"

			"attribute vec3 position;\n"
			"attribute vec2 a_texCoord;\n"

			"varying vec2 v_texCoord;\n"

			"void main() {\n"
			"   gl_Position = proj * view * vec4(position, 1.0);\n"
			"   v_texCoord = vec2(a_texCoord.x, 1.0 - a_texCoord.y);\n"
			"}\n";

		static const constexpr GLchar* fsh_source =
			"#version 120\n"
			"//precision mediump float;\n"

			"uniform float x_offset;\n"

			"uniform sampler2D audio_data;\n"
			"uniform sampler2D gradient;\n"
			"varying vec2 v_texCoord;\n"

			"void main() {\n"

			"	float ampl = texture2D(audio_data, vec2(v_texCoord.x + x_offset, pow(v_texCoord.y, 2.0)/3.0)).r;\n"
			// "	float ampl = texture2D(audio_data, vec2(v_texCoord.x + x_offset, v_texCoord.y)).r * .5;\n"
			"  gl_FragColor = vec4(texture2D(gradient, vec2(ampl, 1.0)));\n"
			// "  gl_FragColor = vec4(vec3(ampl), 1.0);\n"
			"}\n";

};


#endif // SNDVEW_SPECTROGRAPH_H

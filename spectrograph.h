#ifndef SPECTROGRAPH_H
#define SPECTROGRAPH_H

#include <fftw3.h>
#include <glm/glm.hpp>

class Graph {
	protected:
		glm::vec3 position  = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 size      = glm::vec3(50.0f, 50.0f, 0.0f);
		glm::mat4 transform = glm::mat4(
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
				);

		Graph() {};
};


class Spectrograph : public Graph {
	public:
		Spectrograph();
		// ~Spectrograph();

		void setSize(int width, int height);

		void setup(double*, const char*);
		void update();
		void draw();

	protected:

		int current_col=0;
		int width=0;
		int height=0;

		double* snd_buffer_ptr;

		fftw_complex* fft_buffer;
		fftw_plan fft_plan;
		int fft_samples;

		GLuint shader;
		GLuint vbo;
		GLuint texture;

		GLfloat* texturebuf;

		GLuint gradient_tex;

		GLfloat vertices[30];
	
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
			// "if (round(sin(v_texCoord.x*20.0f)) == 1.0f) {\n"
			"   gl_FragColor = vec4(1.0f);\n"
			// "} else\n"
			// "   gl_FragColor = vec4(0.0f);\n"
			// "}\n"
			// "   gl_FragColor = vec4(v_texCoord, 1.0, 1.0);\n"
			"	float ampl = texture2D(audio_data, vec2(v_texCoord.x + x_offset, pow(v_texCoord.y, 2.0)/5)).r;\n"
			"  gl_FragColor = vec4(texture2D(gradient, vec2(ampl, 1.0)));\n"
			// "  gl_FragColor = vec4(vec3(ampl), 1.0);\n"
			"}\n";

};

class Oscilloscope {
	public:

		void setSize(int width, int height);

	protected:
		int width=0;
		int height=0;
};

#endif // SPECTROGRAPH_H

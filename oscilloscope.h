#ifndef SNDVEW_OSCILLOSCOPE_H
#define SNDVEW_OSCILLOSCOPE_H

#include <glm/glm.hpp>
#include "audio_buffer.h"
#include "graph.h"

class Oscilloscope: TextureGraph {
	public:
		Oscilloscope();
		Oscilloscope(glm::vec3 pos, glm::vec3 size);

		void setup(AudioBuffer* audio_buffer, const char* gradient_filename);
		void update();
		void draw();

	protected:
		int sample_count = 512;
		GLuint texture;
		GLuint gradient_tex;
		GLuint shader;
		GLuint vbo;

		double* audiobuf;
		GLfloat* texturebuf;

		AudioBuffer* audio_buffer;
		int handle;



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

			"uniform float max_ampl;\n"

			"uniform sampler2D audio_data;\n"
			"uniform sampler2D gradient;\n"
			"varying vec2 v_texCoord;\n"

			"void main() {\n"

			// "  gl_FragColor = vec4(vec3(.3), 1.0);\n"
			"	float ampl = texture2D(audio_data, v_texCoord).r;\n"
			// "	float ampl = texture2D(audio_data, vec2(v_texCoord.x + x_offset, v_texCoord.y)).r * .5;\n"
			// "  gl_FragColor = vec4(texture2D(gradient, vec2(ampl, 1.0)));\n"
			" vec3 color = vec3(0,0,0);\n"
			// " float dist = 1.0f/(v_texCoord.y - ampl);\n"
			" if (v_texCoord.y < ampl+.005 && v_texCoord.y > ampl-.005) {\n"
			// "		color = texture2D(gradient, vec2(max_ampl*5, 0.0)).rgb;\n"
			"		color = vec3(.9* v_texCoord.y * 1.5, .9 * (1-v_texCoord.y) * 1.5, .1);\n"
			"}\n"
			"  gl_FragColor = vec4(color, 1.0);\n"
			"}\n";

};

#endif //SNDVEW_OSCILLOSCOPE_H

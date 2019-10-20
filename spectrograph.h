#ifndef SPECTROGRAPH_H
#define SPECTROGRAPH_H

// class Object {
// 	public:
//
// }

class Spectrograph {
	public:

		void setSize(int width, int height);

		void setup();
		void draw();

	protected:
		Spectrograph();
		~Spectrograph();

		int width=0;
		int height=0;

		fftw_complex* fft_buffer;
		fftw_plan fft_plan;

		GLint shader;

		static const GLchar* vsh_source =
			"#version 100\n"
			"attribute vec3 position;\n"
			"attribute vec2 a_texCoord;\n"
			"varying vec2 v_texCoord;\n"
			"void main() {\n"
			"   gl_Position = vec4(position, 1.0);\n"
			"   v_texCoord = a_texCoord;\n"
			"}\n";

		static const GLchar* fsh_source =
			"#version 100\n"
			"precision mediump float;\n"
			"uniform sampler2D audio_data;\n"
			"varying vec2 v_texCoord;\n"
			"void main() {\n"
			// "   gl_FragColor = vec4(v_texCoord, 1.0, 1.0);\n"
			"   gl_FragColor = vec4(texture2D(audio_data, vec2(v_texCoord.x, v_texCoord.y) ).r * vec3(0.0, 0.5, 2.0), 1.0);\n"
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

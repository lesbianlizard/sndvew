#define GLFW_INCLUDE_ES3
#include <GLFW/glfw3.h>

#include "spectrograph.h"
#include <complex.h>
#include <fftw3.h>
#include <cstring>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>

void is_bad_problem() {
    switch(glGetError()) {
	break;
	case GL_NO_ERROR:
           fprintf(stderr, "No error has been recorded. The value of this symbolic constant is guaranteed to be 0.\n");

	   break;
	case GL_INVALID_ENUM:
           fprintf(stderr, "An unacceptable value is specified for an enumerated argument. The offending command is ignored and has no other\n");
           fprintf(stderr, "side effect than to set the error flag.\n");

	   break;
	case        GL_INVALID_VALUE:
           fprintf(stderr, "A numeric argument is out of range. The offending command is ignored and has no other side effect than to set the\n");
           fprintf(stderr, "error flag.\n");

	   break;
	case        GL_INVALID_OPERATION:
           fprintf(stderr, "The specified operation is not allowed in the current state. The offending command is ignored and has no other side\n");
           fprintf(stderr, "effect than to set the error flag.\n");

	   break;
	case        GL_INVALID_FRAMEBUFFER_OPERATION:
           fprintf(stderr, "The framebuffer object is not complete. The offending command is ignored and has no other side effect than to set\n");
           fprintf(stderr, "the error flag.\n");

	   break;
	case        GL_OUT_OF_MEMORY:
           fprintf(stderr, "There is not enough memory left to execute the command. The state of the GL is undefined, except for the state of\n");
           fprintf(stderr, "the error flags, after this error is recorded.\n");

	   break;

	default:
	   fprintf(stderr, "wtf even\n");
    }
}


GLint compile_shader(const char* vertex_shader_source, const char* fragment_shader_source) {
    enum Consts {INFOLOG_LEN = 512};
    GLchar infoLog[INFOLOG_LEN];
    GLint fragment_shader;
    GLint shader_program;
    GLint success;
    GLint vertex_shader;

    /* Vertex shader */
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, INFOLOG_LEN, NULL, infoLog);
        printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
    }

    /* Fragment shader */
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, INFOLOG_LEN, NULL, infoLog);
        printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
    }

    /* Link shaders */
    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program, INFOLOG_LEN, NULL, infoLog);
        printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    return shader_program;
}

Spectrograph::Spectrograph() {
	this->width = 512;
	this->height = 1024;

	this->fft_buffer = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * 2048);
}

void Spectrograph::setSize(int x, int y) {
    this->size.x = x;
    this->size.y = y;

    this->vertices[1] = this->size[1];
    this->vertices[16] = this->size[1];
    this->vertices[21] = this->size[1];

    this->vertices[5] = this->size[0];
    this->vertices[20] = this->size[0];
    this->vertices[25] = this->size[0];
}

void Spectrograph::setup(double* snd_buffer) {

	this->snd_buffer_ptr = snd_buffer;
	this->fft_plan = fftw_plan_dft_r2c_1d(1024, this->snd_buffer_ptr, this->fft_buffer, FFTW_ESTIMATE);

	this->shader = compile_shader(this->vsh_source, this->fsh_source);
	GLint pos = glGetAttribLocation(this->shader, "position");
	GLint tex_pos = glGetAttribLocation(this->shader, "a_texCoord");

	// GLfloat vertices[] = {
	// 	-1.0f,  1.0f,  0.0f,   0.0f, 1.0f,
	// 	1.0f,  -1.0f,  0.0f,   1.0f, 0.0f,
	// 	-1.0f, -1.0f,  0.0f,   0.0f, 0.0f,
        //
	// 	-1.0f,  1.0f,  0.0f,   0.0f, 1.0f,
	// 	1.0f,   1.0f,  0.0f,   1.0f, 1.0f,
	// 	1.0f,  -1.0f,  0.0f,   1.0f, 0.0f,
	// };

	GLfloat vertices[] = {
		0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	};
	memcpy(this->vertices, vertices, sizeof(GLfloat)*30);

	this->setSize(512, 512);

	glGenBuffers(1, &this->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->vertices), this->vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (GLvoid*)0);
	glVertexAttribPointer(tex_pos, 2, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
	glEnableVertexAttribArray(pos);
	glEnableVertexAttribArray(tex_pos);

	glGenTextures(1, &this->texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glUseProgram(this->shader);

	// glGenerateMipmap(GL_TEXTURE_2D);

	GLint tex_u = glGetUniformLocation(this->shader, "audio_data");
	glUniform1i(tex_u, 0);

	unsigned char buf[this->width*3][this->height];
	for (int i=0; i<this->width*3; i++) {
	    for (int j=0; j<this->height; j++) {
			buf[i][j] = 50;
		}
		buf[i][0] = this->fft_buffer[i][1];
	}
	// g::spec::img[i][g::spec::col] = abs(g::spec::out[i][REAL]);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, this->width, this->height, 0, GL_RED, GL_UNSIGNED_BYTE, buf);

	// GLfloat identityMatrix[] = {
	//     1.0f, 0.0f, 0.0f, 0.0f,
	//     0.0f, 1.0f, 0.0f, 0.0f,
	//     0.0f, 0.0f, 1.0f, 0.0f,
	//     0.0f, 0.0f, 0.0f, 1.0f,
	// };

	// GLint modelAddr = glGetUniformLocation(this->shader, "model");
	// glUniformMatrix4fv(modelAddr, 1, GL_FALSE, glm::value_ptr(this->transform));

	glm::mat4 view          = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 projection    = glm::mat4(1.0f);
	// projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        // projection = glm::perspective(0.1f, 1920.0f / 540.0f, 0.1f, 100.0f);
	projection = glm::ortho(0.0f, 1920.0f, 1080.0f, 0.0f, 0.1f, 100.0f);
        view       = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

	GLint viewAddr = glGetUniformLocation(this->shader, "view");
	glUniformMatrix4fv(viewAddr, 1, GL_FALSE, glm::value_ptr(view));

	GLint projAddr = glGetUniformLocation(this->shader, "proj");
	glUniformMatrix4fv(projAddr, 1, GL_FALSE, glm::value_ptr(projection));

}

void Spectrograph::update() {
    fftw_execute(this->fft_plan);

    // unsigned char buf[this->width*3][this->height];
    // for (int i=0; i<this->width*3; i++) {
	// for (int j=0; j<this->height; j++) {
	//     buf[i][j] = 1;
	// }
	// buf[i][0] = this->fft_buffer[i][1];
    // }

    // unsigned char buf[1][this->height*3];
    // for (int i=0; i<this->height; i++) {
	// for (int j=0; j<3; j++) {
	//     buf[0][(3*i)+j] = std::max(this->fft_buffer[i][0], 0.0) * 255;
	// }
    // }

    GLfloat buf[1][this->height*10];
    for (int i=0; i<this->height*10; i++) {
	if (i < current_col) {
	    buf[0][i] = 255;
	} else {
	    buf[0][i] = 0;
	}
	// std::max(this->fft_buffer[i][0], 0.0) * 255;
    }
    fprintf(stderr, "%i\n", current_col);


    glBindTexture(GL_TEXTURE_2D, this->texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, current_col/2, 0, 1, this->height, GL_RED, GL_FLOAT, buf);
    is_bad_problem();

    // is_bad_problem();


    current_col+=1;
    if (current_col >= this->width) {
	current_col = 0;
    }

    // for (int i=0; i<g::spec::h; i++) {
		 // g::spec::img[i][g::spec::col] = abs(g::spec::out[i][REAL]);
    // }
}

void Spectrograph::draw() {
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBindTexture(GL_TEXTURE_2D, this->texture);
	glUseProgram(this->shader);

	// GLint wtfAddr = glGetUniformLocation(this->shader, "wtf");
	// glUniform1f(wtfAddr, 1.0f);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

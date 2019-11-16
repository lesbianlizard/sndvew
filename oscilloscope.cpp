#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "stb_image.h"
#include "oscilloscope.h"
#include "opengl_utils.h"
#include <cstring>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
#include <string.h>

Oscilloscope::Oscilloscope() {}
Oscilloscope::Oscilloscope(glm::vec3 pos, glm::vec3 size) : TextureGraph(pos, size) {}

void Oscilloscope::setup(AudioBuffer* audio_buffer, const char* gradient_filename) {

	this->audio_buffer = audio_buffer;
	this->handle = this->audio_buffer->addAccessor();

	this->audiobuf = new double[sample_count];

	this->shader = compile_shader(this->vsh_source, this->fsh_source);
	GLint pos = glGetAttribLocation(this->shader, "position");
	GLint tex_pos = glGetAttribLocation(this->shader, "a_texCoord");

	glGenBuffers(1, &this->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->vertices), this->vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (GLvoid*)0);
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glVertexAttribPointer(tex_pos, 2, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
	glEnableVertexAttribArray(pos);
	glEnableVertexAttribArray(tex_pos);

	
	
	glGenTextures(1, &this->texture);
	glBindTexture(GL_TEXTURE_2D, this->texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	glGenTextures(1, &this->gradient_tex);
	glBindTexture(GL_TEXTURE_2D, this->gradient_tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int tex_w, tex_h, tex_channels;
	unsigned char* tex_data = stbi_load(gradient_filename, &tex_w, &tex_h, &tex_channels, 0);
	if (!tex_data) {
	    fprintf(stderr, "Failed to load image texture %s\n", gradient_filename);
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_w, tex_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_data);
	stbi_image_free(tex_data);

	glUseProgram(this->shader);

	GLuint tex_u;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->gradient_tex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, this->texture);

	tex_u = glGetUniformLocation(this->shader, "gradient");
	glUniform1i(tex_u, 0);

	tex_u = glGetUniformLocation(this->shader, "audio_data");
	glUniform1i(tex_u, 1);

	texturebuf = new GLfloat[this->sample_count];

	for (int i=0; i<sample_count; i++) {
	    texturebuf[i] = 0.00f;
	}

	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, this->width, this->height, 0, GL_RED, GL_FLOAT, texturebuf);


	glm::mat4 view          = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 projection    = glm::mat4(1.0f);
	projection = glm::ortho(0.0f, 1920.0f, 1080.0f, 0.0f, 0.1f, 100.0f);
        view       = glm::translate(view, glm::vec3(position.x, position.y, -3.0f));

	GLint viewAddr = glGetUniformLocation(this->shader, "view");
	glUniformMatrix4fv(viewAddr, 1, GL_FALSE, glm::value_ptr(view));

	GLint projAddr = glGetUniformLocation(this->shader, "proj");
	glUniformMatrix4fv(projAddr, 1, GL_FALSE, glm::value_ptr(projection));

	GLint maxAmplAddr = glGetUniformLocation(this->shader, "max_ampl");
	glUniform1f(maxAmplAddr, 0.0f);

}

void Oscilloscope::update() {
    float max_ampl = 0.0;

    while (audio_buffer->unread(this->handle) >= sample_count) {
	audio_buffer->pop(this->handle, this->audiobuf, sample_count);

	for (int j=0; j<sample_count; j++) {
	    texturebuf[j] = .5+(this->audiobuf[j]);

	    float abs_ampl = std::abs(this->audiobuf[j]);
	    if (abs_ampl >= max_ampl) {
		max_ampl = abs_ampl;
	    }
	}

    }

    glUseProgram(this->shader);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, this->texture);

    // glTexSubImage2D(GL_TEXTURE_2D, 0, current_col, 0, 1, this->height, GL_RED, GL_FLOAT, texturebuf);

    // would be nice to use a float internal format like GL_R16F but it doesn't seem to be supported on the pi?
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, sample_count, 1, 0, GL_RED, GL_FLOAT, texturebuf);

    GLint maxAmplAddr = glGetUniformLocation(this->shader, "max_ampl");
    glUniform1f(maxAmplAddr, max_ampl);

}

void Oscilloscope::draw() {
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->gradient_tex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, this->texture);

	glUseProgram(this->shader);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}


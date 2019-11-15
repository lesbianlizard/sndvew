#include <glm/glm.hpp>
#include <string.h>
#include "graph.h"

TextureGraph::TextureGraph() {
	this->setup();
}
TextureGraph::TextureGraph(glm::vec3 pos, glm::vec3 size) : Object(pos, size) {
	this->setup();
	this->setSize(size);
}

void TextureGraph::setSize(glm::vec3 s) {
	this->size = s;

	this->vertices[1] = this->size[1];
	this->vertices[16] = this->size[1];
	this->vertices[21] = this->size[1];

	this->vertices[5] = this->size[0];
	this->vertices[20] = this->size[0];
	this->vertices[25] = this->size[0];
}

void TextureGraph::setup() {
	GLfloat vertices[] = {
		0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	};
	memcpy(this->vertices, vertices, sizeof(GLfloat)*30);
}

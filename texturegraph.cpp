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

	this->vertices[1] = this->size.y;
	this->vertices[16] = this->size.y;
	this->vertices[21] = this->size.y;

	this->vertices[5] = this->size.x;
	this->vertices[20] = this->size.x;
	this->vertices[25] = this->size.x;
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

#ifndef SNDVEW_GRAPH_H
#define SNDVEW_GRAPH_H

#include <GL/glew.h>
#include <glm/glm.hpp>

class Object {
	public:
		Object() {};
		Object(glm::vec3 pos, glm::vec3 size) : position(pos), size(size) {};

	protected:
		glm::vec3 position  = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 size      = glm::vec3(50.0f, 50.0f, 0.0f);
		glm::mat4 transform = glm::mat4(
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
				);


};

class TextureGraph: public Object {
	public:
		virtual void setSize(glm::vec3);
	protected:
		TextureGraph();
		TextureGraph(glm::vec3 pos, glm::vec3 size);

		GLfloat vertices[30];

	private:
		void setup();
};


#endif //SNDVEW_GRAPH_H

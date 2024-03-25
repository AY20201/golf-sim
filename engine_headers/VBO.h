#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include<glm/glm.hpp>
#include<glad/glad.h>

#include<vector>

struct Vertex
{
	glm::vec3 position;
	glm::vec2 texcoord;
	glm::vec3 normal;
	glm::vec3 tangent;
};

class VBO
{
	public:
		GLuint ID;
		VBO(std::vector<Vertex>&);

		void Bind();
		void Unbind();
		void Delete();
};

#endif
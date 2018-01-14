#ifndef GLPRESS_MESH_H
#define GLPRESS_MESH_H

#include <vector>
#include <string>
#include <memory>
#include <GL/glew.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

struct Vertex {
public:
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 textCoord;

	Vertex() {};

	Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 textCoord) :
		position(position), normal(normal), textCoord(textCoord) {}
};

class Mesh {

public:
	Mesh(std::vector<Vertex>&& vertices, std::vector<GLuint>&& indices, GLenum usage);
	~Mesh() = default;

	void draw();

	void reset();

	std::vector<Vertex>& getVertices()
	{
		return mVertices;
	}
	std::vector<GLuint>& getIndices()
	{
		return mIndices;
	}

private:

	std::vector<Vertex> mVertices;
	std::vector<GLuint> mIndices;

	GLuint mVAO;
	GLuint mVBO;
	GLuint mEBO;
	GLenum mUsage;
};


#endif //GLPRESS_MESH_H

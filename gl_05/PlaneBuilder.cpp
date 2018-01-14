#include "PlaneBuilder.h"


std::shared_ptr<Mesh> PlaneBuilder::build()
{
		std::vector<Vertex> vertices;
		std::vector<GLuint> indices{ 0,  1,  2,
			2,  1,  3, };

		int direction = 1;
		if(rotation == 0)
		{
			vertices.emplace_back(glm::vec3(-0.5 * mWidth, -0.5 * mHeight, 0.0f), glm::vec3(0, 0, direction), glm::vec2(1.0f, 0.0f));
			vertices.emplace_back(glm::vec3(0.5 * mWidth, -0.5 * mHeight, 0.0f), glm::vec3(0, 0, direction), glm::vec2(1.0f, 1.0f));
			vertices.emplace_back(glm::vec3(-0.5 * mWidth, 0.5 * mHeight, 0.0f), glm::vec3(0, 0, direction), glm::vec2(0.0f, 0.0f));
			vertices.emplace_back(glm::vec3(0.5 * mWidth, 0.5 * mHeight, 0.0f), glm::vec3(0, 0, direction), glm::vec2(0.0f, 1.0f));
		}
		else if(rotation == 1)
		{
			vertices.emplace_back(glm::vec3(0.5 * mWidth, -0.5 * mHeight, 0.0f), glm::vec3(0, 0, -direction), glm::vec2(0.0f, 0.0f));
			vertices.emplace_back(glm::vec3(-0.5 * mWidth, -0.5 * mHeight, 0.0f), glm::vec3(0, 0, -direction), glm::vec2(0.0f, 1.0f));
			vertices.emplace_back(glm::vec3(0.5 * mWidth, 0.5 * mHeight, 0.0f), glm::vec3(0, 0, -direction), glm::vec2(1.0f, 0.0f));
			vertices.emplace_back(glm::vec3(-0.5 * mWidth, 0.5 * mHeight, 0.0f), glm::vec3(0, 0, -direction), glm::vec2(1.0f, 1.0f));
		}
		else if(rotation == 2)
		{
			vertices.emplace_back(glm::vec3(-0.5 * mWidth, 0.0f, 0.5 * mDepth), glm::vec3(0, -direction, 0), glm::vec2(1.0f, 0.0f));
			vertices.emplace_back(glm::vec3(0.5 * mWidth, 0.0f * mHeight, 0.5 * mDepth), glm::vec3(0, -direction, 0), glm::vec2(0.0f, 0.0f));
			vertices.emplace_back(glm::vec3(-0.5 * mWidth, 0.0f * mHeight, -0.5 * mDepth), glm::vec3(0, -direction, 0), glm::vec2(1.0f, 1.0f));
			vertices.emplace_back(glm::vec3(0.5 * mWidth, 0.0f * mHeight, -0.5 * mDepth), glm::vec3(0, -direction, 0), glm::vec2(0.0f, 1.0f));
		}

		else if (rotation == 3)
		{
			vertices.emplace_back(glm::vec3(-0.5 * mWidth, 0.0f * mHeight, 0.5 * mDepth), glm::vec3(0, direction, 0), glm::vec2(0.0f, 0.0f));
			vertices.emplace_back(glm::vec3(0.5 * mWidth, 0.0f * mHeight, 0.5 * mDepth), glm::vec3(0, direction, 0), glm::vec2(1.0f, 0.0f));
			vertices.emplace_back(glm::vec3(-0.5 * mWidth, 0.0f * mHeight, -0.5 * mDepth), glm::vec3(0, direction, 0), glm::vec2(0.0f, 1.0f));
			vertices.emplace_back(glm::vec3(0.5 * mWidth, 0.0f * mHeight, -0.5 * mDepth), glm::vec3(0, direction, 0), glm::vec2(1.0f, 1.0f));

		}

		else if (rotation == 4)
		{
			vertices.emplace_back(glm::vec3(0.0f * mWidth, -0.5 * mHeight, 0.5 * mDepth), glm::vec3(direction, 0, 0), glm::vec2(1.0f, 0.0f));
			vertices.emplace_back(glm::vec3(0.0f * mWidth, -0.5 * mHeight, -0.5 * mDepth), glm::vec3(direction, 0, 0), glm::vec2(1.0f, 1.0f));
			vertices.emplace_back(glm::vec3(0.0f * mWidth, 0.5 * mHeight, 0.5 * mDepth), glm::vec3(direction, 0, 0), glm::vec2(0.0f, 0.0f));
			vertices.emplace_back(glm::vec3(0.0f * mWidth, 0.5 * mHeight, -0.5 * mDepth), glm::vec3(direction, 0, 0), glm::vec2(0.0f, 1.0f));
		}

		else if (rotation == 5)
		{
			vertices.emplace_back(glm::vec3(0.0f * mWidth, -0.5 * mHeight, -0.5 * mDepth), glm::vec3(-direction, 0, 0), glm::vec2(0.0f, 1.0f));
			vertices.emplace_back(glm::vec3(0.0f * mWidth, -0.5 * mHeight, 0.5 * mDepth), glm::vec3(-direction, 0, 0), glm::vec2(0.0f, 0.0f));
			vertices.emplace_back(glm::vec3(0.0f * mWidth, 0.5 * mHeight, -0.5 * mDepth), glm::vec3(-direction, 0, 0), glm::vec2(1.0f, 1.0f));
			vertices.emplace_back(glm::vec3(0.0f * mWidth, 0.5 * mHeight, 0.5 * mDepth), glm::vec3(-direction, 0, 0), glm::vec2(1.0f, 0.0f));
		}
	

		return std::make_shared<Mesh>(std::move(vertices), std::move(indices), GL_STATIC_DRAW);
}


PlaneBuilder::~PlaneBuilder()
{
}

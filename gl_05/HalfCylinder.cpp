#include <glm/gtc/constants.hpp>
#include <cmath>
#include <glm/geometric.hpp>
#include "HalfCylinder.h"

std::shared_ptr<Mesh> HalfCylinder::build() {

	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;

	float deltaAngle = glm::two_pi<float>() / mSides;

	GLfloat deltaU = (mWrapEnd.x - mWrapBegin.x) / mSides;

	GLuint start = right ? 0 : mSides / 2;
	GLuint stop = right ? mSides / 2 : mSides;

	GLfloat normalizer = mRadius / mRadius1;

	for (GLuint i = start; i <= stop; ++i) {
		float x = mRadius * std::cos(deltaAngle * i);
		float y = mRadius1 * std::sin(deltaAngle * i);
			vertices.emplace_back(glm::vec3(x, y, -mHeight / 2), glm::normalize(glm::vec3(x, y, 0)),
				mWrapBegin.x + glm::vec2(i * deltaU / normalizer, mWrapBegin.y* normalizer));
			vertices.emplace_back(glm::vec3(x, y, mHeight / 2), glm::normalize(glm::vec3(x, y, 0)),
				mWrapBegin.x + glm::vec2(i * deltaU / normalizer, mWrapEnd.y* normalizer));
	}


	for (GLuint i = 0; i < mSides - mSides%2; ++i) {
		indices.emplace_back(2 * ((i + 2) / 2) - 1);
		indices.emplace_back(i + 2);
		indices.emplace_back(2 * ((i + 1) / 2) );
	}

	// Caps
	GLuint centerIdx = (GLuint)vertices.size();

	glm::vec2 lowerTextureCenter(mLowerCapBegin + (mLowerCapEnd - mLowerCapBegin) * 0.5f);
	GLfloat lowerTextureSize = mLowerCapEnd.x - mLowerCapBegin.x;




	vertices.emplace_back(glm::vec3(0, 0, -mHeight / 2), glm::vec3(0, 0, -1.0f), lowerTextureCenter);
	for (GLuint i = start; i <= stop; ++i) {
		float x = std::cos(deltaAngle * i);
		float y = std::sin(deltaAngle * i);

		vertices.emplace_back(glm::vec3(x * mRadius, y * mRadius1, -mHeight / 2),
			glm::vec3(0, 0, -1.0f),
			glm::vec2(lowerTextureCenter.x + (x / 2) * lowerTextureSize / normalizer,
				lowerTextureCenter.y + (-y / 2) * lowerTextureSize));
	}

	for (GLuint i = 1; i < mSides/2 + 1; ++i) {
		indices.emplace_back(centerIdx);
		indices.emplace_back(centerIdx + i);
		indices.emplace_back(centerIdx + i + 1);
	}


	//Upper cap
	glm::vec2 upperTextureCenter(mUpperCapBegin + (mUpperCapEnd - mUpperCapBegin) * 0.5f);
	GLfloat upperTextureSize = mUpperCapEnd.x - mUpperCapBegin.x;

	centerIdx = (GLuint)vertices.size();
	vertices.emplace_back(glm::vec3(0, 0, mHeight / 2), glm::vec3(0, 0, 1.0f), upperTextureCenter);
	for (GLuint i = start; i <= stop; ++i) {
		float x = std::cos(deltaAngle * i);
		float y = std::sin(deltaAngle * i);

		vertices.emplace_back(glm::vec3(mRadius * x, mRadius1 * y, mHeight / 2),
			glm::vec3(0, 0, 1.0f),
			glm::vec2(upperTextureCenter.x + (x / 2) * upperTextureSize / normalizer,
				upperTextureCenter.y + (-y / 2) * upperTextureSize));
	}

	for (GLuint i = 1; i < mSides/2 + 1; ++i) {
		indices.emplace_back(centerIdx);
		indices.emplace_back(centerIdx + i);
		indices.emplace_back(centerIdx + i + 1);
	}
	float normalParameter = right ? -1.0f : 1.0f;

	float x = std::cos(deltaAngle * start);
	float y = std::sin(deltaAngle * start);

	vertices.emplace_back(glm::vec3(mRadius * x, mRadius1 * y, mHeight / 2),
		glm::vec3(0.0f, normalParameter, 0.0f),
		glm::vec2(mWrapBegin.x/4, mWrapEnd.y));

	vertices.emplace_back(glm::vec3(mRadius * x, mRadius1 * y, -mHeight / 2),
		glm::vec3(0.0f, normalParameter, 0.0f),
		glm::vec2(mWrapBegin.x/4, mWrapBegin.y));

	x = std::cos(deltaAngle * stop);
	y = std::sin(deltaAngle * stop);

	vertices.emplace_back(glm::vec3(mRadius * x, mRadius1 * y, mHeight / 2),
		glm::vec3(0.0f, normalParameter, 0.0f),
		glm::vec2(mWrapEnd.x/4, mWrapEnd.y));

	vertices.emplace_back(glm::vec3(mRadius * x, mRadius1 * y, -mHeight / 2),
		glm::vec3(0.0f, normalParameter, 0.0f),
		glm::vec2(mWrapEnd.x/4, mWrapBegin.y));

	if(back)
	{
		indices.emplace_back(vertices.size() - 4);
		indices.emplace_back(vertices.size() - 3);
		indices.emplace_back(vertices.size() - 2);

		indices.emplace_back(vertices.size() - 2);
		indices.emplace_back(vertices.size() - 1);
		indices.emplace_back(vertices.size() - 3);

	}


	return std::make_shared<Mesh>(std::move(vertices), std::move(indices), GL_STATIC_DRAW);
}

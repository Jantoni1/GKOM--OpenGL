#include <glm/gtc/constants.hpp>
#include <cmath>
#include <glm/geometric.hpp>
#include "Cylinder.h"

std::shared_ptr<Mesh> Cylinder::build() {

	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;

	float deltaAngle = glm::two_pi<float>() / mSides;

	GLfloat deltaU = (mWrapEnd.x - mWrapBegin.x) / mSides;

	GLfloat normalConverter = innerNormals ? -1.0 : 1.0;

	for (GLuint i = 0; i <= mSides; ++i) {
		float x = mRadius * std::cos(deltaAngle * i);
		float y = mRadius * std::sin(deltaAngle * i);

		vertices.emplace_back(glm::vec3(x, y, -mHeight / 2), glm::normalize(glm::vec3(normalConverter*x, normalConverter*y, 0)),
			mWrapBegin.x + glm::vec2(i * deltaU, mWrapBegin.y));
		vertices.emplace_back(glm::vec3(x, y, mHeight / 2), glm::normalize(glm::vec3(normalConverter*x, normalConverter*y, 0)),
			mWrapBegin.x + glm::vec2(i * deltaU, mWrapEnd.y));
	}

	if( mEffectiveSides == mSides)
	{
		indices.emplace_back((mSides - mEffectiveSides) / 2);
		indices.emplace_back((mSides - mEffectiveSides) / 2 + 1);
		indices.emplace_back((mSides - mEffectiveSides) / 2 + 2);
	}

	GLuint tmp = mSides != mEffectiveSides ? mSides - mEffectiveSides - 1 : 0;
	for (GLuint i = tmp; i <  mSides  + mEffectiveSides - 1; ++i) {
		indices.emplace_back(2 * ((i + 2) / 2));
		indices.emplace_back(i + 3);
		indices.emplace_back(2 * ((i + 1) / 2) + 1);
	}

	if(bot)
	{
		// Caps
		GLuint centerIdx = (GLuint)vertices.size();

		glm::vec2 lowerTextureCenter(mLowerCapBegin + (mLowerCapEnd - mLowerCapBegin) * 0.5f);
		GLfloat lowerTextureSize = mLowerCapEnd.x - mLowerCapBegin.x;


		vertices.emplace_back(glm::vec3(0, 0, -mHeight / 2), glm::vec3(0, 0, normalConverter*-1), lowerTextureCenter);
		for (GLuint i = 0; i <= mSides; ++i) {
			float x = std::cos(deltaAngle * i);
			float y = std::sin(deltaAngle * i);

			vertices.emplace_back(glm::vec3(x * mRadius, y * mRadius, -mHeight / 2),
				glm::vec3(0, 0, normalConverter*-1),
				glm::vec2(lowerTextureCenter.x + (x / 2) * lowerTextureSize,
					lowerTextureCenter.y + (-y / 2) * lowerTextureSize));
		}

		for (GLuint i = 1; i < mSides + 1; ++i) {
			indices.emplace_back(centerIdx);
			indices.emplace_back(centerIdx + i);
			indices.emplace_back(centerIdx + i + 1);
		}
	}
	
	if(top)
	{
		//Upper cap
		glm::vec2 upperTextureCenter(mUpperCapBegin + (mUpperCapEnd - mUpperCapBegin) * 0.5f);
		GLfloat upperTextureSize = mUpperCapEnd.x - mUpperCapBegin.x;

		GLuint centerIdx1 = (GLuint)vertices.size();
		vertices.emplace_back(glm::vec3(0, 0, mHeight / 2), glm::vec3(0, 0, normalConverter*1.0f), upperTextureCenter);
		for (GLuint i = 0; i <= mSides; ++i) {
			float x = std::cos(deltaAngle * i);
			float y = std::sin(deltaAngle * i);

			vertices.emplace_back(glm::vec3(mRadius * x, mRadius * y, mHeight / 2),
				glm::vec3(0, 0, normalConverter*1.0f),
				glm::vec2(upperTextureCenter.x + (x / 2) * upperTextureSize,
					upperTextureCenter.y + (-y / 2) * upperTextureSize));
		}

		for (GLuint i = 1; i < mSides + 1; ++i) {
			indices.emplace_back(centerIdx1);
			indices.emplace_back(centerIdx1 + i);
			indices.emplace_back(centerIdx1 + i + 1);
		}


	}


	
	return std::make_shared<Mesh>(std::move(vertices), std::move(indices), GL_STATIC_DRAW);
}

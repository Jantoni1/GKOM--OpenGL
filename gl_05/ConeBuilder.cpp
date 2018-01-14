#include <glm/gtc/constants.hpp>
#include <cmath>
#include <glm/geometric.hpp>
#include "ConeBuilder.h"
#include <iostream>

std::shared_ptr<Mesh> ConeBuilder::build() {

	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;

	float deltaAngle = glm::two_pi<float>() / mSides;

	// Caps
	GLuint centerIdx = (GLuint)0;
	GLuint hCenterIdx = (GLuint)2;

	glm::vec2 lowerTextureCenter(mLowerCapBegin + (mLowerCapEnd - mLowerCapBegin) * 0.5f);
	glm::vec2 top1 = mLowerCapBegin + glm::vec2((mLowerCapEnd.x - mLowerCapBegin.x) / 2, mLowerCapBegin.y);
	glm::vec2 top2 = mLowerCapBegin + glm::vec2((mLowerCapEnd.x - mLowerCapBegin.x) / 2, mLowerCapEnd.y);

	GLfloat lowerTextureSize = mLowerCapEnd.x - mLowerCapBegin.x;

	/*For each triangle ABC
		n : = normalize(cross(B - A, C - A))
		A.n : = n
		B.n : = n
		C.n : = n*/

	vertices.emplace_back(glm::vec3(0, 0, mHeight / 2), glm::vec3(0.0f, 0.0f, 0.0f), lowerTextureCenter);
	vertices.emplace_back(glm::vec3(0, -mRadius , -mHeight / 2), glm::vec3(0.0f, 0.0f, 0.0f), top1);
	vertices.emplace_back(glm::vec3(0, mRadius , -mHeight / 2), glm::vec3(0.0f, 0.0f, 0.0f), top2);
	for (GLuint i = 0; i <= mSides; ++i) {
		float x = std::cos(deltaAngle * i);
		float y = std::sin(deltaAngle * i);

		vertices.emplace_back(glm::vec3(x * mRadius, y * mRadius, mHeight / 2),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec2(lowerTextureCenter.x + (x / 2) * lowerTextureSize,
				lowerTextureCenter.y + (-y / 2) * lowerTextureSize));
	}
	for (GLuint i = 0; i <= mSides; ++i) {
		float x = std::cos(deltaAngle * i);
		float y = std::sin(deltaAngle * i);

		vertices.emplace_back(glm::vec3(x * mRadius, y * mRadius, mHeight / 2),
			glm::normalize(glm::vec3(x, y, 0.0f)),
			glm::vec2(lowerTextureCenter.x + (x / 2) * lowerTextureSize,
				lowerTextureCenter.y + (-y / 2) * lowerTextureSize));
	}
	for (GLuint i = 3; i <= mSides + 3; ++i)
	{
		indices.emplace_back(centerIdx);
		indices.emplace_back(centerIdx + i);
		indices.emplace_back(centerIdx + i + 1);
	}
	for (GLuint i = 3; i <= (mSides + 3) / 2 + 1; ++i) {
		indices.emplace_back(hCenterIdx);
		indices.emplace_back(hCenterIdx + i - 2);
		indices.emplace_back(hCenterIdx + i - 1);

	}
	hCenterIdx--;
	for (GLuint i = (mSides + 3) / 2 + 2; i <= mSides + 3; ++i) {
		indices.emplace_back(hCenterIdx);
		indices.emplace_back(hCenterIdx + i - 1);
		indices.emplace_back(hCenterIdx + i);
	}
	indices.emplace_back(hCenterIdx);
	indices.emplace_back(hCenterIdx + 1);
	indices.emplace_back(3);

	indices.emplace_back(hCenterIdx);
	indices.emplace_back(hCenterIdx + 1);
	indices.emplace_back(mSides + 3);

	indices.emplace_back(hCenterIdx);
	indices.emplace_back(hCenterIdx + 1);
	indices.emplace_back((mSides + 3) / 2 + 2);


	glm::vec3 vec1;
	glm::vec3 vec2;
	glm::vec3 normalized;
	hCenterIdx++;
	//for(GLuint i = 3; i < mSides + 2; ++i)
	//{
	//	vec1 = vertices[centerIdx + i].position - vertices[centerIdx].position;
	//	vec2 = vertices[(centerIdx + i + 1) % mSides / 3].position - vertices[centerIdx].position;
	//	normalized = glm::normalize(glm::cross(vec2, vec1));
	//	vertices[centerIdx].normal += normalized;
	//	vertices[centerIdx + i].normal += normalized;
	//	vertices[centerIdx + i + 1].normal += normalized;

	//}
	for (GLuint i = 3; i <= (mSides + 3) /2 + 1; ++i) {
		vec1 = vertices[hCenterIdx + i - 2].position - vertices[hCenterIdx].position;
		vec2 = vertices[hCenterIdx + i - 1].position - vertices[hCenterIdx].position;
		normalized = glm::normalize(glm::cross(vec2, vec1));
		vertices[hCenterIdx].normal += normalized;
		vertices[hCenterIdx + i - 2].normal += normalized;
		vertices[hCenterIdx + i - 1].normal += normalized;
	}
	hCenterIdx--;
	for (GLuint i = (mSides + 3) / 2 + 2 ; i <= mSides + 2; ++i) {
		vec1 = vertices[hCenterIdx + i - 1].position - vertices[hCenterIdx].position;
		vec2 = vertices[hCenterIdx + i].position - vertices[hCenterIdx].position;
		normalized = glm::normalize(glm::cross(vec2, vec1));
		vertices[hCenterIdx].normal += normalized;
		vertices[hCenterIdx + i - 1].normal += normalized;
		vertices[hCenterIdx + i].normal += normalized;
	}
	//vec1 = vertices[hCenterIdx + 1].position - vertices[3].position;
	//vec2 = vertices[hCenterIdx].position - vertices[3].position;
	//normalized = glm::normalize(glm::cross(vec2, vec1));
	//vertices[hCenterIdx].normal += normalized;
	//vertices[hCenterIdx + 1].normal += normalized;
	//vertices[3].normal += normalized;

	//vec1 = vertices[hCenterIdx + 1].position - vertices[mSides+3].position;
	//vec2 = vertices[hCenterIdx].position - vertices[mSides+3].position;
	//normalized = glm::normalize(glm::cross(vec2, vec1));
	//vertices[hCenterIdx].normal += normalized;
	//vertices[hCenterIdx + 1].normal += normalized;
	//vertices[mSides+3].normal += normalized;

	//vec1 = vertices[hCenterIdx + 1].position - vertices[(mSides + 3) / 2 + 2].position;
	//vec2 = vertices[hCenterIdx].position - vertices[(mSides + 3) / 2 + 2].position;
	//normalized = glm::normalize(glm::cross(vec2, vec1));
	//vertices[hCenterIdx].normal += normalized;
	//vertices[hCenterIdx + 1].normal += normalized;
	//vertices[(mSides + 3) / 2 + 2].normal += normalized;


	
	for (GLuint i = 0; i <= mSides + 3; ++i) {
		vertices[i].normal = glm::normalize(vertices[i].normal);
	}
	vertices[1].normal = glm::vec3(vertices[1].normal.x, 0.0f, vertices[1].normal.z);
	vertices[2].normal = glm::vec3(vertices[2].normal.x, 0.0f, vertices[2].normal.z);


	return std::make_shared<Mesh>(std::move(vertices), std::move(indices), GL_STATIC_DRAW);
}

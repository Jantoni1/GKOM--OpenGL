//
// Created by wojciech on 01.06.17.
//

#ifndef GLPRESS_CYLINDERBUILDER_H
#define GLPRESS_CYLINDERBUILDER_H

#include <glm/vec2.hpp>
#include <GL/glew.h>
#include "Mesh.h"

class Cylinder {
private:

	GLfloat mRadius;
	GLfloat mHeight;
	GLuint mSides;
	GLuint mEffectiveSides;
	bool top;
	bool bot;
	bool innerNormals;

	glm::vec2 mWrapBegin;
	glm::vec2 mWrapEnd;

	glm::vec2 mUpperCapBegin;
	glm::vec2 mUpperCapEnd;

	glm::vec2 mLowerCapBegin;
	glm::vec2 mLowerCapEnd;


public:
	Cylinder() : mRadius(0.4f), mHeight(0.4f), mSides(48), mEffectiveSides(48), top(true), bot(true), innerNormals(false) { };

	void normals(bool normal)
	{
		innerNormals = normal;
	}

	GLfloat getRadius()
	{
		return mRadius;
	}

	void includeTop(bool top)
	{
		this->top = top;
	}

	void includeBot(bool bot)
	{
		this->bot = bot;
	}

	void effectiveSides(int sides)
	{
		mEffectiveSides = sides;
	}

	GLfloat getHeight()
	{
		return mHeight;
	}

	Cylinder& radius(GLfloat radius) {
		mRadius = radius;
		return *this;
	};

	Cylinder& height(GLfloat height) {
		mHeight = height;
		return *this;
	};

	Cylinder& sides(GLuint sides) {
		mSides = sides;
		mEffectiveSides = sides;
		return *this;
	};

	Cylinder& wrap(glm::vec2 begin, glm::vec2 end) {
		mWrapBegin = begin;
		mWrapEnd = end;
		return *this;
	};

	Cylinder& upperCap(glm::vec2 begin, glm::vec2 end) {
		mUpperCapBegin = begin;
		mUpperCapEnd = end;
		return *this;
	};

	Cylinder& lowerCap(glm::vec2 begin, glm::vec2 end) {
		mLowerCapBegin = begin;
		mLowerCapEnd = end;
		return *this;
	};

	std::shared_ptr<Mesh> build();
};

#endif //GLPRESS_CYLINDERBUILDER_H

//
// Created by wojciech on 01.06.17.
//

#ifndef GLPRESS_HALF_CYLINDERBUILDER_H
#define GLPRESS_HALF_CYLINDERBUILDER_H

#include <glm/vec2.hpp>
#include <GL/glew.h>
#include "Mesh.h"

class HalfCylinder {
private:
	bool right;
	bool back;

	GLfloat mRadius;
	GLfloat mRadius1;
	GLfloat mHeight;
	GLuint mSides;

	glm::vec2 mWrapBegin;
	glm::vec2 mWrapEnd;

	glm::vec2 mUpperCapBegin;
	glm::vec2 mUpperCapEnd;

	glm::vec2 mLowerCapBegin;
	glm::vec2 mLowerCapEnd;

public:
	HalfCylinder() : mRadius(0.4f), mRadius1(0.4f), mHeight(0.4f), mSides(48), right(true), back(true) { };

	void radius1(GLfloat radius)
	{
		mRadius1 = radius;
	}

	void setBack(bool back)
	{
		this->back = back;
	}

	float getHeight()
	{
		return mHeight;
	}

	float getRadius()
	{
		return mRadius;
	}

	HalfCylinder& radius(GLfloat radius) {
		mRadius = radius;
		mRadius1 = radius;
		return *this;
	};

	HalfCylinder& height(GLfloat height) {
		mHeight = height;
		return *this;
	};

	HalfCylinder& sides(GLuint sides) {
		mSides = sides;
		return *this;
	};

	HalfCylinder& wrap(glm::vec2 begin, glm::vec2 end) {
		mWrapBegin = begin;
		mWrapEnd = end;
		return *this;
	};

	HalfCylinder& upperCap(glm::vec2 begin, glm::vec2 end) {
		mUpperCapBegin = begin;
		mUpperCapEnd = end;
		return *this;
	};

	HalfCylinder& lowerCap(glm::vec2 begin, glm::vec2 end) {
		mLowerCapBegin = begin;
		mLowerCapEnd = end;
		return *this;
	};

	void setPart(bool right)
	{
		this->right = right;
	}

	std::shared_ptr<Mesh> build();
};

#endif //GLPRESS_CYLINDERBUILDER_H

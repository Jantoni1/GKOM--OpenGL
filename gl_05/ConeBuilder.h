//
// Created by wojciech on 01.06.17.
//

#ifndef GLPRESS_ConeBuilderBUILDER_H
#define GLPRESS_ConeBuilderBUILDER_H

#include <glm/vec2.hpp>
#include <GL/glew.h>
#include "Mesh.h"

class ConeBuilder {
private:

	GLfloat mRadius;
	GLfloat mHeight;
	GLuint mSides;

	glm::vec2 mUpperCapBegin;
	glm::vec2 mUpperCapEnd;

	glm::vec2 mLowerCapBegin;
	glm::vec2 mLowerCapEnd;

public:
	ConeBuilder() : mRadius(0.4f), mHeight(0.4f), mSides(48) { };


	GLfloat getHeight()
	{
		return mHeight;
	}

	ConeBuilder& radius(GLfloat radius) {
		mRadius = radius;
		return *this;
	};

	ConeBuilder& height(GLfloat height) {
		mHeight = height;
		return *this;
	};

	ConeBuilder& sides(GLuint sides) {
		mSides = sides;
		return *this;
	};


	ConeBuilder& upperCap(glm::vec2 begin, glm::vec2 end) {
		mUpperCapBegin = begin;
		mUpperCapEnd = end;
		return *this;
	};

	ConeBuilder& lowerCap(glm::vec2 begin, glm::vec2 end) {
		mLowerCapBegin = begin;
		mLowerCapEnd = end;
		return *this;
	};

	std::shared_ptr<Mesh> build();
};

#endif //GLPRESS_ConeBuilderBUILDER_H

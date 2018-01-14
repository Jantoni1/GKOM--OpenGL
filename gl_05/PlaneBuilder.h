#pragma once
#include <memory>
#include "Mesh.h"

class PlaneBuilder
{
public:

	int rotation;
	GLfloat mHeight;
	GLfloat mWidth;
	GLfloat mDepth;

	std::shared_ptr<Mesh> build();
	PlaneBuilder(GLfloat height, GLfloat width, GLfloat depth, int pRotation) : mHeight(height), mWidth(width), mDepth(depth), rotation(pRotation)
	{};
	~PlaneBuilder();
};


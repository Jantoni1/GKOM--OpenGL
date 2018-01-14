#define GLEW_STATIC
#include <GL/glew.h>
#include "shprogram.h"
#include <GLFW/glfw3.h>
#include <SOIL.h>
#include <iostream>
#include "CubeBuilder.h"
#include "Cylinder.h"
#include "HalfCylinder.h"
#include "ConeBuilder.h"
#include "PlaneBuilder.h"
using namespace std;
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const GLuint WIDTH = 800, HEIGHT = 800;

float lastX = 400, lastY = 300;

float yaw = -115.0f;
float pitch = -5.0f;
float speed = 2.5f;
float sensitivity = 0.1f;
float zoom = 45.0f;
bool firstMouse = true;

glm::vec3 lightPos(0.5f, 0.0f, 1.8f);

glm::vec3 startPos = glm::vec3(0.5f, 0.0f, 1.3f);
glm::vec3 cameraPos = glm::vec3(0.5f, 0.0f, 1.3f);

glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.05f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;
	glm::vec3 front;
	front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	front.y = sin(glm::radians(pitch));
	front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	cameraFront = glm::normalize(front);

}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

GLuint LoadMipmapTexture(GLuint texId, const char* fname)
{
	int width, height;
	unsigned char* image = SOIL_load_image(fname, &width, &height, 0, SOIL_LOAD_RGB);
	if (image == nullptr)
	{
		throw exception(fname);
	}


	GLuint texture;
	glGenTextures(1, &texture);

	glActiveTexture(texId);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}

ostream& operator<<(ostream& os, const glm::mat4& mx)
{
	for (int row = 0; row < 4; ++row)
	{
		for (int col = 0; col < 4; ++col)
			cout << mx[row][col] << ' ';
		cout << endl;
	}
	return os;
}

int main()
{
	if (glfwInit() != GL_TRUE)
	{
		cout << "GLFW initialization failed" << endl;
		return -1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	try
	{
		GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "GKOM - OpenGL 05", nullptr, nullptr);
		if (window == nullptr)
			throw exception("GLFW window not created");
		glfwMakeContextCurrent(window);
		glfwSetKeyCallback(window, key_callback);


		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK)
			throw exception("GLEW Initialization failed");

		glViewport(0, 0, WIDTH, HEIGHT);

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		glfwSetCursorPosCallback(window, mouse_callback);
		
		glm::vec3 front;
		front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
		front.y = sin(glm::radians(pitch));
		front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
		cameraFront = glm::normalize(front);

		//glEnable(GL_CULL_FACE);
		//glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		// Let's check what are maximum parameters counts
		GLint nrAttributes;
		glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
		//cout << "Max vertex attributes allowed: " << nrAttributes << std::endl;
		glGetIntegerv(GL_MAX_TEXTURE_COORDS, &nrAttributes);
		//cout << "Max texture coords allowed: " << nrAttributes << std::endl;

		// Build, compile and link shader program

		ShaderProgram theProgram("gl_05.vert", "gl_06.frag");

		ShaderProgram programForHalfCylinder("gl_06.vert", "gl_06.frag");

		ShaderProgram skyBoxProgram("gl_05.vert", "gl_05.frag");

		ShaderProgram lampShader("lamp.vert", "lamp.frag");

		//theProgram.setInt("material.diffuse", 1);
		//theProgram.setInt("material.specular", 1);

		//programForHalfCylinder.setInt("material.diffuse", 1);
		//programForHalfCylinder.setInt("material.specular", 1);

		static GLfloat floormove = 0.0f;
		static GLfloat delay = 0.0f;

		static double parameter = 0.3;
		GLfloat hole = 0.05f;
		GLfloat additionalContainerOffset = 0.5f;
		static GLfloat halvesHeight = 0.1f;
		
		float cubeHeightRatio = 4.0f;
		float width = 1.2f;

		CubeBuilder cubebuilder;
		cubebuilder.height(width*cubeHeightRatio * parameter);
		cubebuilder.depth(0.12 * parameter);
		cubebuilder.width(width * parameter);
		cubebuilder.rightMap(glm::vec2(0.5f, -5.0 * cubebuilder.getHeight()/ parameter/ 1.5), glm::vec2(1.0f, 1.0f));
		cubebuilder.leftMap(glm::vec2(0.5f, -5.0 * cubebuilder.getHeight() / parameter/1.5), glm::vec2(1.0f, 1.0f));
		cubebuilder.frontMap(glm::vec2(0.0f, floormove + 0.0f), glm::vec2(0.5f, floormove + 2.0f));
		cubebuilder.backMap(glm::vec2(0.0f, floormove + 0.0f), glm::vec2(0.5f, floormove + 2.0f));
		cubebuilder.topMap(glm::vec2(0.0f, floormove + 0.0f), glm::vec2(0.5f, floormove + 2.0f  * cubebuilder.getDepth() / cubebuilder.getHeight()));
		cubebuilder.bottomMap(glm::vec2(0.0f, floormove + 0.0f), glm::vec2(0.5f, floormove + 2.0f * cubebuilder.getDepth() / cubebuilder.getHeight()));
		cubebuilder.setUsage(GL_DYNAMIC_DRAW);

		CubeBuilder miningContainer;
		miningContainer.height(2.0 * parameter);
		miningContainer.depth(1.2 * parameter);
		miningContainer.width(1.5*parameter);
		miningContainer.frontMap(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
		miningContainer.backMap(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
		miningContainer.rightMap(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
		miningContainer.leftMap(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
		miningContainer.topMap(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
		miningContainer.bottomMap(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
		miningContainer.setRemoveTop(true);

		CubeBuilder fill;
		fill.height(0.02 * parameter);
		fill.depth(miningContainer.getDepth());
		fill.width((1.5 - width)*parameter / 2);
		fill.frontMap(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
		fill.backMap(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
		fill.rightMap(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
		fill.leftMap(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
		fill.topMap(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
		fill.bottomMap(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f));

		CubeBuilder fill2;
		fill2.height(0.2 * parameter);
		fill2.depth(miningContainer.getDepth() /2 - cubebuilder.getDepth()/2 - (halvesHeight) * parameter);
		fill2.width(cubebuilder.getWidth());
		fill2.frontMap(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
		fill2.backMap(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
		fill2.rightMap(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
		fill2.leftMap(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
		fill2.topMap(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
		fill2.bottomMap(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f));

		CubeBuilder fill3;
		fill3.height(0.2 * parameter);
		fill3.depth(miningContainer.getDepth()/2 - cubebuilder.getDepth() / 2 - hole * parameter);
		fill3.width(cubebuilder.getWidth());
		fill3.frontMap(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
		fill3.backMap(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
		fill3.rightMap(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
		fill3.leftMap(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
		fill3.topMap(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
		fill3.bottomMap(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f));

		CubeBuilder bakePart;
		bakePart.height(1.0 * parameter);
		bakePart.depth(0.12 * parameter);
		bakePart.width(width * parameter);
		bakePart.rightMap(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
		bakePart.leftMap(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
		bakePart.frontMap(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
		bakePart.backMap(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
		bakePart.topMap(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
		bakePart.bottomMap(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f));


		Cylinder mainCylinder, drill, joint;
		mainCylinder.radius(0.11 * parameter);
		mainCylinder.height(0.5 * parameter);
		mainCylinder.wrap(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.5f));
		mainCylinder.upperCap(glm::vec2(0.0f, 0.0f), glm::vec2(0.5f, 0.5f));
		mainCylinder.lowerCap(glm::vec2(0.0f, 0.5f), glm::vec2(1.0f, 0.5f));

		joint.radius(0.08 * parameter);
		joint.height(0.5 * parameter);
		joint.wrap(glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 0.5f));
		joint.upperCap(glm::vec2(0.0f, 0.5f), glm::vec2(0.5f, 1.0f));
		joint.lowerCap(glm::vec2(0.5f, 0.5f), glm::vec2(1.0f, 1.0f));
		joint.sides(6);

		drill.radius(0.03 * parameter);
		drill.height(0.9 * parameter);
		drill.wrap(glm::vec2(0.0f, 0.0f), glm::vec2(0.4f, 0.4f));
		drill.upperCap(glm::vec2(0.5, 0.5f), glm::vec2(1.0f, 1.0f));
		drill.lowerCap(glm::vec2(0.0f, 0.5f), glm::vec2(0.5f, 1.0f));
		drill.sides(8);

		GLuint tmpSides = 16;
		GLuint tmpActualSides = 12;

		double bakeHeight = 2.0;
		Cylinder bake_cylinder;
		bake_cylinder.radius(1.0 * parameter);
		bake_cylinder.height(bakeHeight * parameter);
		bake_cylinder.wrap(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
		bake_cylinder.upperCap(glm::vec2(0.0f, 0.0f), glm::vec2(0.5f, 0.5f));
		bake_cylinder.lowerCap(glm::vec2(0.0f, 0.5f), glm::vec2(1.0f, 0.5f));
		bake_cylinder.sides(tmpSides);
		bake_cylinder.effectiveSides(tmpActualSides);

		Cylinder bake_door;
		bake_door.radius(1.0 * parameter);
		bake_door.height(bakeHeight / 3  * parameter);
		bake_door.wrap(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.5f));
		bake_door.upperCap(glm::vec2(0.0f, 0.0f), glm::vec2(0.5f, 0.5f));
		bake_door.lowerCap(glm::vec2(0.0f, 0.5f), glm::vec2(1.0f, 0.5f));
		bake_door.sides(tmpSides);
		bake_door.effectiveSides(tmpSides - tmpActualSides);
		bake_door.includeTop(false);
		bake_door.includeBot(false);

		HalfCylinder halfCylinder;
		halfCylinder.height(halvesHeight * parameter);
		halfCylinder.radius(0.2 * parameter);
		halfCylinder.wrap(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.08f));
		halfCylinder.upperCap(glm::vec2(0.5, 0.5f), glm::vec2(1.0f, 0.75f));
		halfCylinder.lowerCap(glm::vec2(0.0f, 0.5f), glm::vec2(0.5f, 0.75f));

		HalfCylinder halfCylinder1;
		halfCylinder1.height(halvesHeight * parameter);
		halfCylinder1.radius(0.2 * parameter);
		halfCylinder1.setPart(false);
		halfCylinder1.wrap(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.08f));
		halfCylinder1.upperCap(glm::vec2(0.5, 0.75f), glm::vec2(1.0f, 1.0f));
		halfCylinder1.lowerCap(glm::vec2(0.0f, 0.75f), glm::vec2(0.5f, 1.0f));

		ConeBuilder cone;
		cone.height(0.05);
		cone.lowerCap(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
		cone.radius(0.03 * parameter);
		cone.sides(8);

		CubeBuilder cube;
		cube.height(0.01f);
		cube.width(0.01f);
		cube.depth(0.01f);


		GLfloat skyBoxHeight = 30.0f;
		GLfloat skyBoxWidth= 30.0f;
		GLfloat skyBoxDepth = 30.0f;

		PlaneBuilder plane1(skyBoxHeight, skyBoxWidth, skyBoxDepth, 0);

		PlaneBuilder plane2(skyBoxHeight, skyBoxWidth, skyBoxDepth, 1);

		PlaneBuilder plane3(skyBoxHeight, skyBoxWidth, skyBoxDepth, 2);

		PlaneBuilder plane4(skyBoxHeight, skyBoxWidth, skyBoxDepth, 3);

		PlaneBuilder plane5(skyBoxHeight, skyBoxWidth, skyBoxDepth, 4);

		PlaneBuilder plane6(skyBoxHeight, skyBoxWidth, skyBoxDepth, 5);


		std::shared_ptr<Mesh> mesh = mainCylinder.build();
		std::shared_ptr<Mesh> mesh1 = joint.build();
		std::shared_ptr<Mesh> mesh2 = drill.build(); 
		std::shared_ptr<Mesh> exp = halfCylinder.build();
		std::shared_ptr<Mesh> exp1 = halfCylinder1.build();
		std::shared_ptr<Mesh> coneDrill = cone.build();
		std::shared_ptr<Mesh> container = miningContainer.build();
		std::shared_ptr<Mesh> filler1 = fill.build();
		std::shared_ptr<Mesh> filler2 = fill.build();
		std::shared_ptr<Mesh> filler3 = fill2.build();
		std::shared_ptr<Mesh> filler4 = fill3.build();
		std::shared_ptr<Mesh> filler5;
		std::shared_ptr<Mesh> filler6;
		std::shared_ptr<Mesh> skyBox1 = plane1.build();
		std::shared_ptr<Mesh> skyBox2 = plane2.build();
		std::shared_ptr<Mesh> skyBox3 = plane3.build();
		std::shared_ptr<Mesh> skyBox4 = plane4.build();
		std::shared_ptr<Mesh> skyBox5 = plane5.build();
		std::shared_ptr<Mesh> skyBox6 = plane6.build();
		std::shared_ptr<Mesh> bake = bake_cylinder.build();
		std::shared_ptr<Mesh> innerTable = bakePart.build();
		bake_door.wrap(glm::vec2(0.0f, 0.67f), glm::vec2(1.0f, 1.0f));
		std::shared_ptr<Mesh> door1 = bake_door.build();
		bake_door.wrap(glm::vec2(0.0f, 0.0f / 2.0f), glm::vec2(1.0f, 0.33));
		std::shared_ptr<Mesh> door3 = bake_door.build();
		bake_door.wrap(glm::vec2(0.0f, 0.33f), glm::vec2(1.0f, 0.67f));
		std::shared_ptr<Mesh> door2 = bake_door.build();
		bake_cylinder.height(bake_cylinder.getHeight()* 0.95);
		bake_cylinder.radius(bake_cylinder.getRadius()* 0.95);
		bake_cylinder.normals(true);
		std::shared_ptr<Mesh> innerBake = bake_cylinder.build();
		bake_cylinder.height(bake_cylinder.getHeight()/ 0.95);
		bake_cylinder.radius(bake_cylinder.getRadius()/ 0.95);

		std::shared_ptr<Mesh> light = cube.build();

		cubebuilder.set4Sides();
		std::shared_ptr<Mesh> mesh3 = cubebuilder.build();
		cubebuilder.set2Sides();
		std::shared_ptr<Mesh> mesh2115 = cubebuilder.build();

		// Set up vertex data 
		GLfloat vertices[] = {
			// coordinates			// color			// texture
			0.25f,  0.5f,  0.0f,	1.0f, 0.0f, 0.0f,	1.0f,  0.0f,
			-0.75f,  0.5f,  0.0f,	0.0f, 1.0f, 0.0f,	0.0f,  0.0f,
			-0.25f, -0.5f,  0.0f,	0.0f, 0.0f, 1.0f,	0.0f,  1.0f,
			0.75f, -0.5f,  0.0f,	1.0f, 0.0f, 1.0f,	1.0f,  1.0f,
		};

		GLuint indices[] = {
			0, 1, 2,
			0, 2, 3,
		};

		GLuint VBO, EBO, VAO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		// vertex geometry data
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		// vertex color data
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		// vertex texture coordinates
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

		glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)

							  // Set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// Set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// prepare textures
		GLuint texture2 = LoadMipmapTexture(GL_TEXTURE2, "rock.jpg");

		GLuint texture0 = LoadMipmapTexture(GL_TEXTURE0, "mainCylinder.jpg");

		GLuint texture1 = LoadMipmapTexture(GL_TEXTURE1, "dullMetal.jpg");

		GLuint texture3 = LoadMipmapTexture(GL_TEXTURE3, "podloga.jpg");

		GLuint texture4= LoadMipmapTexture(GL_TEXTURE4, "mainContainer.jpg");
		
		GLuint texture5 = LoadMipmapTexture(GL_TEXTURE5, "topmou.jpg");

		GLuint texture6 = LoadMipmapTexture(GL_TEXTURE6, "botmou.jpg");

		GLuint texture7 = LoadMipmapTexture(GL_TEXTURE7, "rightmou.jpg");

		GLuint texture8 = LoadMipmapTexture(GL_TEXTURE8, "leftmou.jpg");

		GLuint texture9 = LoadMipmapTexture(GL_TEXTURE9, "frontmou.jpg");

		GLuint texture10 = LoadMipmapTexture(GL_TEXTURE10, "backmou.jpg");

		GLuint texture11 = LoadMipmapTexture(GL_TEXTURE11, "piec3.jpg");




		//GLuint texture1 = LoadMipmapTexture(GL_TEXTURE1, "c.jpg");

		glUseProgram(programForHalfCylinder.get_programID());

		//glActiveTexture(GL_TEXTURE2);
		//glBindTexture(GL_TEXTURE_2D, texture2);


		glUseProgram(theProgram.get_programID());

		//glActiveTexture(GL_TEXTURE0);

		//glBindTexture(GL_TEXTURE_2D, texture0);

		//glActiveTexture(GL_TEXTURE1);

		//glBindTexture(GL_TEXTURE_2D, texture1);

		glm::mat4 projection;
		projection = glm::perspective(glm::radians(45.0f), float(WIDTH / HEIGHT), 0.1f, 100.0f);


		GLuint perspective = glGetUniformLocation(programForHalfCylinder.get_programID(), "projection");
		glUniformMatrix4fv(perspective, 1, GL_FALSE, glm::value_ptr(projection));

		perspective = glGetUniformLocation(theProgram.get_programID(), "projection");
		glUniformMatrix4fv(perspective, 1, GL_FALSE, glm::value_ptr(projection));

		// main event loop
		while (!glfwWindowShouldClose(window))
		{
			// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
			glfwPollEvents();

			// Clear the colorbuffer
			glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			// Bind Textures using texture units




			theProgram.Use();



			glm::mat4 trans;
			static GLfloat rot_angle = 90.0f;
			/*trans = glm::rotate(trans, glm::radians(rot_angle), glm::vec3(1.0, 0.0, 0.0));*/

			static float cylinderDisplacement = 0.0f;
			static bool cylinderLock = false;
			static bool cylinderLock1 = false;

			static int slowdown = 0, minorSlowdown = 0;;
			static float disp = 0.0f;
			static bool downwards = false;
			static bool upwards = false;
			static bool broken = false;
			static bool drilling = false;
			static bool canGoDeeper = false;
			static bool createnew = false;
			static bool allowed = false;
			static bool blockDrill = false;
			static float time = 0.0f;
			static int drillOver = 1000.0f;
			static GLfloat hammerDisplacement = -0.3f;
			static GLfloat deepestHammerDisplacement = -0.3f - halvesHeight * parameter;
			static GLfloat stepForMain = 0.001f;
			static GLfloat drillMaxDisplacement = cubebuilder.getWidth()/2 - halfCylinder.getRadius();
			if (disp >= deepestHammerDisplacement && disp < hammerDisplacement && !blockDrill)
			{
				canGoDeeper = true;
			}
			else
			{
				canGoDeeper = false;
			}
			if(downwards && (disp > hammerDisplacement || (disp > deepestHammerDisplacement && broken && canGoDeeper)))
			{
				disp -= stepForMain;
			}
			else if ((disp - hammerDisplacement) < 0.00001f && !broken)
			{
				drilling = true;
			}
			if(upwards && disp < 0.0)
			{
				disp += stepForMain;

			}
			else {
				++slowdown;
			}

			static float minorDisp = 0.0f;
			static float step = 0.01f;
			static bool minorDown = true;

			static GLfloat move = 0.2f;
			static GLfloat displacementForCylinders = 0.0f + move;
			static GLfloat displacementmax1 = 0.0f + move;
			static GLfloat displacementmax2 = -0.7f;
			static GLfloat destinationDisplacement = 0.7f + move;
			static GLfloat displacementStep = 0.00072f;
			static bool bakeUp = false;
			static bool bakeDown = false;
			static GLfloat upperBound = bake_door.getHeight();
			static GLfloat lowerBound = 0.0f;
			static GLfloat moveStep = 0.00032f;
			static GLfloat doorTimer = 0.0f;
			static int counter = 0;
			static int result = 100;

			static bool moveForward;
			static GLfloat upperbound = bake_cylinder.getRadius()/2 + 0.045;
			static GLfloat step1 = upperbound * moveStep / upperBound;

			static GLfloat lowerbound = 0.0f;
			static GLfloat displacement1337 = 0.0f;
			if(bakeUp)
			{	
				doorTimer += moveStep;
				displacement1337 += step1;
				if(doorTimer >= upperBound)
				{
					++counter;
					displacement1337 = upperbound;
					doorTimer = upperBound;
					if(counter == result)
					{	
						counter = 0;
						bakeUp = false;
						bakeDown = true;
					}

				}
			}
			if(bakeDown)
			{
				displacement1337 -= step1;
				doorTimer -= moveStep;
				if(doorTimer <= lowerBound)
				{
					doorTimer = lowerBound;
					bakeDown = false;
				}
			}

			if(minorSlowdown == 5) {
				minorSlowdown = 0;
				minorDisp += minorDown ? -step : step;
				if(minorDisp <= -0.15f)
				{
					minorDisp = -0.15f;
					minorDown = false;
				}
				if(minorDisp >= 0.0f)
				{
					minorDisp = 0.0f;
					minorDown = true;
				}
			}
			else {
				++minorSlowdown;
			}
			if(cylinderDisplacement <= drillMaxDisplacement && broken && allowed)
			{
				cylinderDisplacement += 0.001f;
			}
			else if(broken && cylinderDisplacement >= drillMaxDisplacement && disp > hammerDisplacement && createnew)
			{
				allowed = false;
				exp._Swap(filler5);
				exp1._Swap(filler6);
				if (exp.get() != nullptr)
				{
					exp.reset();
					exp1.reset();
				}
				createnew = false;
				exp = halfCylinder.build();
				exp1 = halfCylinder1.build();
				cylinderDisplacement = 0.0f;
				displacementForCylinders = displacementmax2;
				bakeUp = true;
				bakeDown = false;
			} 

			if(broken && !allowed)
			{
				if (displacementForCylinders < displacementmax1)
				{
					blockDrill = true;
					displacementForCylinders += displacementStep;
					canGoDeeper = false;
				}
				else
				{
					blockDrill = false;
					broken = false;
				}
			}

			static GLfloat angle = 90.0f;
			glm::mat4 displacement, displacement1, displacement2, displacement3, displacement4, displacement5, rotation, drillDisplacement, containerDisplacement;
			glm::mat4 fillDispl1, fillDispl2, fillDispl3, fillDispl4, oldHalf, oldHalf1;
			glm::mat4 bakeCylinder, bakeDoor1, bakeDoor2, bakeDoor3, bakeTable;
			static GLfloat displ = 0.2f;
			static GLfloat mainCylinderOffset = displ + mainCylinder.getHeight() / 2 + joint.getHeight();
			static GLfloat jointOffset = displ + joint.getHeight() / 2;
			GLfloat drillOffset = -drill.getHeight() * 1 / 2 + displ + hammerDisplacement - halvesHeight * parameter;
			displacement = glm::translate( displacement,  glm::vec3(0.0f, mainCylinderOffset + disp, move)); //MainCylinder
			displacement1 = glm::translate(displacement1, glm::vec3(0.0f,  jointOffset + disp, move));	//Joint
			displacement2 = glm::translate(displacement2, glm::vec3(0.0f,  jointOffset - drill.getHeight() / 2 + displ + minorDisp + disp, move)); //DRILL
			displacement3 = glm::translate(displacement3, glm::vec3(0.0f, drillOffset - halvesHeight * parameter /2 - cubebuilder.getDepth()/2 - cone.getHeight() / 1, 0.0f)); // TABLE
			displacement4 = glm::translate(displacement4, glm::vec3(0.0f + cylinderDisplacement, drillOffset - cone.getHeight() / 1, displacementForCylinders)); //HALF
			displacement5 = glm::translate(displacement5, glm::vec3(0.0f - cylinderDisplacement, drillOffset - cone.getHeight() / 1, displacementForCylinders)); //HALF
			drillDisplacement = glm::translate(drillDisplacement, glm::vec3(0.0f, jointOffset - drill.getHeight()  - cone.getHeight() / 2+ displ + minorDisp + disp, move));
			glm::vec3 containerDispl = glm::vec3(0.0f, drillOffset - halvesHeight * parameter / 2 - cone.getHeight() / 1,
				-miningContainer.getHeight() / 2 - cubebuilder.getHeight() / 2 + additionalContainerOffset);
			containerDisplacement = glm::translate(containerDisplacement, containerDispl);
			fillDispl1 = glm::translate(fillDispl1, containerDispl + glm::vec3(cubebuilder.getWidth() / 2 + fill.getWidth()/2 - 0.002, 0.0f, miningContainer.getHeight() / 2 - fill.getHeight() / 2));
			fillDispl2 = glm::translate(fillDispl2, containerDispl + glm::vec3(-cubebuilder.getWidth() / 2 - fill.getWidth() / 2 + 0.002, 0.0f, miningContainer.getHeight() / 2 - fill.getHeight() / 2));
			fillDispl3 = glm::translate(fillDispl3, containerDispl + glm::vec3(0.0f, miningContainer.getDepth()/2 - fill2.getDepth() / 2 - 0.002, miningContainer.getHeight() / 2 - fill2.getHeight() / 2));
			fillDispl4 = glm::translate(fillDispl4, containerDispl + glm::vec3(0.0f, -miningContainer.getDepth() / 2 + fill3.getDepth() / 2 + 0.002,  miningContainer.getHeight() / 2 - fill3.getHeight() / 2 ));
			oldHalf = glm::translate(oldHalf, glm::vec3(0.0f + drillMaxDisplacement, drillOffset - cone.getHeight() / 1, destinationDisplacement + displacementForCylinders)); //HALF
			oldHalf1 = glm::translate(oldHalf1, glm::vec3(0.0f - drillMaxDisplacement, drillOffset - cone.getHeight() / 1, destinationDisplacement + displacementForCylinders)); //HALF
			bakeCylinder = glm::translate(bakeCylinder, glm::vec3(0.0f, drillOffset - halvesHeight * parameter / 2 - cone.getHeight() / 1
				, cubebuilder.getHeight()/2 + bake_cylinder.getRadius() / 2 + move));
			bakeTable = glm::translate(bakeCylinder, glm::vec3(0.0f, -bakePart.getDepth() / 2, -displacement1337));
			bakeDoor1 = glm::translate(bakeCylinder, glm::vec3(0.0f, bake_cylinder.getHeight()/3, 0.0f));
			bakeDoor2 = glm::translate(bakeCylinder, glm::vec3(0.0f, doorTimer, -upperBound * 0.0001));
			bakeDoor3 = glm::translate(bakeCylinder, glm::vec3(0.0f, -bake_cylinder.getHeight()/3, 0.0f));
			bakeCylinder = glm::rotate(bakeCylinder, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			bakeDoor1 = glm::rotate(bakeDoor1, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			bakeDoor2 = glm::rotate(bakeDoor2, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			bakeDoor3 = glm::rotate(bakeDoor3, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));



			glm::mat4 p1, p2, p3, p4, p5, p6;
			GLfloat d = 0.01f;
			p1 = glm::translate(p1, startPos + glm::vec3(0.0f, plane1.mDepth / 2 - d, 0.0f));
			p2 = glm::translate(p2, startPos + glm::vec3(0.0f, -plane1.mDepth / 2 + d, 0.0f));
			p3 = glm::translate(p3, startPos + glm::vec3(0.0f, 0.0f,  plane1.mHeight / 2 - d));
			p4 = glm::translate(p4, startPos + glm::vec3(0.0f, 0.0f, -plane1.mHeight / 2 + d));
			p5 = glm::translate(p5, startPos + glm::vec3( plane1.mWidth/2 - d, 0.0f, 0.0f));
			p6 = glm::translate(p6, startPos + glm::vec3(-plane1.mWidth / 2 + d, 0.0f, 0.0f));


			static float deltaTime = 0.0f;	// Time between current frame and last frame
			static float lastFrame = 0.0f; // Time of last frame
			float currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;
			static float cameraSpeed = 0.0f;
			static float radius = 0.5;
			float camX = sin(glfwGetTime()) * radius;
			float camZ = cos(glfwGetTime()) * radius;
			glm::mat4 view;
			//view = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));

			cameraSpeed = 2.5f * deltaTime; // adjust accordingly

			static bool lockDown = false;

			static int cooldown = 1;
			cooldown--;
			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			{
				cameraPos += cameraSpeed * cameraFront;
				if(lockDown)
					lightPos += cameraSpeed * cameraFront;
			}
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			{
				cameraPos -= cameraSpeed * cameraFront;
				if (lockDown)
					lightPos -= cameraSpeed * cameraFront;
			}
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			{
				cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
				if (lockDown)
					lightPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

			}
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			{
				cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
				if (lockDown)
				{
					lightPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
				}
			}
			if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) 
			{
				if (cooldown <= 0)
				{
					step = (step - 0.001f) >= 0.0f ? step - 0.001f : step >= 0 ? 0.0f : step;
					cooldown = 20;
				}
			}
			if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
			{
				if(cooldown <= 0)
				{
					step = (step + 0.001f) <= 0.1f ? step + 0.001f : step;
					cooldown = 20;
				}
			}
			if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_P) != GLFW_PRESS)
			{
				if (drilling && displacementForCylinders >= displacementmax1)
				{
					time += step * step * 2000;
					if (time >= drillOver)
					{
						drillMaxDisplacement = (cubebuilder.getWidth() / 2 - halfCylinder.getRadius()) * step /  0.1f;
						time = 0.0f;
						allowed = true;
						drilling = false;
						broken = true;
						canGoDeeper = true;
						createnew = true;
					}
				}

				downwards = true;
				upwards = false;
			}
			else
			{
				downwards = false;
			}
			if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_O) != GLFW_PRESS)
			{	
				upwards = true;
				downwards = false;
			}
			else
			{
				upwards = false;
			}
			if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
			{	
				lightPos = cameraPos;
				lockDown = false;
			}
			if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
			{
				lockDown = false;
			}
			else if(glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
			{
				lockDown = true;
			}
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			
			static GLfloat lightType = 0.0f;

			if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
			{
				lightType -= 0.001f;
				if (lightType < -1.0f)
				{
					lightType = 0.0f;
				}
			}
			else if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
			{
				lightType += 0.001f;
				if (lightType > 3.0f)
				{
					lightType = 3.0f;
				}
			}
			static bool show = true;
			if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
			{
				show = true;
			}
			else if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
			{
				show = false;
			}
			
			//view = glm::lookAt(cameraPos, cameraFront, cameraUp);
			view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);


			delay += 0.001f;
			if(delay >= 0.001f )
			{
				if(broken && !canGoDeeper && !allowed)
				{
					floormove += delay;
					if (floormove >= 1.0f)
					{
						floormove = 0.0f;
					}
					delay = 0.0f;
					/*cubebuilder.frontMap(glm::vec2(0.0f, floormove + 0.0f), glm::vec2(0.5f, floormove + 2.0f));
					cubebuilder.backMap(glm::vec2(0.0f, floormove + 0.0f), glm::vec2(0.5f, floormove + 2.0f));
					cubebuilder.topMap(glm::vec2(0.0f, floormove + 0.0f), glm::vec2(0.5f, floormove + 2.0f  * cubebuilder.getDepth() / cubebuilder.getHeight()));
					cubebuilder.bottomMap(glm::vec2(0.0f, floormove + 0.0f), glm::vec2(0.5f, floormove + 2.0f * cubebuilder.getDepth() / cubebuilder.getHeight()));
					mesh3.reset();
					mesh3 = cubebuilder.build();*/
				}
				else
				{
					delay -= +0.0027f;
				}
				
			}
			
			static bool isSkyBox = 0.0f;

			rotation = glm::rotate(rotation, -glm::radians(angle), glm::vec3(0.0, 0.0, 1.0));
			

			GLuint camera = glGetUniformLocation(programForHalfCylinder.get_programID(), "view");
			glUniformMatrix4fv(camera, 1, GL_FALSE, glm::value_ptr(view));


			camera = glGetUniformLocation(theProgram.get_programID(), "view");
			glUniformMatrix4fv(camera, 1, GL_FALSE, glm::value_ptr(view));


			perspective = glGetUniformLocation(theProgram.get_programID(), "projection");
			glUniformMatrix4fv(perspective, 1, GL_FALSE, glm::value_ptr(projection));
			//rot_angle += 0.05f;
			//if (rot_angle >= 360.0f)
			//	rot_angle -= 360.0f;
			trans = glm::rotate(trans, -glm::radians(rot_angle), glm::vec3(1.0, 0.0, 0.0));
			//GLuint transformLoc = glGetUniformLocation(theProgram.get_programID(), "transform");
			//glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

			GLuint rotationLoc = glGetUniformLocation(theProgram.get_programID(), "model");
			glUniformMatrix4fv(rotationLoc, 1, GL_FALSE, glm::value_ptr(displacement));

			//theProgram.setMat4("transform", view);

			GLfloat ordinaryAmbient = 0.2f;
			GLfloat skyBoxAmbient = 1.0f;

			theProgram.Use();
			theProgram.setInt("material.diffuse", 0);
			theProgram.setInt("material.specular", 0);
			theProgram.setVec2("TexCoordShift", 0.0f, 0.0f);



			theProgram.setVec3("light.position", lightPos.x, lightPos.y, lightPos.z);
			theProgram.setVec3("viewPos", cameraPos.x, cameraPos.y, cameraPos.z);


			// light properties
			theProgram.setVec3("light.ambient", 0.2f + lightType, 0.2f + lightType, 0.2f + lightType);
			theProgram.setVec3("light.diffuse", 0.5f + lightType, 0.5f + lightType, 0.5f + lightType);
			theProgram.setVec3("light.specular", 1.0f + lightType, 1.0f + lightType, 1.0f + lightType);

			// material properties
			theProgram.setFloat("material.shininess", 1.0f);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture0);

			//glUniform1i(glGetUniformLocation(theProgram.get_programID(), "Texture1"), 1);

			displacement = displacement * trans;
			glUniformMatrix4fv(rotationLoc, 1, GL_FALSE, glm::value_ptr(displacement));
			mesh->draw();	//main

			displacement1 = displacement1 * trans;
			glUniformMatrix4fv(rotationLoc, 1, GL_FALSE, glm::value_ptr(displacement1));
			mesh1->draw();	//joint

			glBindTexture(GL_TEXTURE_2D, texture11);
			theProgram.setFloat("material.shininess", 0.8f);
			bakeCylinder = bakeCylinder * trans;
			glUniformMatrix4fv(rotationLoc, 1, GL_FALSE, glm::value_ptr(bakeCylinder));
			bake->draw();	

			bakeDoor1 = bakeDoor1 * trans;
			glUniformMatrix4fv(rotationLoc, 1, GL_FALSE, glm::value_ptr(bakeDoor1));

			door1->draw();

			bakeDoor2 = bakeDoor2 * trans;
			glUniformMatrix4fv(rotationLoc, 1, GL_FALSE, glm::value_ptr(bakeDoor2));

			door2->draw();

			bakeDoor3 = bakeDoor3 * trans;
			glUniformMatrix4fv(rotationLoc, 1, GL_FALSE, glm::value_ptr(bakeDoor3));

			door3->draw();

			skyBoxProgram.Use();
			skyBoxProgram.setFloat("ambientIntensity", 0.25f);
			glm::mat4 neutral = glm::lookAt(startPos, startPos + cameraFront, cameraUp);
			camera = glGetUniformLocation(skyBoxProgram.get_programID(), "view");
			glUniformMatrix4fv(camera, 1, GL_FALSE, glm::value_ptr(view));
			perspective = glGetUniformLocation(skyBoxProgram.get_programID(), "projection");
			glUniformMatrix4fv(perspective, 1, GL_FALSE, glm::value_ptr(projection));
			GLuint kozak = glGetUniformLocation(skyBoxProgram.get_programID(), "model");
			glUniformMatrix4fv(kozak, 1, GL_FALSE, glm::value_ptr(bakeCylinder));
			innerBake->draw();
			bakeTable = bakeTable * trans;

			glUniformMatrix4fv(kozak, 1, GL_FALSE, glm::value_ptr(bakeTable));
			innerTable->draw();

			glUniformMatrix4fv(camera, 1, GL_FALSE, glm::value_ptr(neutral));


			theProgram.Use();

			

			theProgram.Use();

			theProgram.setFloat("material.shininess", 1.0f);

			glBindTexture(GL_TEXTURE_2D, texture4);

			containerDisplacement *= trans;
			glUniformMatrix4fv(rotationLoc, 1, GL_FALSE, glm::value_ptr(containerDisplacement));
			container->draw(); // containerMain
			fillDispl1 *= trans;
			glUniformMatrix4fv(rotationLoc, 1, GL_FALSE, glm::value_ptr(fillDispl1));
			filler1->draw(); // bocznyPrawy
			fillDispl2 *= trans;
			glUniformMatrix4fv(rotationLoc, 1, GL_FALSE, glm::value_ptr(fillDispl2));
			filler2->draw(); // bocznyLewy
			fillDispl3 *= trans;
			glUniformMatrix4fv(rotationLoc, 1, GL_FALSE, glm::value_ptr(fillDispl3));
			filler3->draw(); // gorny
			fillDispl4 *= trans;
			glUniformMatrix4fv(rotationLoc, 1, GL_FALSE, glm::value_ptr(fillDispl4));
			filler4->draw(); // dolny


			theProgram.setFloat("material.shininess", 0.6f);

			glBindTexture(GL_TEXTURE_2D, texture1);
			displacement2 *= trans;
			glUniformMatrix4fv(rotationLoc, 1, GL_FALSE, glm::value_ptr(displacement2));
			mesh2->draw();	//wiertlo
			drillDisplacement *= trans;
			glUniformMatrix4fv(rotationLoc, 1, GL_FALSE, glm::value_ptr(drillDisplacement));
			coneDrill->draw();

			theProgram.setFloat("material.shininess", 0.8f);

			theProgram.setVec2("TexCoordShift", 0.0f , floormove);

			glBindTexture(GL_TEXTURE_2D, texture3);
			displacement3 *= trans;
			glUniformMatrix4fv(rotationLoc, 1, GL_FALSE, glm::value_ptr(displacement3));
			mesh3->draw();	//stol

			theProgram.setVec2("TexCoordShift", 0.0f, 0.0f);

			mesh2115->draw();
			programForHalfCylinder.Use();

			programForHalfCylinder.setInt("material.diffuse", 0);
			programForHalfCylinder.setInt("material.specular", 0);
			programForHalfCylinder.setVec3("light.position", lightPos.x, lightPos.y, lightPos.z);
			programForHalfCylinder.setVec3("viewPos", cameraPos.x, cameraPos.y, cameraPos.z);

			// light properties
			programForHalfCylinder.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
			programForHalfCylinder.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
			programForHalfCylinder.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

			// material properties
			programForHalfCylinder.setFloat("material.shininess", 0.4f);

			camera = glGetUniformLocation(programForHalfCylinder.get_programID(), "view");
			glUniformMatrix4fv(camera, 1, GL_FALSE, glm::value_ptr(view));
			perspective = glGetUniformLocation(programForHalfCylinder.get_programID(), "projection");
			glUniformMatrix4fv(perspective, 1, GL_FALSE, glm::value_ptr(projection));
			
			
			glBindTexture(GL_TEXTURE_2D, texture2);
			glUniform1i(glGetUniformLocation(programForHalfCylinder.get_programID(), "Texture"), 1);
			rotationLoc = glGetUniformLocation(programForHalfCylinder.get_programID(), "model");

			displacement4 *= trans * rotation;
			glUniformMatrix4fv(rotationLoc, 1, GL_FALSE, glm::value_ptr(displacement4));
			//glUniformMatrix4fv(rot, 1, GL_FALSE, glm::value_ptr(rotation));
			//glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
			exp->draw();	//chyba prawy
			displacement5 *= trans * rotation;
			glUniformMatrix4fv(rotationLoc, 1, GL_FALSE, glm::value_ptr(displacement5));
			exp1->draw();	//chyba lewy

			if(filler5.get() != nullptr && filler6.get() != nullptr)
			{
				oldHalf *= trans * rotation;
				glUniformMatrix4fv(rotationLoc, 1, GL_FALSE, glm::value_ptr(oldHalf));
				filler5->draw();
				oldHalf1 *= trans * rotation;
				glUniformMatrix4fv(rotationLoc, 1, GL_FALSE, glm::value_ptr(oldHalf1));
				filler6->draw();
			}

			if(show)
			{
				lampShader.Use();
				lampShader.setMat4("projection", projection);
				lampShader.setMat4("view", view);
				glm::mat4 model = glm::mat4();
				model = glm::translate(model, lightPos);
				//model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
				lampShader.setMat4("model", model);
				light->draw();
			}


			skyBoxProgram.Use();
			//theProgram.setFloat("ambientStrength", skyBoxAmbient);
			//theProgram.setFloat("skyBox", 1.0f);
			skyBoxProgram.setFloat("ambientIntensity", 1.0f);
			rotationLoc = glGetUniformLocation(skyBoxProgram.get_programID(), "model");
			perspective = glGetUniformLocation(skyBoxProgram.get_programID(), "projection");
			glUniformMatrix4fv(perspective, 1, GL_FALSE, glm::value_ptr(projection));


			//glUniform1i(glGetUniformLocation(theProgram.get_programID(), "Texture"), 5);

			glBindTexture(GL_TEXTURE_2D, texture5);

			p1 *= trans;
			glUniformMatrix4fv(rotationLoc, 1, GL_FALSE, glm::value_ptr(p1));
			skyBox1->draw();


			//glUniform1i(glGetUniformLocation(theProgram.get_programID(), "Texture"), 6);

			//glActiveTexture(GL_TEXTURE6);
			glBindTexture(GL_TEXTURE_2D, texture6);

			p2 *= trans;
			glUniformMatrix4fv(rotationLoc, 1, GL_FALSE, glm::value_ptr(p2));
			skyBox2->draw();


			//glUniform1i(glGetUniformLocation(theProgram.get_programID(), "Texture"), 1);

			//glActiveTexture(GL_TEXTURE7);
			glBindTexture(GL_TEXTURE_2D, texture7);

			p3 *= trans;
			glUniformMatrix4fv(rotationLoc, 1, GL_FALSE, glm::value_ptr(p3));
			skyBox3->draw();

			//glUniform1i(glGetUniformLocation(theProgram.get_programID(), "Texture"), 1);

			//glActiveTexture(GL_TEXTURE8);
			glBindTexture(GL_TEXTURE_2D, texture8);

			p4 *= trans;
			glUniformMatrix4fv(rotationLoc, 1, GL_FALSE, glm::value_ptr(p4));
			skyBox4->draw();

			glUniform1i(glGetUniformLocation(theProgram.get_programID(), "Texture"), 1);

			//glActiveTexture(GL_TEXTURE9);
			glBindTexture(GL_TEXTURE_2D, texture9);

			p5 *= trans;
			glUniformMatrix4fv(rotationLoc, 1, GL_FALSE, glm::value_ptr(p5));
			skyBox5->draw();

			glUniform1i(glGetUniformLocation(theProgram.get_programID(), "Texture"), 1);

			//glActiveTexture(GL_TEXTURE10);
			glBindTexture(GL_TEXTURE_2D, texture10);

			p6 *= trans;
			glUniformMatrix4fv(rotationLoc, 1, GL_FALSE, glm::value_ptr(p6));
			skyBox6->draw();




			// Swap the screen buffers
			glfwSwapBuffers(window);
		}
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}
	catch (exception ex)
	{
		cout << ex.what() << endl;
	}
	glfwTerminate();

	return 0;
}

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "tigl.h"
#include <glm/gtc/matrix_transform.hpp>

#include "FpsCam.h"
#include "ObjModel.h"
#include "Texture.h"
#include "Text/Text.h"
using tigl::Vertex;

#include "GameObject.h"
#include "PlayerComponent.h"
#include "CubeComponent.h"
#include "MoveToComponent.h"
#include "SpinComponent.h"
#include "EnemyComponent.h"

#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "opengl32.lib")

GLFWwindow* window;

void init();
void update();
void draw();
void addBackground();
void controlShaders();
void drawObjects();
void drawModels();
void addPlanets();
void loadTextures();


std::list<GameObject*> objects;
double lastFrameTime = 0;
GameObject* movingObject;
FpsCam* camera;
GameObject* player;
GameObject* backgroundBox;
Texture* textures[5];
std::vector<ObjModel*> models;
int moving = 0;
Text * textObject;

int main(void)
{
	if (!glfwInit())
		throw "Could not initialize glwf";
	window = glfwCreateWindow(1400, 800, "Galaxy!", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		throw "Could not initialize glwf";
	}
	glfwMakeContextCurrent(window);

	tigl::init();

	init();

	while (!glfwWindowShouldClose(window))
	{
		update();
		draw();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();


	return 0;
}


void init()
{
	glEnable(GL_DEPTH_TEST);

	addBackground();
	addPlanets();
	loadTextures();

	textObject = new Text();

	camera = new FpsCam(window);


	glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE)
			glfwSetWindowShouldClose(window, true);
	});

}

float x = 0;

void update()
{
	double currentFrameTime = glfwGetTime();
	double deltaTime = currentFrameTime - lastFrameTime;
	lastFrameTime = currentFrameTime;
	camera->update(window);
	for (auto& o : objects)
		o->update(deltaTime);

	x += 0.1f;
}

void draw()
{
	controlShaders();

	drawObjects();
	drawModels();

	glDisable(GL_DEPTH_TEST);
	for (int i = 0; i < 10; i++)
		textObject->draw("TEST", x, 100 + 32 * i);
}

void drawObjects()
{
	for (auto& o : objects)
	{
		if (o == backgroundBox)
		{
			textures[0]->bind(); //Bind galaxy texture for backgroundbox
			tigl::shader->enableColor(false);
			tigl::shader->enableTexture(true);
		}
		else
		{
			tigl::shader->enableColor(true);
			tigl::shader->enableTexture(false);
		}
		o->draw();
	}
}

void drawModels()
{
	for (auto model : models)
	{
		if (model->hasTexture())
		{
			if (model->getName() == "Moon")
			{
				//Static planet moon
				glm::mat4 scalingMatrix(1.0f); // for scaling the moon
				glm::vec3 movingVector(0, moving, 0); // for moving the moon
				scalingMatrix = glm::scale(scalingMatrix, glm::vec3(0.1f)); // for scaling the moon
				scalingMatrix = glm::translate(scalingMatrix, movingVector); //translate vector for moving
				//moving++;
				tigl::shader->setModelMatrix(scalingMatrix); // for scaling the moon
			}

			tigl::shader->enableColor(false);
			tigl::shader->enableTexture(true);
		}
		else
		{
			tigl::shader->enableColor(true);
			tigl::shader->enableTexture(false);
		}
		model->draw();
	}
}

void controlShaders()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glm::mat4 projection = glm::perspective(glm::radians(75.0f), viewport[2] / (float)viewport[3], 0.01f, 1000.0f);
	tigl::shader->setViewMatrix(camera->getMatrix());
	tigl::shader->setProjectionMatrix(projection);
	tigl::shader->setModelMatrix(glm::mat4(1.0f));
	tigl::shader->enableColor(true);
	glEnable(GL_DEPTH_TEST);
}

void addBackground()
{
	//Adding the galaxy bounding box
	backgroundBox = new GameObject();
	backgroundBox->position = glm::vec3(0, 0, 5);
	backgroundBox->addComponent(new CubeComponent(150));
	objects.push_back(backgroundBox);
}

void addPlanets()
{
	//Adding the moon
	models.push_back(new ObjModel("Resources/Moon_3D_Model/moon.obj", "Moon"));

	//Adding Astronaut
	models.push_back(new ObjModel("Resources/low-poly-astronaut/Astronaut.obj", "Astronaut"));
}

void loadTextures()
{
	textures[0] = new Texture("Resources/GalaxyTexture.png");
}

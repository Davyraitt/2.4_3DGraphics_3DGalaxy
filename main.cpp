#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "tigl.h"
#include <glm/gtc/matrix_transform.hpp>

#include "FpsCam.h"
#include "Components/ObjModel.h"
#include "Components/Texture.h"
#include "Components/CubeComponent.h"
#include "Components/GameObject.h"
#include "Text/Text.h"
using tigl::Vertex;

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
int movingRocket = 204;
bool rocketLaunching = true;
float rotationUfo = 1.0f;
Text* textObject;

//remove
float rotationtest = 1.0f;

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

	//DRAWING LINES
	// glLineWidth(12.0);
	// glBegin(GL_LINES);
	// glColor3f(1.0, 0.0, 0.0);
	// glVertex3f(0,0, 0);
	// glVertex3f(1000, 0, 1000);
	// glEnd();
	//


	glDisable(GL_DEPTH_TEST);

	textObject->draw("Project Galaxy By Davy", 15, 30);
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
				glm::vec3 movingVector(0, 0, 0); // for moving the moon
				scalingMatrix = glm::scale(scalingMatrix, glm::vec3(0.4f)); // for scaling the moon
				scalingMatrix = glm::translate(scalingMatrix, movingVector); //translate vector for moving
				//moving++;
				tigl::shader->setModelMatrix(scalingMatrix); // for scaling the moon
			}

			if (model->getName() == "RocketShip")
			{
				//Static planet moon
				glm::mat4 modelMatrix(1.0f); // for scaling the rocket
				glm::vec3 movingVector(0, 0, movingRocket); // for moving the rocket
				modelMatrix = glm::rotate(modelMatrix, 180.0f, glm::vec3(1));
				//Rotate the rocket, wasnt rendered correctly
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f)); // for scaling the rocket
				modelMatrix = glm::translate(modelMatrix, movingVector); //translate vector for moving
				if (rocketLaunching)
				{
					movingRocket ++;
					if (movingRocket >= 600)
					{
						rocketLaunching = false;
					}
				}

				if (!rocketLaunching)
				{
					movingRocket--;
					if (movingRocket <= 204)
					{
						rocketLaunching = true;
					}
				}
				tigl::shader->setModelMatrix(modelMatrix); // for scaling the rocket
			}

			if (model->getName() == "UFO")
			{
				//Static UFO
				glm::mat4 matrixUFO(1.0f); // for scaling the rocket
				glm::vec3 movingVector(-40, -20, -175); // for moving the rocket
				matrixUFO = glm::rotate(matrixUFO, rotationUfo, glm::vec3(1));
				//Rotate the rocket, wasnt rendered correctly
				matrixUFO = glm::scale(matrixUFO, glm::vec3(0.5f)); // for scaling the rocket
				matrixUFO = glm::translate(matrixUFO, movingVector); //translate vector for moving
				rotationUfo = rotationUfo + 0.006f;
				tigl::shader->setModelMatrix(matrixUFO); // for scaling the rocket
			}

			if (model->getName() == "Sun")
			{
				glm::mat4 scalingMatrixPlane(1.0f); // for scaling the moon
				glm::vec3 movingVector(-1400, -50, 0); // for moving the moon
				scalingMatrixPlane = glm::rotate(scalingMatrixPlane, glm::radians(rotationtest),
				                                 glm::vec3(0.0f, 1.0f, 0.0f));
				scalingMatrixPlane = glm::scale(scalingMatrixPlane, glm::vec3(0.03f)); // for scaling the moon
				scalingMatrixPlane = glm::translate(scalingMatrixPlane, movingVector); //translate vector for moving
				tigl::shader->setModelMatrix(scalingMatrixPlane); // for scaling the moon
				rotationtest = rotationtest + 1.0f;
			}

			if (model->getName() == "Spacejet")
			{
				glm::mat4 scalingMatrixPlane(1.0f); // for scaling the moon
				//rotate the plane
				std::cout << "X IS" << camera->getPosition().x * 5 << std::endl;
				//std::cout << "Y IS" << camera->getPosition().y << std::endl;
				//std::cout << "Z IS" << camera->getPosition().z << std::endl;
				glm::vec3 movingVector((camera->getPosition().x * 10) - 80, (camera->getPosition().y * 10) - 150, camera->getPosition().z * 10);
				scalingMatrixPlane = glm::rotate(scalingMatrixPlane, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				scalingMatrixPlane = glm::scale(scalingMatrixPlane, glm::vec3(0.1f)); // for scaling the moon
				scalingMatrixPlane = glm::translate(scalingMatrixPlane, movingVector); //translate vector for moving
				tigl::shader->setModelMatrix(scalingMatrixPlane); // for scaling the moon
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
		//std::cout << rotationtest << std::endl;
	}
}

void controlShaders()
{
	glClearColor(0.6f, 0.0f, 0.0f, 1.0f);
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

	//Adding rocket
	models.push_back(new ObjModel("Resources/RocketShip/10475_Rocket_Ship_v1_L3.obj", "RocketShip"));

	//Adding asteroid
	std::cout << models.size() << std::endl;
	models.push_back(new ObjModel("Resources/UFO/Low_poly_UFO.obj", "UFO"));

	//Adding sun
	std::cout << models.size() << std::endl;
	models.push_back(new ObjModel("Resources/Sun/13913_Sun_v2_l3.obj", "Sun"));

	//Adding spacejet
	std::cout << models.size() << std::endl;
	models.push_back(new ObjModel("Resources/SpaceJet/atmo_fighter2.obj", "Spacejet"));
}

void loadTextures()
{
	textures[0] = new Texture("Resources/GalaxyTexture.png");
}

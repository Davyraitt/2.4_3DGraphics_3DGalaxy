#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "../tigl.h"

#include <string>
#include <vector>
#include <list>

class Texture;

class ObjModel
{
private:
	class Vertex
	{
	public:
		int position;
		int normal;
		int texcoord;
	};

	class Face
	{
	public:
		std::list<Vertex> vertices;
	};


	class MaterialInfo
	{
	public:
		MaterialInfo();
		std::string name;
		Texture* texture;
		glm::vec3 ka;
		glm::vec3 kd;
		glm::vec3 ks;

		glm::vec4 getColor();
	};

	class ObjGroup
	{
	public:
		std::string name;
		int materialIndex;
		std::list<Face> faces;
	};

	//todo remove these when vbo is created
	std::vector<glm::vec3>	vertices;
	std::vector<glm::vec3>	normals;
	std::vector<glm::vec2>	texcoords;
	std::vector<ObjGroup*> groups;
	std::vector<MaterialInfo*> materials;
	std::vector<tigl::VBO*> vbos;
	bool texture;
	std::string objectName;

	void loadMaterialFile(const std::string& fileName, const std::string& dirName);


public:
	void createVBO();
	ObjModel(const std::string &filename, std::string name);
	~ObjModel(void);

	void draw();
	std::string getName();
	bool hasTexture();
};


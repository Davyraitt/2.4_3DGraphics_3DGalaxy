#include "ObjModel.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>

#include <glm/gtc/matrix_transform.hpp>

#include "tigl.h"
#include "Texture.h"

using tigl::Vertex;

/**
* Replaces a substring in a string
*/

static std::string replace(std::string str, const std::string &toReplace, const std::string &replacement)
{
	size_t index = 0;
	while (true)
	{
		 index = str.find(toReplace, index);
		 if (index == std::string::npos)
			 break;
		 str.replace(index, toReplace.length(), replacement);
		 ++index;
	}
	return str;
}

/**
* Splits a string into substrings, based on a seperator
*/
static std::vector<std::string> split(std::string str, const std::string &seperator)
{
	std::vector<std::string> ret;
	size_t index;
	while(true)
	{
		index = str.find(seperator);
		if(index == std::string::npos)
			break;
		ret.push_back(str.substr(0, index));
		str = str.substr(index+1);
	}
	ret.push_back(str);
	return ret;
}

/**
* Turns a string to lowercase
*/
static inline std::string toLower(std::string data)
{
	std::transform(data.begin(), data.end(), data.begin(), ::tolower);
	return data;
}


/**
* Cleans up a line for processing
*/
static inline std::string cleanLine(std::string line)
{
	line = replace(line, "\t", " ");
	while (line.find("  ") != std::string::npos)
		line = replace(line, "  ", " ");
	if (line == "")
		return "";
	if (line[0] == ' ')
		line = line.substr(1);
	if (line == "")
		return "";
	if (line[line.length() - 1] == ' ')
		line = line.substr(0, line.length() - 1);
	return line;
}


void ObjModel::createVBO() {
	//foreach group in groups
	for (auto group : this->groups) {
		texture = false;
		glm::vec4 color = glm::vec4(1.0f);
		std::vector<tigl::Vertex> verticesList;

		//  set material texture, if available
		if (materials.size() > group->materialIndex && group->materialIndex != -1)
		{
			if (materials.at(group->materialIndex)->texture != NULL)
			{
				texture = true;
				materials.at(group->materialIndex)->texture->bind();
			}

			color = materials.at(group->materialIndex)->getColor();
		}


		//  foreach face in group
		for (auto face : group->faces) {

			//    foreach vertex in face
			std::vector<tigl::Vertex> vertexList;
			for (auto vertex : face.vertices) {
				//emit vertex
				if (texture)
				{
					//verticesList.push_back(tigl::Vertex::PT(vertices.at(vertex.position), texcoords.at(vertex.texcoord) ));
					verticesList.push_back(tigl::Vertex::PTN(vertices.at(vertex.position), texcoords.at(vertex.texcoord), normals.at(vertex.normal)));
				}
				else
				{
					verticesList.push_back(tigl::Vertex::PC(vertices.at(vertex.position), color));
				}
			}
		}

		vbos.push_back(tigl::createVbo(verticesList));
	}

}

/**
* Loads an object model
*/
ObjModel::ObjModel(const std::string &fileName, std::string name)
{
	objectName = name;
	std::cout << "Loading " << fileName << std::endl;
	std::string dirName = fileName;
	if(dirName.rfind("/") != std::string::npos)
		dirName = dirName.substr(0, dirName.rfind("/"));
	if(dirName.rfind("\\") != std::string::npos)
		dirName = dirName.substr(0, dirName.rfind("\\"));
	if(fileName == dirName)
		dirName = "";


	std::ifstream pFile(fileName.c_str());

	if (!pFile.is_open())
	{
		std::cout << "Could not open file " << fileName << std::endl;
		return;
	}


	ObjGroup* currentGroup = new ObjGroup();
	currentGroup->materialIndex = -1;


	while(!pFile.eof())
	{
		std::string line;
		std::getline(pFile, line);
		line = cleanLine(line);
		if(line == "" || line[0] == '#') //skip empty or commented line
			continue;

		std::vector<std::string> params = split(line, " ");
		params[0] = toLower(params[0]);

		if(params[0] == "v")
			vertices.push_back(glm::vec3((float)atof(params[1].c_str()), (float)atof(params[2].c_str()), (float)atof(params[3].c_str())));
		else if(params[0] == "vn")
			normals.push_back(glm::vec3((float)atof(params[1].c_str()), (float)atof(params[2].c_str()), (float)atof(params[3].c_str())));
		else if(params[0] == "vt")
			texcoords.push_back(glm::vec2((float)atof(params[1].c_str()), (float)atof(params[2].c_str())));
		else if(params[0] == "f")
		{
			for(size_t ii = 4; ii <= params.size(); ii++)
			{
				Face face;

				for(size_t i = ii-3; i < ii; i++)	//magische forlus om van quads triangles te maken ;)
				{
					Vertex vertex;
					std::vector<std::string> indices = split(params[i == (ii-3) ? 1 : i], "/");
					if (indices.size() >= 1)	//er is een positie
						vertex.position = atoi(indices[0].c_str()) - 1;
					if(indices.size() == 2)		//alleen texture
						vertex.texcoord = atoi(indices[1].c_str())-1;
					if(indices.size() == 3)		//v/t/n of v//n
					{
						if( indices[1] != "")
							vertex.texcoord = atoi(indices[1].c_str())-1;
						vertex.normal = atoi(indices[2].c_str())-1;
					}
					face.vertices.push_back(vertex);
				}
				currentGroup->faces.push_back(face);
			}
		}
		else if(params[0] == "s")
		{
			//smoothing groups
		}
        else if(params[0] == "mtllib")
        {
            loadMaterialFile(dirName + "/" + params[1], dirName);
        }
		else if(params[0] == "usemtl")
		{
			if(currentGroup->faces.size() != 0)
				groups.push_back(currentGroup);
			currentGroup = new ObjGroup();
			currentGroup->materialIndex = -1;

			for(size_t i = 0; i < materials.size(); i++)
			{
				MaterialInfo* info = materials[i];
				if(info->name == params[1])
				{
					currentGroup->materialIndex = i;
					break;
				}
			}
			if(currentGroup->materialIndex == -1)
				std::cout<<"Could not find material name "<<params[1]<<std::endl;
		}
	}
	groups.push_back(currentGroup);

	createVBO();
}


ObjModel::~ObjModel(void)
{
}

void ObjModel::draw()
{
	for (auto vbo : vbos) {

		materials.at(0)->texture->bind();
		tigl::drawVertices(GL_TRIANGLES, vbo);
	}
}

std::string ObjModel::getName()
{
	return objectName;
}

bool ObjModel::hasTexture()
{
	return texture;
}

void ObjModel::loadMaterialFile( const std::string &fileName, const std::string &dirName )
{
	std::cout << "Loading " << fileName << std::endl;
	std::ifstream pFile(fileName.c_str());
	if (!pFile.is_open())
	{
		std::cout << "Could not open file " << fileName << std::endl;
		return;
	}

	MaterialInfo* currentMaterial = NULL;

	while(!pFile.eof())
	{
		std::string line;
		std::getline(pFile, line);
		line = cleanLine(line);
		if(line == "" || line[0] == '#')
			continue;

		std::vector<std::string> params = split(line, " ");
		params[0] = toLower(params[0]);

		if(params[0] == "newmtl")
		{
			currentMaterial = new MaterialInfo();
			currentMaterial->name = params[1];
			materials.push_back(currentMaterial);
		}
		else if(params[0] == "map_kd")
		{
			std::string tex = params[1];
			if (tex.find("/"))
				tex = tex.substr(tex.rfind("/") + 1);
			if (tex.find("\\"))
				tex = tex.substr(tex.rfind("\\") + 1);

			if (tex!=".")
			{
				currentMaterial->texture = new Texture(dirName + "/" + tex);
			}

		}
		else if (params[0] == "kd")
		{
			currentMaterial->kd = glm::vec3(atoi(params[1].c_str()), atoi(params[2].c_str()), atoi(params[3].c_str()));
		}
		else if (params[0] == "ka")
		{
			currentMaterial->ka = glm::vec3(atoi(params[1].c_str()), atoi(params[2].c_str()), atoi(params[3].c_str()));
		}
		else if (params[0] == "ks")
		{
			currentMaterial->ks = glm::vec3(atoi(params[1].c_str()), atoi(params[2].c_str()), atoi(params[3].c_str()));
		}
		else if (
			params[0] == "illum" ||
			params[0] == "map_bump" ||
			params[0] == "map_ke" ||
			params[0] == "map_ka" ||
			params[0] == "map_d" ||
			params[0] == "d" ||
			params[0] == "ke" ||
			params[0] == "ns" ||
			params[0] == "ni" ||
			params[0] == "td" ||
			params[0] == "tf" ||
			params[0] == "tr" ||
			false)
		{
			//these values are usually not used for rendering at this time, so ignore them
		}
		else
			std::cout<<"Didn't parse "<<params[0]<<" in material file"<<std::endl;
	}
}

ObjModel::MaterialInfo::MaterialInfo()
{
	texture = NULL;
}

glm::vec4 ObjModel::MaterialInfo::getColor()
{
	float color = 1.0f;

	/*float color = this->ka * ambientcolor +
		this->kd * lambertian * diffusecolor +
		this->ks * specular * specularColor;*/
	return glm::vec4(color,color,color,1.0f);
}



#define STB_TRUETYPE_IMPLEMENTATION
#include "Text.h"
#include <cstdio>
#include <fstream>
#include <string>
#include <GL/glew.h>
#include "../CubeComponent.h"
#include "glm/gtc/matrix_transform.hpp"


Text::Text()
{
	//read file from disk to byte[] ttf_buffer
	std::ifstream inFile("c:/windows/fonts/times.ttf", std::ios_base::binary);
	inFile.seekg(0, std::ios::end);
	std::size_t size = inFile.tellg();
	inFile.seekg(0, std::ios::beg);
	unsigned char* ttf_buffer = new unsigned char[size];
	inFile.read((char*)ttf_buffer, size);
	inFile.close();

	//create bitmap
	unsigned char* temp_bitmap = new unsigned char[512 * 512];
	stbtt_BakeFontBitmap(ttf_buffer, 0, 32.0, temp_bitmap, 512, 512, 32, 96, cdata); // no guarantee this fits!

	//generate texture and copy bitmap to GPU
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 512, 512, 0, GL_ALPHA, GL_UNSIGNED_BYTE, temp_bitmap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//clean up memory
	delete[] ttf_buffer;
	delete[] temp_bitmap;
}


Text::~Text()
{
}

void Text::draw(const std::string& text, float x, float y)
{
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	tigl::shader->setProjectionMatrix(glm::ortho(0.0f, (float)viewport[2], (float)viewport[3], 0.0f, -100.0f, 100.0f));
	tigl::shader->setViewMatrix(glm::mat4(1.0f));
	tigl::shader->setModelMatrix(glm::mat4(1.0f));

	tigl::shader->enableColor(false);
	tigl::shader->enableLighting(false);
	tigl::shader->enableTexture(true);
	tigl::shader->enableColorMult(true);
	tigl::shader->setColorAdd(glm::vec4(1, 1, 1, 0));
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D, texId);
	stbtt_aligned_quad q;
	tigl::begin(GL_QUADS);
	for (int i = 0; i < text.size(); i++)
	{
		if (text[i] >= 32 && text[i] < 128)
		{
			stbtt_GetBakedQuad(cdata, 512, 512, text[i] - 32, &x, &y, &q, 1);
			tigl::addVertex(Vertex::PT(glm::vec3(q.x0, q.y0, 0), glm::vec2(q.s0, q.t0)));
			tigl::addVertex(Vertex::PT(glm::vec3(q.x1, q.y0, 0), glm::vec2(q.s1, q.t0)));
			tigl::addVertex(Vertex::PT(glm::vec3(q.x1, q.y1, 0), glm::vec2(q.s1, q.t1)));
			tigl::addVertex(Vertex::PT(glm::vec3(q.x0, q.y1, 0), glm::vec2(q.s0, q.t1)));
		}
	}
	tigl::end();
	tigl::shader->setColorAdd(glm::vec4(0, 0, 0, 0));
}

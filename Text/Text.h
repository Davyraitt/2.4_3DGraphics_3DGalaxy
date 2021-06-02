#pragma once
#include <xstring>
#include <GL/glew.h>
#include "stb_truetype.h"

class Text
{
public:
	Text();
	~Text();
	void init();
	void draw(const std::string &drawText, float x, float y);
private:
	GLuint texId;
	stbtt_bakedchar cdata[96];
};


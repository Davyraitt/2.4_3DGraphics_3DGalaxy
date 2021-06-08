#pragma once

#include "DrawComponent.h"
#include "../tigl.h"

namespace tigl {
	struct Vertex;
}

using tigl::Vertex;

#include <vector>

class CubeComponent : public DrawComponent
{
	std::vector<Vertex> verts;
public:
	CubeComponent(float size);
	~CubeComponent();


	virtual void draw() override;
};


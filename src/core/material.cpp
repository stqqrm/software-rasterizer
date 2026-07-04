#include "material.h"
#include <iostream>

namespace core {

material::material() : 
	_diffuse_slot(0), _specular_slot(0), _normal_slot(0), _diffuse_color(0, 0, 0, 255)
{}

material::~material()
{}

} // namespace core

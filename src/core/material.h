#pragma once
#include "inc.h"
#include "texture.h"

namespace core {

class material {
    friend class model;
public:
    material();
	~material();

    int GetDiffuseSlot()  const { return _diffuse_slot; }
    int GetSpecularSlot() const { return _specular_slot; }
    int GetNormalSlot()   const { return _normal_slot; }

	color GetDiffuseColor() const { return _diffuse_color; }
private:
    int _diffuse_slot = 0;
    int _specular_slot = 0;
    int _normal_slot = 0;
	color _diffuse_color;
};

} // namespace core

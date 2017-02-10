#pragma once

#include "Assets.h"
#include "RigidBody.h"

#include <glm/ext.hpp>

namespace RigidBodyFactory {
    
    extern void resetSpinningTop(RigidBody &rb, int type,  bool rotating, bool upsidedown, float xOffset, float yOffset);

};
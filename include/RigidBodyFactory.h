#pragma once

#include "RigidBody.h"
#include "Assets.h"
#include <glm/ext.hpp>

namespace RigidBodyFactory {
    
    extern void resetSpinningTop(RigidBody &rb, int type,  bool rotating, bool upsidedown);
};
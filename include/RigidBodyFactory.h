#pragma once

#include "RigidBody.h"
#include "Assets.h"
#include <glm/ext.hpp>

namespace RigidBodyFactory {
    
    extern void reset(RigidBody &rb);
    
    extern void resetSphere(RigidBody &rb);
    
    extern void resetCube(RigidBody &rb);
    
    extern void resetSpinningTop1(RigidBody &rb);
    
    extern void resetSpinningTop2(RigidBody &rb);
    
    extern void resetSpinningTop3(RigidBody &rb);
    
    extern void resetSpinningTop3Top(RigidBody &rb);
    
    extern void resetSpinningTop3Bottom(RigidBody &rb);
    
    extern void resetSpinningTop4(RigidBody &rb);
    
    extern void resetSpinningTop5(RigidBody &rb);
    
    extern void resetSpinningTop6(RigidBody &rb);
    
    extern void resetSpinningTop(RigidBody &rb, int type);
};
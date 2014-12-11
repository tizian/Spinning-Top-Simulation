#include "RigidBodyFactory.h"

void RigidBodyFactory::reset(RigidBody &rb) {
    rb = RigidBody();
    rb.setPosition(glm::vec3(0, 5, 0));
    rb.setMaterial(Assets::getWhiteMaterial());
    rb.setTexture(Assets::getLightWood());
    
    //        rb.setOrientation(glm::quat_cast(glm::rotate((float)M_PI, glm::vec3(1, 0, 0))));
    //        rb.addForce(vec3(0,0,400), rb.getPosition() + vec3(1,0,0));
    //        rb.addForce(vec3(0,0,-400), rb.getPosition() + vec3(-1,0,0));
}

void RigidBodyFactory::resetSphere(RigidBody &rb) {
    reset(rb);
    rb.setBodyInertiaTensorInv(glm::diagonal3x3(glm::vec3(2.5, 2.5, 2.5)));
    
    rb.setMesh(Assets::getSphere());
    
    rb.type = 0;
}

void RigidBodyFactory::resetCube(RigidBody &rb) {
    reset(rb);
    rb.setBodyInertiaTensorInv(glm::diagonal3x3(glm::vec3(1.5, 1.5, 1.5)));
    
    rb.setMesh(Assets::getCube());
    
    rb.type = 9;
}

void RigidBodyFactory::resetSpinningTop1(RigidBody &rb) {
    reset(rb);
    //    spinningTop.setBodyInertiaTensorInv(glm::diagonal3x3(glm::vec3(5, 3.4, 5)));
    rb.setBodyInertiaTensorInv(glm::diagonal3x3(glm::vec3(5, 1.5*5, 5)));
    
    rb.setMesh(Assets::getSpinningTop1());
    
    rb.type = 1;
}

void RigidBodyFactory::resetSpinningTop2(RigidBody &rb) {
    reset(rb);
    
    rb.setBodyInertiaTensorInv(glm::diagonal3x3(glm::vec3(4.8, 4, 4.8)));
    //        rb.setBodyInertiaTensorInv(glm::diagonal3x3(glm::vec3(4.8, 1.5*4.8, 4.8)));
    
    rb.setMesh(Assets::getSpinningTop2());
    
    rb.setTexture(Assets::getBrushedMetal());
    
    rb.type = 2;
}

void RigidBodyFactory::resetSpinningTop3(RigidBody &rb) {
    reset(rb);
    
    rb.setBodyInertiaTensorInv(glm::diagonal3x3(glm::vec3(3.6, 2.4, 3.6)));
    //        rb.setBodyInertiaTensorInv(glm::diagonal3x3(glm::vec3(3.6, 1.5*3.6, 3.6)));
    
    rb.setMesh(Assets::getSpinningTop3());
    
    rb.type = 3;
}

void RigidBodyFactory::resetSpinningTop3Top(RigidBody &rb) {
    reset(rb);
    
    rb.setBodyInertiaTensorInv(glm::diagonal3x3(glm::vec3(1.19, 2.42, 1.19)));    // After 1 Mio. samples
    rb.setMaterial(Assets::getYellowMaterial());
    
    rb.setMesh(Assets::getSpinningTop3Top());
}

void RigidBodyFactory::resetSpinningTop3Bottom(RigidBody &rb) {
    reset(rb);
    
    rb.setBodyInertiaTensorInv(glm::diagonal3x3(glm::vec3(2.79, 2.42, 2.79)));    // After 1 Mio. samples
    rb.setMaterial(Assets::getRedMaterial());
    
    rb.setMesh(Assets::getSpinningTop3Bottom());
}

void RigidBodyFactory::resetSpinningTop4(RigidBody &rb) {
    reset(rb);
    
    //        rb.setBodyInertiaTensorInv(glm::diagonal3x3(glm::vec3(2, 2, 2)));
    
    rb.setBodyInertiaTensorInv(glm::diagonal3x3(glm::vec3(2.06, 2.03, 2.06)));    // After 1 Mio. samples
    
    rb.setMesh(Assets::getSpinningTop4());
    
    rb.type = 4;
}

void RigidBodyFactory::resetSpinningTop5(RigidBody &rb) {
    reset(rb);
    
    rb.setBodyInertiaTensorInv(glm::diagonal3x3(glm::vec3(3.16, 3.89, 3.16)));    // After 1 Mio. samples
    
    rb.setMesh(Assets::getSpinningTop5());
    
    rb.type = 5;
}

void RigidBodyFactory::resetSpinningTop6(RigidBody &rb) {
    reset(rb);
    
    rb.setBodyInertiaTensorInv(glm::diagonal3x3(glm::vec3(3.63, 3.28, 3.63)));    // After 1 Mio. samples
    
    rb.setMesh(Assets::getSpinningTop6());
    
    rb.type = 6;
}

void RigidBodyFactory::resetSpinningTop(RigidBody &rb, int type) {
    if (type == 1) {
        resetSpinningTop1(rb);
    }
    else if (type == 2) {
        resetSpinningTop2(rb);
    }
    else if (type == 3) {
        resetSpinningTop3(rb);
    }
    else if (type == 4) {
        resetSpinningTop4(rb);
    }
    else if (type == 5) {
        resetSpinningTop5(rb);
    }
    else if (type == 6) {
        resetSpinningTop6(rb);
    }
    else if (type == 9) {
        resetCube(rb);
    }
    else if (type == 0) {
        resetSphere(rb);
    }
}
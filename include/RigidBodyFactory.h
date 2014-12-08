#pragma once

namespace RigidBodyFactory {
    
    void reset(RigidBody &rb) {
        rb = RigidBody();
        rb.setPosition(glm::vec3(0, 5, 0));
        rb.setMaterial(&Assets::whiteMaterial);
        rb.setTexture(&Assets::lightWood);
        
        rb.setDebugMesh(&Assets::sphere);
        rb.setDebugMaterial(&Assets::redMaterial);
        
//        rb.setOrientation(glm::quat_cast(glm::rotate((float)M_PI, glm::vec3(1, 0, 0))));
//        rb.addForce(vec3(0,0,400), rb.getPosition() + vec3(1,0,0));
//        rb.addForce(vec3(0,0,-400), rb.getPosition() + vec3(-1,0,0));
    }
    
    void resetSphere(RigidBody &rb) {
        reset(rb);
        rb.setBodyInertiaTensorInv(glm::diagonal3x3(glm::vec3(2.5, 2.5, 2.5)));
        
        rb.setMesh(&Assets::sphere);
        
        rb.type = 0;
    }
    
    void resetCube(RigidBody &rb) {
        reset(rb);
        rb.setBodyInertiaTensorInv(glm::diagonal3x3(glm::vec3(1.5, 1.5, 1.5)));
        
        rb.setMesh(&Assets::cube);
        
        rb.type = 9;
    }
    
    void resetSpinningTop1(RigidBody &rb) {
        reset(rb);
        //    spinningTop.setBodyInertiaTensorInv(glm::diagonal3x3(glm::vec3(5, 3.4, 5)));
        rb.setBodyInertiaTensorInv(glm::diagonal3x3(glm::vec3(5, 1.5*5, 5)));
        
        rb.setMesh(&Assets::spinningTop1);
        
        rb.type = 1;
    }
    
    void resetSpinningTop2(RigidBody &rb) {
        reset(rb);
        
        rb.setBodyInertiaTensorInv(glm::diagonal3x3(glm::vec3(4.8, 4, 4.8)));
//        rb.setBodyInertiaTensorInv(glm::diagonal3x3(glm::vec3(4.8, 1.5*4.8, 4.8)));
        
        rb.setMesh(&Assets::spinningTop2);
        
        rb.setTexture(&Assets::brushedMetal);
        
        rb.type = 2;
    }
    
    void resetSpinningTop3(RigidBody &rb) {
        reset(rb);
        
        rb.setBodyInertiaTensorInv(glm::diagonal3x3(glm::vec3(3.6, 2.4, 3.6)));
//        rb.setBodyInertiaTensorInv(glm::diagonal3x3(glm::vec3(3.6, 1.5*3.6, 3.6)));
        
        rb.setMesh(&Assets::spinningTop3);
        
        rb.type = 3;
    }
    
    void resetSpinningTop3Top(RigidBody &rb) {
        reset(rb);
        
        rb.setBodyInertiaTensorInv(glm::diagonal3x3(glm::vec3(1.19, 2.42, 1.19)));    // After 1 Mio. samples
        rb.setMaterial(&Assets::yellowMaterial);
        
        rb.setMesh(&Assets::spinningTop3Top);
    }
    
    void resetSpinningTop3Bottom(RigidBody &rb) {
        reset(rb);
        
        rb.setBodyInertiaTensorInv(glm::diagonal3x3(glm::vec3(2.79, 2.42, 2.79)));    // After 1 Mio. samples
        rb.setMaterial(&Assets::redMaterial);
        
        rb.setMesh(&Assets::spinningTop3Bottom);
    }
    
    void resetSpinningTop4(RigidBody &rb) {
        reset(rb);
        
//        rb.setBodyInertiaTensorInv(glm::diagonal3x3(glm::vec3(2, 2, 2)));
        
        rb.setBodyInertiaTensorInv(glm::diagonal3x3(glm::vec3(2.06, 2.03, 2.06)));    // After 1 Mio. samples
        
        rb.setMesh(&Assets::spinningTop4);
        
        rb.type = 4;
    }
    
    void resetSpinningTop5(RigidBody &rb) {
        reset(rb);
        
        rb.setBodyInertiaTensorInv(glm::diagonal3x3(glm::vec3(3.16, 3.89, 3.16)));    // After 1 Mio. samples
        
        rb.setMesh(&Assets::spinningTop5);
        
        rb.type = 5;
    }
    
    void resetSpinningTop6(RigidBody &rb) {
        reset(rb);
        
        rb.setBodyInertiaTensorInv(glm::diagonal3x3(glm::vec3(3.63, 3.28, 3.63)));    // After 1 Mio. samples
        
        rb.setMesh(&Assets::spinningTop6);
        
        rb.type = 6;
    }
};
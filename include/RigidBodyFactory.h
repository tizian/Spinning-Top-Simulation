#pragma once

namespace RigidBodyFactory {
    
    void reset(RigidBody &rb) {
        rb = RigidBody();
        rb.setPosition(glm::vec3(0, 5, 0));
        rb.setMaterial(&Assets::blueMaterial);
        rb.setTexture(&Assets::checkerboard);
        
//        rb.setOrientation(glm::quat_cast(glm::rotate((float)M_PI/20.0f, glm::vec3(1, 0, 0))));
    }
    
    void resetSphere(RigidBody &rb) {
        reset(rb);
        rb.setMesh(&Assets::sphere);
        rb.setBodyInertiaTensorInv(glm::diagonal3x3(glm::vec3(2.5, 2.5, 2.5)));
    }
    
    void resetCube(RigidBody &rb) {
        reset(rb);
        rb.setMesh(&Assets::cube);
        rb.setBodyInertiaTensorInv(glm::diagonal3x3(glm::vec3(1.5, 1.5, 1.5)));
    }
    
    void resetSpinningTop1(RigidBody &rb) {
        reset(rb);
        rb.setMesh(&Assets::spinningTop1);
        //    spinningTop.setBodyInertiaTensorInv(glm::diagonal3x3(glm::vec3(5, 3.4, 5)));
        rb.setBodyInertiaTensorInv(glm::diagonal3x3(glm::vec3(5, 1.5*5, 5)));
    }
    
    void resetSpinningTop2(RigidBody &rb) {
        reset(rb);
        rb.setMesh(&Assets::spinningTop2);
        //    spinningTop.setBodyInertiaTensorInv(glm::diagonal3x3(glm::vec3(4.8, 4, 4.8)));
        rb.setBodyInertiaTensorInv(glm::diagonal3x3(glm::vec3(4.8, 1.5*4.8, 4.8)));
    }
    
    void resetSpinningTop3(RigidBody &rb) {
        reset(rb);
        rb.setMesh(&Assets::spinningTop3);
        //    spinningTop.setBodyInertiaTensorInv(glm::diagonal3x3(glm::vec3(3.6, 2.4, 3.6)));
//        rb.setBodyInertiaTensorInv(glm::diagonal3x3(glm::vec3(3.6, 1.5*3.6, 3.6)));
        rb.setBodyInertiaTensorInv(glm::diagonal3x3(glm::vec3(1.19, 2.42, 1.19)));
    }
    
    void resetSpinningTop3Top(RigidBody &rb) {
        reset(rb);
        rb.setMesh(&Assets::spinningTop3Top);
        rb.setBodyInertiaTensorInv(glm::diagonal3x3(glm::vec3(1.19, 2.42, 1.19)));    // After 1 Mio. samples
        rb.setMaterial(&Assets::yellowMaterial);
    }
    
    void resetSpinningTop3Bottom(RigidBody &rb) {
        reset(rb);
        rb.setMesh(&Assets::spinningTop3Bottom);
        rb.setBodyInertiaTensorInv(glm::diagonal3x3(glm::vec3(2.79, 2.42, 2.79)));    // After 1 Mio. samples
        rb.setMaterial(&Assets::redMaterial);
    }
    
    void resetSpinningTop4(RigidBody &rb) {
        reset(rb);
        rb.setMesh(&Assets::spinningTop4);
        rb.setBodyInertiaTensorInv(glm::diagonal3x3(glm::vec3(2, 2, 2)));
    }
    
    void resetSpinningTop5(RigidBody &rb) {
        reset(rb);
        rb.setMesh(&Assets::spinningTop5);
        rb.setBodyInertiaTensorInv(glm::diagonal3x3(glm::vec3(3.16, 3.89, 3.16)));    // After 1 Mio. samples
    }
    
    void resetSpinningTop6(RigidBody &rb) {
        reset(rb);
        rb.setMesh(&Assets::spinningTop6);
        rb.setBodyInertiaTensorInv(glm::diagonal3x3(glm::vec3(3.63, 3.28, 3.63)));    // After 1 Mio. samples
    }
};
#pragma once

#include <vector>

#include "Contact.h"
#include "RigidBody.h"

using namespace glm;

namespace Collision {
    
    // Coefficient of restitution
    static float e = 0.3f;
    
    /* Assume groud as plane at y = 0
     *
     * Precondiditon: first contact in list is the single contact point for the collision response impulse
     */
    static void collisionResponseWithGround(RigidBody & a, std::vector<Contact> & contacts) {
        if (contacts.size() == 0) return;
        
        vec3 org_linearMomentum = a.getLinearMomentum();
        
        vec3 point = contacts[0].p;
        vec3 normal = contacts[0].n;
        
        vec3 ra = point - a.getPosition();
        vec3 va = org_linearMomentum / a.getMass() + cross(a.getAngularVelocity(), ra);
        vec3 vb = vec3(0.0, 0.0, 0.0);
        
        vec3 vrel = va - vb;
        
        float vrelMagnitude = dot(normal, vrel);
//        printf("vrel: %f\n", vrelMagnitude);
        
        if (vrelMagnitude > 0.8) return;
        
        
        
        // Colliding contact
        
        float j = -(1.f+e) * dot(vrel, normal) / (1.f/a.getMass() + dot(normal, cross(a.getInertiaTensorInv() * cross(ra, normal), ra)));
        j = max(0.0f, j);
        
        vec3 collisionImpulse = j * normal;
        
//        printf("impulse: %f %f %f\n", collisionImpulse.x, collisionImpulse.y, collisionImpulse.z);
        
        a.addImpulse(collisionImpulse, point);
        
        // Friction
        int start_i = contacts.size() == 1 ? 0 : 1;
        int numberContacts = contacts.size() == 1 ? 1 : (int)contacts.size() - 1;
        
        for (int i = start_i; i < contacts.size(); i++) {
            
            vec3 p = contacts[i].p;
            vec3 n = contacts[i].n;
            
            ra = p - a.getPosition();
            vrel = org_linearMomentum/a.getMass() + cross(a.getAngularVelocity(), ra);
            
            // Torque Friction from http://ch.mathworks.com/help/physmod/simscape/ref/rotationalfriction.html
            
            float frictionTorque;                       // T
            float coulombFrictionTorque = 20;           // T_C; [N*m]
            float breakawayFrictionTorque = 25;         // T_brk; [N*m];
            float velocityThreshold = 0.001;            // omega_th; should be between 10^-3 and 10^-5
            float viscousFrictionCoefficient = 0.001;   // f; [N*m/(rad/s)];
            float coefficient = 10;                     // c_v; [rad/s]
            
            // x
            
            float omega = a.getAngularVelocity().x;
            
            if (abs(omega) >= velocityThreshold)
            {
                frictionTorque = (coulombFrictionTorque + (breakawayFrictionTorque - coulombFrictionTorque) * exp(-coefficient * abs(omega))) * sign(omega) + viscousFrictionCoefficient * omega;
            }
            else {
                frictionTorque = omega * (viscousFrictionCoefficient * velocityThreshold + (coulombFrictionTorque + (breakawayFrictionTorque - coulombFrictionTorque) * exp(-coefficient * velocityThreshold))) / velocityThreshold;
            }
            a.addTorque(0.1f * vec3(-frictionTorque / numberContacts, 0.0, 0.0));
            
            // y
            
            omega = a.getAngularVelocity().y;
            
            if (abs(omega) >= velocityThreshold)
            {
                frictionTorque = (coulombFrictionTorque + (breakawayFrictionTorque - coulombFrictionTorque) * exp(-coefficient * abs(omega))) * sign(omega) + viscousFrictionCoefficient * omega;
            }
            else {
                frictionTorque = omega * (viscousFrictionCoefficient * velocityThreshold + (coulombFrictionTorque + (breakawayFrictionTorque - coulombFrictionTorque) * exp(-coefficient * velocityThreshold))) / velocityThreshold;
            }
            a.addTorque(0.1f * vec3(0, -frictionTorque / numberContacts, 0));
            
            // z
            
            omega = a.getAngularVelocity().z;
            
            if (abs(omega) >= velocityThreshold)
            {
                frictionTorque = (coulombFrictionTorque + (breakawayFrictionTorque - coulombFrictionTorque) * exp(-coefficient * abs(omega))) * sign(omega) + viscousFrictionCoefficient * omega;
            }
            else {
                frictionTorque = omega * (viscousFrictionCoefficient * velocityThreshold + (coulombFrictionTorque + (breakawayFrictionTorque - coulombFrictionTorque) * exp(-coefficient * velocityThreshold))) / velocityThreshold;
            }
            a.addTorque(0.1f * vec3(0, 0, -frictionTorque / numberContacts));
            
            // Linear Friction from http://ch.mathworks.com/help/physmod/simscape/ref/translationalfriction.html
            
            float frictionForce;                // F
            float coulombFrictionForce = 20;    // F_C; [N]
            float breakawayFrictionForce = 25;  // F_brk; [N];
            velocityThreshold = 0.0001;         // omega_th; should be between 10^-4 and 10^-6
            viscousFrictionCoefficient = 20;    // f; [N/(m//s)]; default = 100;
            coefficient = 10;                   // c_v; [s/m]
            
            // x
            
            float v = vrel.x;
            
            if (abs(v) >= velocityThreshold)
            {
                frictionForce = (coulombFrictionForce + (breakawayFrictionForce - coulombFrictionForce) * exp(-coefficient * abs(v))) * sign(v) + viscousFrictionCoefficient * v;
            } else {
                frictionForce = v * (viscousFrictionCoefficient * velocityThreshold + (coulombFrictionForce + (breakawayFrictionForce - coulombFrictionForce) * exp(-coefficient * velocityThreshold))) / velocityThreshold;
            }
            
            a.addForce(0.1f * vec3(-frictionForce / numberContacts, 0, 0), p/* - vec3(0, distanceGround, 0)*/);
            
            // y
            
            v = vrel.y;
            
            if (abs(v) >= velocityThreshold)
            {
                frictionForce = (coulombFrictionForce + (breakawayFrictionForce - coulombFrictionForce) * exp(-coefficient * abs(v))) * sign(v) + viscousFrictionCoefficient * v;
            } else {
                frictionForce = v * (viscousFrictionCoefficient * velocityThreshold + (coulombFrictionForce + (breakawayFrictionForce - coulombFrictionForce) * exp(-coefficient * velocityThreshold))) / velocityThreshold;
            }
            
            a.addForce(0.1f * vec3(0, -frictionForce / numberContacts, 0), p/* - vec3(0, distanceGround, 0)*/);
            
            // z
            
            v = vrel.z;
            
            if (abs(v) >= velocityThreshold)
            {
                frictionForce = (coulombFrictionForce + (breakawayFrictionForce - coulombFrictionForce) * exp(-coefficient * abs(v))) * sign(v) + viscousFrictionCoefficient * v;
            } else {
                frictionForce = v * (viscousFrictionCoefficient * velocityThreshold + (coulombFrictionForce + (breakawayFrictionForce - coulombFrictionForce) * exp(-coefficient * velocityThreshold))) / velocityThreshold;
            }
            
            a.addForce(0.1f * vec3(0, 0, -frictionForce / numberContacts), p/* - vec3(0, distanceGround, 0)*/);
        }
        
    }
    
    static void collisionResponseBetween(RigidBody & a, RigidBody & b, std::vector<Contact> & contacts) {
        
        int numberContacts = (int)contacts.size();
        if (numberContacts == 0) return;
        
        vec3 theCollisionPoint = vec3(0,0,0);
        vec3 theCollisionNormal = vec3(0,0,0);
        for (int i = 0; i < numberContacts; ++i) {
            theCollisionPoint += contacts[i].p;
//            printf("a point: %f %f %f\n", contacts[i].p.x, contacts[i].p.y, contacts[i].p.z);
            theCollisionNormal += contacts[i].n;
//            printf("a normal: %f %f %f\n", contacts[i].n.x, contacts[i].n.y, contacts[i].n.z);
        }
        
        theCollisionPoint *= 1.f/(float)numberContacts;
        if (length(theCollisionNormal) != 0)
        {
            theCollisionNormal = normalize(theCollisionNormal);
        } else {
            theCollisionNormal = contacts[0].n;
            printf("unable to get average normal\n");
        }
//        theCollisionNormal = vec3(0,1,0); // this is how it should be (for the start case) our solution gives: 0.16, 0.98, -0.05 which is too imprecise :-(
//        printf("theCollisionPont: %f %f %f\n", theCollisionPoint.x, theCollisionPoint.y, theCollisionPoint.z);
//        printf("theCollisionNormal: %f %f %f\n", theCollisionNormal.x, theCollisionNormal.y, theCollisionNormal.z);
        
        vec3 org_linearMomentumA = a.getLinearMomentum();
        vec3 org_linearMomentumB = b.getLinearMomentum();
        
//        for (int i = 0; i < numberContacts; i++) {
//            vec3 point = contacts[i].p;
//            vec3 normal = contacts[i].n;
        vec3 point = theCollisionPoint;
        vec3 normal = theCollisionNormal;
        
            vec3 ra = point - a.getPosition();
            vec3 rb = point - b.getPosition();
            
            vec3 va = org_linearMomentumA / a.getMass() + cross(a.getAngularVelocity(), ra);
            vec3 vb = org_linearMomentumB / b.getMass() + cross(b.getAngularVelocity(), rb);
            
            vec3 vrel = va - vb;
            
            float vrelMagnitude = dot(normal, vrel);
//            printf("vrel: %f\n", vrelMagnitude);
        
            if (vrelMagnitude > 0.8)
            {
//                printf("no impuls added\n");
                return;
            }
        
            // Colliding contact
            
            float nom = -(1.f+e) * dot(vrel, normal);
            float denom1 = 1.f/a.getMass() + 1.f/b.getMass();
            float denom2 = dot(normal, cross(a.getInertiaTensorInv() * cross(ra, normal), ra));
            float denom3 = dot(normal, cross(b.getInertiaTensorInv() * cross(rb, normal), rb));
            
            float j = nom / (denom1 + denom2 + denom3);
//            printf("j: %f\n", j);
//            j = j / (float)numberContacts;
            j = max(0.0f, j);
        
            vec3 collisionImpulse = j * normal;
            
//            printf("impulse: %f %f %f\n", collisionImpulse.x, collisionImpulse.y, collisionImpulse.z);
//            printf("normal: %f %f %f\n", normal.x, normal.y, normal.z);
            
            a.addImpulse(collisionImpulse, point);
            b.addImpulse(-collisionImpulse, point);
//        }
    }
}

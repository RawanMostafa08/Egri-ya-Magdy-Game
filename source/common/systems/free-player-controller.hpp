#pragma once

#include "../ecs/world.hpp"
#include "../components/camera.hpp"
#include "../components/free-player-controller.hpp"
#include "../components/free-inspector-controller.hpp"

#include "../application.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <iostream>

namespace our
{

    // The free camera controller system is responsible for moving every entity which contains a FreeCameraControllerComponent.
    // This system is added as a slightly complex example for how use the ECS framework to implement logic.
    // For more information, see "common/components/free-camera-controller.hpp"
    class FreePLayerControllerSystem
    {
        Application *app;           // The application in which the state runs
        Entity *playerEntity;
        Entity *cameraEntity;
        Entity *inspectorEntity;

        CameraComponent *camera ;
        FreePlayerControllerComponent *controller;
        bool mouse_locked = false; // Is the mouse locked
        float jumpStrength = 10.0f; // Strength of the jump impulse
        bool isOnGround = true;     // This should be updated based on collision detection with the ground
        bool isRightKeyPressed = false;
        bool isLeftKeyPressed = false;

    public:
        // When a state enters, it should call this function and give it the pointer to the application
        void enter(Application *app)
        {
            this->app = app;

        }
        void setPlayer(Entity *player,Entity* inspector)
        {
            this->playerEntity = player;
            this->inspectorEntity = inspector; 
            this->controller = playerEntity->getComponent<FreePlayerControllerComponent>();
        }

        void setCamera(Entity *camera)
        {
            this->cameraEntity = camera;
            this->cameraEntity->getComponent<CameraComponent>();
        }

        // This should be called every frame to update all entities containing a FreeCameraControllerComponent
        void update(World *world, float deltaTime)
        {
            // First of all, we search for an entity containing both a CameraComponent and a FreeCameraControllerComponent
            // As soon as we find one, we break
                    
    
            // Get the entity that we found via getOwner of camera (we could use controller->getOwner())
            controller = playerEntity->getComponent<FreePlayerControllerComponent>();
        
            // We get a reference to the entity's position and rotation
            glm::vec3 &position = playerEntity->localTransform.position;
            glm::vec3 &rotation = playerEntity->localTransform.rotation;

            glm::vec3 &cam_position = cameraEntity->localTransform.position;

            // We prevent the pitch from exceeding a certain angle from the XZ plane to prevent gimbal locks
            if (rotation.x < -glm::half_pi<float>() * 0.99f)
                rotation.x = -glm::half_pi<float>() * 0.99f;
            if (rotation.x > glm::half_pi<float>() * 0.99f)
                rotation.x = glm::half_pi<float>() * 0.99f;
            // This is not necessary, but whenever the rotation goes outside the 0 to 2*PI range, we wrap it back inside.
            // This could prevent floating point error if the player rotates in single direction for an extremely long time.
            rotation.y = glm::wrapAngle(rotation.y);



            // We get the camera model matrix (relative to its parent) to compute the front, up and right directions
            glm::mat4 matrix = playerEntity->localTransform.toMat4();

            glm::vec3 front = glm::vec3(matrix * glm::vec4(0, 0, -1, 0)),
                      up = glm::vec3(matrix * glm::vec4(0, 1, 0, 0)),
                      right = glm::vec3(matrix * glm::vec4(1, 0, 0, 0));

            glm::vec3 current_sensitivity = controller->positionSensitivity;

            position -= front * (deltaTime * current_sensitivity.z);

                cam_position -= front * (deltaTime * current_sensitivity.z);
            
            if (app->getKeyboard().isPressed(GLFW_KEY_SPACE) && !controller->isJumping)
            {
                controller->isJumping = true;                             // The player starts jumping
                controller->currentVelocity.y = controller->jumpVelocity; // Apply vertical jump velocity
            }

            // Apply gravity and update position if jumping
            if (controller->isJumping)
            {
                position += controller->currentVelocity * deltaTime;              // Update position based on current velocity
                controller->currentVelocity.y -= controller->gravity * deltaTime; // Apply gravity to vertical velocity

                // Check if player has landed
                if (position.y <= 0.8)
                {
                    position.y = 1;                  // Reset to ground level
                    controller->isJumping = false;     // Stop jumping
                    controller->currentVelocity.y = 0; // Reset vertical velocity
                    controller->currentVelocity.x = 0; // Reset horizontal velocity
                }
            }
            //set the level of the player
            if(position.y>=1.5) controller->level='t';
            else controller->level='f';

            // If the LEFT SHIFT key is pressed, we multiply the position sensitivity by the speed up factor
            if (app->getKeyboard().isPressed(GLFW_KEY_LEFT_SHIFT))
                current_sensitivity *= controller->speedupFactor;

            // this is for checking for the key press of left and right, but making the key press registered only once
            if (app->getKeyboard().isPressed(GLFW_KEY_RIGHT))
            {
                if (!isRightKeyPressed)
                {
                    if (position.x >= 1.4)
                    {
                        position.x=1.5;
                    }
                    else
                    position -= right * glm::vec3(30, 30, 30);
                    isRightKeyPressed = true;
                }
            }
            else
            {
                isRightKeyPressed = false;
            }

            // Check for left key press
            if (app->getKeyboard().isPressed(GLFW_KEY_LEFT) )
            {
                if (!isLeftKeyPressed)
                {
                    if (position.x <= -1.5)
                    {
                        position.x=-1.5;
                    }
                    else
                    position += right * glm::vec3(30, 30, 30);
                    isLeftKeyPressed = true;
                }
            }
            else
            {
                isLeftKeyPressed = false;
            }
////////////////////////////////////////////////////////
            glm::mat4 cammatrix = cameraEntity->localTransform.toMat4();

            glm::vec3 camfront = glm::vec3(cammatrix * glm::vec4(0, 0, -1, 0)),
                      camup = glm::vec3(cammatrix * glm::vec4(0, 1, 0, 0)),
                      camright = glm::vec3(cammatrix * glm::vec4(1, 0, 0, 0));


   

            // We change the camera position based on the keys WASD/QE
            // S & W moves the player back and forth
            if(app->getKeyboard().isPressed(GLFW_KEY_W)) cam_position += camfront * (deltaTime * current_sensitivity.z);
            if(app->getKeyboard().isPressed(GLFW_KEY_S)) cam_position -= camfront * (deltaTime * current_sensitivity.z);
            // Q & E moves the player up and down
            if(app->getKeyboard().isPressed(GLFW_KEY_Q)) cam_position += camup * (deltaTime * current_sensitivity.y);
            if(app->getKeyboard().isPressed(GLFW_KEY_E)) cam_position -= camup * (deltaTime * current_sensitivity.y);
            // A & D moves the player left or right
            if(app->getKeyboard().isPressed(GLFW_KEY_D)) cam_position += camright * (deltaTime * current_sensitivity.x);
            if(app->getKeyboard().isPressed(GLFW_KEY_A)) cam_position -= camright * (deltaTime * current_sensitivity.x);
        
        // Update position of inspector
        glm::vec3 &ins_position = inspectorEntity->localTransform.position;
        ins_position = position;
        ins_position.z += 0.5; 
        
        }
        // When the state exits, it should call this function to ensure the mouse is unlocked
        void
        exit()
        {
            if (mouse_locked)
            {
                mouse_locked = false;
                app->getMouse().unlockMouse(app->getWindow());
            }
        }
    };
}
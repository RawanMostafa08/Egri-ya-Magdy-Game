#pragma once
#include "../ecs/world.hpp"
#include "../components/movement.hpp"
#include "../components/mesh-renderer.hpp"
#include "../application.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <iostream>
#include <string>

namespace our
{

    class CollisionSystem
    {
        // std::vector<Entity *> cars, batteries, planes, packages;
        Entity *player;

    public:
        void setPlayer(Entity *player)
        {
            this->player = player;
        }
        char getlane(glm::vec3 position){
            if (position.x >= -0.1 && position.x <= 0.1)
                return 'm';
            else if (position.x < -0.8)
                return 'l';
            else if (position.x > 0.8)
                return 'r';
            return 'm';
        }
        bool checkCollision(Entity *objectComponent, Entity *playerComponent){
            glm::vec3 playerPosition = playerComponent->localTransform.position;
            glm::vec3 objectPosition = objectComponent->localTransform.position;
            RepeatControllerComponent *collidedObject = objectComponent->getComponent<RepeatControllerComponent>();

            //case1: not on the same lane-->return false
            if (getlane(playerPosition) != getlane(objectPosition)) 
                return false;

            //case2: magdy is on the train and collided with a train-->return false
            FreePlayerControllerComponent *player = playerComponent->getComponent<FreePlayerControllerComponent>();
            if (player && player->level == 't' &&collidedObject && collidedObject->repeatedObject == "train")
                return false;
            //case3: magdy collided with a coin anywhere
            std::cout<<collidedObject->repeatedObject<<" "<<player->level<<" "<< objectPosition.y<<" "<<playerPosition.z<<" "<<objectPosition.z<<std::endl;
            if(collidedObject && collidedObject->repeatedObject == "coin"
            &&((player->level=='t' && objectPosition.y==1.7)||(player->level=='f' && objectPosition.y==0.7))
            && playerPosition.z==objectPosition.z)
            {
            std::cout<<"ana akalt coin"<<std::endl;
            return true;
            }

            
            glm::vec3 frontFace =  objectPosition + objectComponent->size/2;
            glm::vec3 backFace  = objectPosition - objectComponent->size/2;
            //case4: magdy is on the floor and collided with a train-->return true
            if (playerPosition.z <= frontFace.z && playerPosition.z >= backFace.z)
                return true;
            else
                return false;
        }

        int update(World *world, float deltaTime)
        {
            // iterate over all entities in the world and check for collisions with the player
            for (const auto &entity : world->getEntities())
            {
                // std::cout << entity->name << std::endl;
                if (entity->name != "magdy" && checkCollision(entity, player))
                {
                    // std::cout << "collision happened" << std::endl;
                    if (entity->name == "coin")
                    {
                        entity->hidden = true;
                        return 1;
                    }
                    else if (entity->name == "train")
                    {
                        entity->hidden = true;
                        return -1;
                    }
                }
            }
            return 0;
        }
    };
}
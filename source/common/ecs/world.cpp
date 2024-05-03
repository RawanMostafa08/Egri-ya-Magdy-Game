#include "world.hpp"
#include "../components/free-player-controller.hpp"
namespace our
{
    Entity* World::getPlayer()
    {
        FreePlayerControllerComponent * player=nullptr;
         for (auto entity : getEntities())
            {
                player = entity->getComponent<FreePlayerControllerComponent>();
                if (player)
                    break;
            }
            return player->getOwner();
    }
    // This will deserialize a json array of entities and add the new entities to the current world
    // If parent pointer is not null, the new entities will be have their parent set to that given pointer
    // If any of the entities has children, this function will be called recursively for these children
    void World::deserialize(const nlohmann::json &data, Entity *parent)
    {
        if (!data.is_array())
            return;
        for (const auto &entityData : data)
        {
            // TODO: (Req 8) Create an entity, make its parent "parent" and call its deserialize with "entityData".
            Entity *entity = this->add();
            entity->parent = parent;
            entity->deserialize(entityData);
            if (entityData.contains("children"))
            {
                // TODO: (Req 8) Recursively call this world's "deserialize" using the children data
                //  and the current entity as the parent
                deserialize(entityData["children"], entity);
            }
        }
    }

}
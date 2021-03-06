#pragma once
#include "../Components/Components.h"
#include "../Components/ComponentManager.h"
#include "../EntityManager/EntityManager.h"
#include "../Systems/GridSystem/GridSystem.h"

#include <array>

class MapCreator
{
public:
  MapCreator(
    EntityManager *entityManager,
    ComponentManager<RenderComponent> *renderComponentManager,
    ComponentManager<CollisionComponent> *collisionComponentManager,
    ComponentManager<HealthComponent> *healthComponentManager,
    ComponentManager<DamageComponent> *damageComponentManager,
    GridSystem *gridSystem
  );
  ~MapCreator();

  void createMap(std::array<std::array<int, 32>, 32> gameMap);

private:
  EntityManager *mEntityManager;
  ComponentManager<RenderComponent> *mRenderComponentManager;
  ComponentManager<CollisionComponent> *mCollisionComponentManager;
  ComponentManager<HealthComponent> *mHealthComponentManager;
  ComponentManager<DamageComponent> *mDamageComponentManager;

  GridSystem *mGridSystem;

  std::array<sf::Texture*, 99> mTextures;

  Entity* createHorizontalWall(float xPos, float yPos);
  Entity* createVerticalWall(float xPos, float yPos);
  Entity* createFloor(Textures texture, sf::Vector2f position, sf::IntRect positionInTexture);
  Entity* createFloorSpikes(sf::Vector2f position);
  sf::Texture* loadTexture(std::string fileName);
};


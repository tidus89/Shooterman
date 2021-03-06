#pragma once

#include "SFML/Graphics.hpp"
#include "../../Common/AnimationType.h"
#include "../../Common/Textures.h"
#include "../../Common/Animation/Animation.h"

#include <map>

enum class ComponentType {
  Position,
  Render,
  Velocity,
  Collision,
  Solid,
  Animated,
};

enum class PlayerState {
  Idle,
  Running,
  Attacking,
  Dead
};

enum class PlayerClass {
  Mage,
  Knight,
  Archer,
  Spearman
};

struct PositionComponent {
  sf::Vector2f position;
};

struct RenderComponent {
  sf::Texture texture;
  Textures textureId;
  sf::Sprite sprite;
  bool visible;
  bool isDynamic = false;
};

struct VelocityComponent {
  sf::Vector2f maxVelocity;
  sf::Vector2f currentVelocity;
  bool moveOnce; // Use this to know if the velocity should be reset to 0 after a movement has been made (for instance for a player who only should move if a key is pressed).
};

struct CollisionComponent {
  bool collided;
  std::vector<int> collidedList;
  bool destroyOnCollision;
};

struct AnimationComponent {
  AnimationType currentAnimation;
  std::map<AnimationType, Animation> animations;
};

struct HealthComponent {
  bool isAlive;
  int health;
};

struct DamageComponent {
  int damage;
};

struct ClockComponent {
  sf::Clock clock;
};

struct PlayerComponent {
  PlayerState state;
  PlayerClass playerClass;
  int attackSpeed;
  sf::Vector2i nextAttackMousePosition;
  int nrOfKills;
  int nrOfDeaths;
  int score;
};

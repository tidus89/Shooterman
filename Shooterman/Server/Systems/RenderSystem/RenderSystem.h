#pragma once

#include <list>
#include <SFML\Network.hpp>
#include "../../Components/ComponentManager.h"
#include "../../Components/Components.h"
#include "../../EntityManager/EntityManager.h"
#include "../../Player.h"
#include "../../../Common/MessageHandler/MessageHandler.h"
#include "../../../Common/Messages/SpriteMessage.h"
#include "../../../Common/Messages/SpriteCacheMessage.h"
#include "../../../Common/Trace.h"

class RenderSystem : Trace {
public:
  RenderSystem();
  RenderSystem(ComponentManager<RenderComponent>* renderComponentManager);
  ~RenderSystem();
  void render(std::shared_ptr<std::map<int, Player*>> connectedClients);

private:
  bool mSentCachedSpriteList = false;
  ComponentManager<RenderComponent>* mRenderComponentManager;

  void renderCached(std::shared_ptr<std::map<int, Player*>> connectedClients);
};


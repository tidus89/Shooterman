#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include <stdlib.h>
#include <time.h>

#include "Executor.h"
#include "Tasks/Network/SetupNetworkHandlerCommunicationTask.h"
#include "../Input/Input.h"
#include "../../Common/MessageHandler/MessageHandler.h"
#include "../GUI/Gui.h"
#include "../Sound/Sound.h"
#include "../Network/NetworkHandler.h"
#include "../../Common/Messages/GameStateMessage.h"
#include "../../Common/Trace.h"
#include "../../Server/Main/GameLoop.h"

class ClientMain : Trace {
public:
	ClientMain();

private:
  bool mRunning = true;
  Subscriber mSystemMessageSubscriber;
  Subscriber mGameStateMessageSubscriber;
  bool mServerStarted;
  GAME_STATE mCurrentGameState;
  Executor mExecutor;

  void handleSystemMessages();
  void handleGameStateMessages();
};
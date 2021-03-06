#include "Gui.h"

Gui::Gui() {
  mName = "CLIENT: GUI";
  TRACE_INFO("Starting module...");
  mGuiThread = std::make_unique<std::thread>(&Gui::init, this);
  TRACE_INFO("Starting module done");
}

void Gui::init() {
  MessageHandler::get().subscribeToSystemMessages(&mSystemMessageSubscriber);
  MessageHandler::get().subscribeToGameStateMessages(&mGameStateMessageSubscriber);
  mMouseInterface = new Interface();
  MessageHandler::get().publishInterface("MousePosition", mMouseInterface);

  Interface pc;

  mMenuMap.emplace(GAME_STATE::MAIN_MENU, new MainMenu());
  mMenuMap.emplace(GAME_STATE::LOBBY, new LobbyMenu());
  mMenuMap.emplace(GAME_STATE::JOIN, new JoinMenu());
  mMenuMap.emplace(GAME_STATE::PLAYING, new PlayWindow());
  mMenuMap.emplace(GAME_STATE::OPTIONS, new OptionsMenu());
  mMenuMap.emplace(GAME_STATE::PAUSE, new PauseMenu());
  mDebugMenu = new DebugMenu;

  // This needs to be after the DebugMenu is created
  while (!MessageHandler::get().subscribeTo("ClientDebugMenu", &mDebugSubscriber)) {
    sf::sleep(sf::milliseconds(5));
  }
  AddDebugButtonMessage debMess(mDebugSubscriber.getId(), "GUI debug traces");
  mDebugSubscriber.reverseSendMessage(debMess.pack());



  mWindow = std::make_shared<sf::RenderWindow>(sf::VideoMode(1024, 1024), "Shooterman");
  mWindowOpen = true;
  mShowDebugMenu = false;

  mCurrentGameState = GAME_STATE::MAIN_MENU;
  mLeftButtonAlreadyPressed = false;

  render();

  MessageHandler::get().unsubscribeAll(&mSystemMessageSubscriber);
  MessageHandler::get().unsubscribeAll(&mGameStateMessageSubscriber);
  MessageHandler::get().unsubscribeTo("ClientDebugMenu", &mDebugSubscriber);
  MessageHandler::get().unpublishInterface("MousePosition");
  delete mMouseInterface;
  delete mDebugMenu;
  // TODO: Delete all in mMenuMap
}

void Gui::render() {
  while (mWindow != nullptr && mWindow->isOpen()) {
    handleWindowEvents();
    
    bool renderNeeded = false;
    renderNeeded = renderGameState(mCurrentGameState);

    if (mRenderNeeded) {
      mWindow->display();
    }

    sf::sleep(sf::milliseconds(FRAME_LENGTH_IN_MS));

    handleGameStateMessages();
    handleSystemMessages();
    handleDebugMessages();
  }
  mWindowOpen = false;
}

void Gui::handleWindowEvents() {
  sf::Event event;
  
  while (mWindow->pollEvent(event)) {
    if (event.type == sf::Event::Closed) {
      sf::Packet shutdownMessage;
      shutdownMessage << SHUT_DOWN;
      MessageHandler::get().pushSystemMessage(shutdownMessage);
    }
    if (event.type == sf::Event::MouseMoved) {
      MouseMessage mm(sf::Mouse::getPosition(*mWindow));
      mMouseInterface->pushMessage(mm.pack());
    }
    if (event.type == sf::Event::Resized) {
      sf::FloatRect visibleArea(0, 0, (float)event.size.width, (float)event.size.height);
      mWindow->setView(sf::View(visibleArea));
    }
    if (event.type == sf::Event::TextEntered && mWindow->hasFocus()) {
      auto it = mMenuMap.find(mCurrentGameState);
      if (it != mMenuMap.end()) {
        it->second->handleNewText(event.text.unicode);
      }
    }
  }

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::F1)) {
    if (!mF1KeyAlreadyPressed) {
      mShowDebugMenu = !mShowDebugMenu;
      mF1KeyAlreadyPressed = true;
    }
  } else {
    mF1KeyAlreadyPressed = false;
  }
}

bool Gui::renderGameState(GAME_STATE gameState) {
  mRenderNeeded = false;

  auto it = mMenuMap.find(gameState);

  if (it == mMenuMap.end()) {
    TRACE_ERROR("GAME_STATE: " << gameState << " is not implemented");
    return mRenderNeeded;
  }

  mWindow->clear(sf::Color::White);
  sf::Vector2f mousePosition = mWindow->mapPixelToCoords(sf::Mouse::getPosition(*mWindow));
  if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && mWindow->hasFocus()) {
    if (!mLeftButtonAlreadyPressed) {
      mMenuMap.at(gameState)->checkMouse(mousePosition);
      if (mShowDebugMenu) {
        mDebugMenu->checkMouse(mousePosition);
      }
    }
    mLeftButtonAlreadyPressed = true;
  } else {
    mLeftButtonAlreadyPressed = false;
  }
  mRenderNeeded = mMenuMap.at(gameState)->render(mWindow, mousePosition);
  if (mShowDebugMenu) {
    mDebugMenu->render(mWindow, mousePosition);
  }
  return mRenderNeeded;
}

void Gui::handleGameStateMessages() {
  std::queue<sf::Packet> gameStateMessageQueue = mGameStateMessageSubscriber.getMessageQueue();
  sf::Packet gameStateMessage;
  while (!gameStateMessageQueue.empty()) {
    gameStateMessage = gameStateMessageQueue.front();
    gameStateMessageQueue.pop();

    GameStateMessage gsm;
    gsm.unpack(gameStateMessage);
    if (gsm.getGameState() != mCurrentGameState) {
      // Changed game state
      auto previousMenu = mMenuMap.find(mCurrentGameState);
      if (previousMenu != mMenuMap.end()) {
        previousMenu->second->uninit();
      }

      mCurrentGameState = gsm.getGameState();
      auto newMenu = mMenuMap.find(mCurrentGameState);
      if (newMenu != mMenuMap.end()) {
        newMenu->second->init();
      }

      mWindow->clear(sf::Color::White);
      mWindow->display();
    }
  }
}

void Gui::handleSystemMessages() {
  std::queue<sf::Packet> systemMessageQueue = mSystemMessageSubscriber.getMessageQueue();
  sf::Packet systemMessage;
  while (!systemMessageQueue.empty()) {
    systemMessage = systemMessageQueue.front();
    systemMessageQueue.pop();

    auto messageId = 0;
    systemMessage >> messageId;
    switch (messageId) {
    case SHUT_DOWN:
      TRACE_INFO("Closing GUI window");
      mWindow->close();
      break;
    default:
      TRACE_WARNING("Unknown system message : " << messageId);
    }
  }
}

void Gui::handleDebugMessages() {
  std::queue<sf::Packet> debugMessageQueue = mDebugSubscriber.getMessageQueue();
  sf::Packet debugMessage;
  while (!debugMessageQueue.empty()) {
    debugMessage = debugMessageQueue.front();
    debugMessageQueue.pop();
    TRACE_DEBUG("Toggle debug traces");
    mDebugEnabled = !mDebugEnabled;
    TRACE_DEBUG("Toggle debug traces");
  }
}

void Gui::shutDown() {
  TRACE_INFO("Shutdown of module requested...");
  while (mWindowOpen) {
    // Wait for GUI to close window
  }
  mGuiThread->join();
  TRACE_INFO("Shutdown of module done");
}
#include "GameStateMessage.h"

GameStateMessage::GameStateMessage() :
  mGameState(GAME_STATE::NO_STATE) { }

GameStateMessage::GameStateMessage(GAME_STATE newGameState) :
  mGameState(newGameState) { }

sf::Packet GameStateMessage::pack() {
  sf::Packet packet;
  packet << mGameState;
  return packet;
}

void  GameStateMessage::unpack(sf::Packet packet) {
  int gameState;
  packet >> gameState;
  mGameState = (GAME_STATE)gameState;
}

GAME_STATE GameStateMessage::getGameState() {
  return mGameState; 
}
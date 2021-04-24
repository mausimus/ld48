#pragma once

#include "Game.h"
#include "Map.h"

constexpr float PLAYER_X_SPEED = 10.0f;
constexpr float PLAYER_Y_SPEED = 10.0f;
constexpr float PLAYER_Z_SPEED = 25.0f;

class TestGame : public Game
{
protected:
    virtual void CreateScreens();
    virtual void OnTick(double deltaTime, double totalTime);
    virtual void OnSwitchScreen(int prevScreen, int nextScreen);

public:
    float playerX;
    float playerY;
    float playerZ;

    float vX;
    float vY;
    float vZ;

    Map m_map;
    TestGame();
};
#pragma once

#include "Game.h"
#include "Map.h"

constexpr float PLAYER_X_SPEED = 10.0f;
constexpr float PLAYER_Y_SPEED = 10.0f;
constexpr float PLAYER_Z_SPEED = 25.0f;

constexpr float MAX_SPEED = 50.0f;

constexpr float LEAN_MAX = 20.0f;

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

    float brakeAmount;
    float weightAmount;

    float weightForce;

    float speed; // based on acceleration (slope + brakeAmount)

    float grossInertia; // curvature * speed
    float netInertia; // curvature * speed - weightAmount

/*
    float aX;
    float aY;
    float aZ;

    float vX; // lean velocity?
    float leanX;
    float vY;
    float vZ; // our velocity    
*/
    float trackCurvature;
    float trackSlope;

    Map m_map;
    TestGame();
};
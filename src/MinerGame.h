#pragma once

#include "Game.h"
#include "Map.h"

constexpr float PLAYER_X_SPEED = 10.0f;
constexpr float PLAYER_Y_SPEED = 10.0f;
constexpr float PLAYER_Z_SPEED = 25.0f;

constexpr float MAX_SPEED = 50.0f;

constexpr float LEAN_MAX = 20.0f;

constexpr int MAX_MINERS = 6;

constexpr float MAX_INERTIA = 1.2f;

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

    float trackCurvature;
    float trackSlope;

    bool isDucking;

    int rescuedMiners;

    bool isPaused;

    float raceTime;
    bool raceStarted;

    bool ignoreBeams;

    Map m_map;
    TestGame();

    void Reset();
};
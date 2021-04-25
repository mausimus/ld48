#include "MinerGame.h"
#include "MinerScreen.h"

TestGame::TestGame() : Game()
{
    Reset();
    m_map.Generate();
}

void TestGame::Reset()
{
    playerX        = 0;
    playerY        = 0;
    playerZ        = 0;
    brakeAmount    = 1.0f;
    weightAmount   = 0;
    weightForce    = 0;
    speed          = 0;
    netInertia     = 0;
    grossInertia   = 0;
    trackSlope     = 0;
    trackCurvature = 0;
    rescuedMiners  = 0;
    isDucking      = false;
    isPaused       = true;
    raceStarted    = false;
    raceTime       = 0;
    ignoreBeams    = false;
    m_map.Reset();
}

void TestGame::CreateScreens()
{
    int sn = 0;
    m_screens.push_back(new TestScreen(sn++, this));
}

void TestGame::OnTick(double deltaTime, double totalTime) { }

void TestGame::OnSwitchScreen(int prevScreen, int nextScreen) { }
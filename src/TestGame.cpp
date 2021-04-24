#include "TestGame.h"
#include "TestScreen.h"

TestGame::TestGame() : Game()
{
    m_map.Generate();
    playerX = 0;
    playerY = 0;
    playerZ = 0;
    vX = vY = vZ = 0;
}

void TestGame::CreateScreens()
{
    int sn = 0;
    m_screens.push_back(new TestScreen(sn++, this));
}

void TestGame::OnTick(double deltaTime, double totalTime)
{    
}

void TestGame::OnSwitchScreen(int prevScreen, int nextScreen)
{    
}
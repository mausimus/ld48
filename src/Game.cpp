#include "Game.h"
#include "Assets.h"
#include "Screen.h"

#include "TestScreen.h"

Game::Game() : m_activeScreen {nullptr}, m_frameCounter {}, m_mouseX {}, m_mouseY {} { }

void Game::LoadScreens()
{
    m_assets = new Assets();
    m_assets->Load();

    CreateScreens();

    for(auto& s : m_screens)
    {
        s->Load();
    }
}

void Game::Start()
{
    LoadScreens();
    SwitchScreen(0);
}

void Game::End()
{
    if(m_activeScreen)
        m_activeScreen->Exit();

    for(auto& s : m_screens)
    {
        s->Unload();
        delete s;
    }

    m_assets->Unload();
}

void Game::Tick(double deltaTime, double totalTime)
{
    m_frameCounter++;
    m_mouseX = GetMouseX();
    m_mouseY = GetMouseY();

    OnTick(deltaTime, totalTime);
    m_activeScreen->Tick(deltaTime, totalTime);
}

void Game::DrawBackground()
{
    m_activeScreen->DrawBackground();
}

void Game::DrawPixels(Color* framebuffer, Rectangle* rect, bool* fullFrame)
{
    m_activeScreen->DrawPixels(framebuffer, rect, fullFrame);
}

void Game::DrawShapes()
{
    m_activeScreen->DrawShapes();

    DrawCircle(m_mouseX, m_mouseY, 5, MAROON); // mouse cursor
}

void Game::SwitchScreen(int sc)
{
    if(m_activeScreen)
        m_activeScreen->Exit();

    OnSwitchScreen(m_activeScreen ? m_activeScreen->m_no : -1, sc);

    m_activeScreen = m_screens[sc];
    m_screenTime   = GetTime();
    m_activeScreen->Enter();
}
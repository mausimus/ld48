#pragma once

#include "TestGame.h"
#include "Assets.h"

class Screen
{
protected:
    TestGame *m_game;

    inline Assets* Assets() { return m_game->m_assets; }

public:
    int m_no;

    Screen(int no, TestGame* game);
    virtual void Load() = 0;
    virtual void Enter() = 0;
    virtual void Tick(double deltaTime, double totalTime) = 0;
    virtual void DrawBackground() = 0;
    virtual void DrawPixels(Color* framebuffer, Rectangle *rect, bool* fullFrame) = 0;
    virtual void DrawShapes() = 0;
    virtual void Exit() = 0;
    virtual void Unload() = 0;
    virtual ~Screen();
};
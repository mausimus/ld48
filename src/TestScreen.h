#include "Screen.h"
#include "TestGame.h"

constexpr int viewCenterX = 160;
constexpr int viewCenterY = 60;
constexpr int viewWidth   = 310;
constexpr int viewHeight  = 110;
constexpr int viewLeft    = viewCenterX - (viewWidth / 2);
constexpr int viewRight   = viewCenterX + (viewWidth / 2);
constexpr int viewTop     = viewCenterY - (viewHeight / 2);
constexpr int viewBottom  = viewCenterY + (viewHeight / 2);

class TestScreen : public Screen
{
public:
    TestScreen(int no, TestGame* game);
    virtual void Load();
    virtual void Enter();
    virtual void Tick(double deltaTime, double totalTime);
    virtual void DrawBackground();
    virtual void DrawPixels(Color* framebuffer, Rectangle* rect, bool* fullFrame);
    virtual void DrawShapes();
    virtual void Exit();
    virtual void Unload();
};

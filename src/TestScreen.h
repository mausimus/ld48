#include "Screen.h"
#include "TestGame.h"

constexpr int drawDistance = 13;

constexpr int viewCenterX = 130;
constexpr int viewCenterY = 85;
constexpr int viewWidth   = 250;
constexpr int viewHeight  = 160;
constexpr int viewLeft    = viewCenterX - (viewWidth / 2);
constexpr int viewRight   = viewCenterX + (viewWidth / 2);
constexpr int viewTop     = viewCenterY - (viewHeight / 2);
constexpr int viewBottom  = viewCenterY + (viewHeight / 2);

constexpr int hudLeft = viewLeft;
constexpr int hudTop = viewBottom + 10;

constexpr int mapTop = 5;
constexpr int mapLeft = 260;
constexpr int mapWidth = 50;
constexpr int mapHeight = 148;

constexpr int trainTop = viewBottom + 11;
constexpr int trainLeft = 190;
constexpr int trainWidth = 70;
constexpr int trainHeight = 10;

constexpr int train2Left = 60;
constexpr int barsStart = trainTop + 2;
constexpr int barsHeight = 8;

class TestScreen : public Screen
{
protected:
    void DrawMap();
    void DrawTrain();
    void DrawTimer();

    float raceTime;

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

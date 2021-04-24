#include "TestScreen.h"

int x  = screenWidth / 2;
int y  = screenHeight / 2;
int _x = 0;

Animator  animator("Test", 2, 1, 5);
Texture2D cloudsTexture;
Font      font;
Sound     fxOgg;

TestScreen::TestScreen(int no, TestGame* game) : Screen(no, game) { }

void TestScreen::Load()
{
    cloudsTexture = LoadTexture("resources/clouds.png");
    //font          = LoadFontEx("resources/test1.ttf", 6, 0, 0);
    //font          = LoadFontEx("resources/fonts/prstartk.ttf", 8, 0, 0);
    font = LoadFontEx("resources/fonts/Px437_Portfolio_6x8.ttf", 8, 0, 0);
    //font          = LoadFontEx("resources/fonts/joystix.ttf", 16, 0, 0);
    //font          = LoadFont("resources/fonts/alpha_beta.png");
    fxOgg = LoadSound("resources/sound.ogg");
    animator.AssignSprite(Assets()->spriteTexture);
}

void TestScreen::Enter() { }

void TestScreen::Exit() { }

void TestScreen::Unload()
{
    UnloadTexture(cloudsTexture);
    UnloadFont(font);
}

void TestScreen::Tick(double deltaTime, double totalTime)
{
    float aX = 0;
    float aY = 0;
    float aZ = 0;

    if(IsKeyDown(KEY_UP))
        aY += 1.0f;
    if(IsKeyDown(KEY_DOWN))
        aY -= 1.0f;
    if(IsKeyDown(KEY_LEFT))
        aX -= 1.0f;
    if(IsKeyDown(KEY_RIGHT))
        aX += 1.0f;
    if(IsKeyDown(KEY_A))
        aZ += 1.0f;
    if(IsKeyDown(KEY_Z))
        aZ -= 1.0f;

    m_game->vX += aX;
    m_game->vY += aY;
    m_game->vZ += aZ;

    m_game->playerX += deltaTime * m_game->vX;
    m_game->playerY += deltaTime * m_game->vY;
    m_game->playerZ += deltaTime * m_game->vZ;
}

void TestScreen::DrawBackground()
{
    DrawTexture(cloudsTexture, 0, 0, WHITE);
}

void TestScreen::DrawPixels(Color* framebuffer, Rectangle* rect, bool* fullFrame)
{
    /*    *fullFrame = false;
    rect->y = 190.0f;
    rect->height = 50.0f;
    rect->x = 0.0f;
    rect->width = screenWidth;
    framebuffer[_x++] = GREEN;*/
}

float PerspectiveScale(float distance)
{
    if(distance <= 0)
        return 0;

    //return 1.0f / distance;
    return 1.0f / (0.75f + (distance / 4.0f));
}

/*float PerspectiveOffset(float offset)
{
    if(distance <= 0)
        return 0;

    //return 1.0f / distance;
    return 1.0f / (0.5f + (distance / 2.0f));
}*/

void TestScreen::DrawShapes()
{
    // player location will be a float

    Vector2 fan[SECTION_POINTS + 2];
    DrawPixel(viewCenterX, viewCenterY, GREEN);

    // we draw 14 sections from player location forward
    auto drawStart = static_cast<int>(m_game->playerZ) + 1;
    auto drawEnd   = drawStart + 13;

    DrawRectangle(viewLeft, viewTop, viewWidth, viewHeight, Assets()->walls[0]);

    std::vector<Vector2> leftTracks;
    std::vector<Vector2> rightTracks;

    const auto &firstSection = m_game->m_map.m_sections[drawStart - 1];
    const auto &secondSection = m_game->m_map.m_sections[drawStart];
    float a = m_game->playerZ - floorf(m_game->playerZ);        
    m_game->playerX = firstSection.m_dx + (secondSection.m_dx - firstSection.m_dx) * a;
    m_game->playerY = firstSection.m_dy + (secondSection.m_dy - firstSection.m_dy) * a;

    for(int i = drawStart; i < drawEnd; i++)
    {
        float scale          = PerspectiveScale(i - m_game->playerZ);
        auto  section        = m_game->m_map.m_sections[i];
        auto  adjustedCenterX = viewCenterX - (m_game->playerX - section.m_dx) * scale * 10.0f;
        auto  adjustedCenterY = viewCenterY + (m_game->playerY - section.m_dy) * scale * 10.0f;
        section.GenerateTriangleFan(
            fan, adjustedCenterX, adjustedCenterY, scale, section.m_ecc);
        DrawTriangleFan(fan, SECTION_POINTS + 2, Assets()->walls[i - drawStart + 1]);

        Vector2 leftTrack, rightTrack;
        section.TrackPosition(&leftTrack, &rightTrack, adjustedCenterX, adjustedCenterY, scale, section.m_ecc);
        leftTracks.push_back(leftTrack);
        rightTracks.push_back(rightTrack);
    }

    int numSections = drawEnd - drawStart - 1;
    for(int i = 0; i < numSections; i++)
    {
        DrawLineEx(leftTracks[i], leftTracks[i + 1], 1 + (numSections - i) / 3, GRAY);
        DrawLineEx(rightTracks[i], rightTracks[i + 1], 1 + (numSections - i) / 3, GRAY);
    }

    // train tracks
    /*for(int i = drawStart; i < drawEnd - 1; i++)
    {
        float scale          = PerspectiveScale(i - m_game->playerZ);
        auto  adjustedCenterX = viewCenterX - (m_game->playerX - section.m_dx) * scale * 10.0f;
        auto  adjustedCenterY = viewCenterY + (m_game->playerY - section.m_dy) * scale * 10.0f;

        Vector2 startPosLeft, startPosRight, endPosLeft, endPosRight;
        auto  section1        = m_game->m_map.m_sections[i];
        auto  section2        = m_game->m_map.m_sections[i + 1];
        section1.TrackPosition(startPosLeft, startPosRight, 
        DrawLineEx()


        /*float scale   = PerspectiveScale(i - m_game->playerZ);
        auto  section = m_game->m_map.m_sections[i];
        auto  adjustedCenter = viewCenterX - (m_game->playerX - section.m_dx) * scale * 10.0f;*/
    

    // trim view
    DrawRectangle(0, 0, screenWidth, viewTop, BLACK);
    DrawRectangle(0, 0, viewLeft, screenHeight, BLACK);
    DrawRectangle(0, viewBottom, screenWidth, screenHeight - viewBottom, BLACK);
    DrawRectangle(viewRight, 0, screenWidth - viewRight, screenHeight, BLACK);

    //DrawTextureRec(animator.GetSprite(), animator.GetFrameRec(), (Vector2) {static_cast<float>(x), static_cast<float>(y)}, WHITE);
    //DrawTextEx(font, "Score SCORE 134234", (Vector2) {5, 5}, (float)font.baseSize, 1.0f, Assets()->palette[63]);
}

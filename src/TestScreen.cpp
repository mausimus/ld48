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
    animator.AssignSprite(Assets()->minerTexture);
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
    m_game->aY = 0;
    m_game->aX = 0;
    m_game->aZ = 0;

    if(IsKeyDown(KEY_UP))
        m_game->aY += 1.0f;
    if(IsKeyDown(KEY_DOWN))
        m_game->aY -= 1.0f;

    if(IsKeyDown(KEY_LEFT))
        m_game->aX -= 1.0f;
    if(IsKeyDown(KEY_RIGHT))
        m_game->aX += 1.0f;

    if(IsKeyDown(KEY_A))
        m_game->aZ += 1.0f;
    if(IsKeyDown(KEY_Z))
        m_game->aZ -= 1.0f;

    m_game->vX += m_game->aX;
    m_game->vY += m_game->aY;
    m_game->vZ += m_game->aZ;

    if(m_game->vZ < 0)
        m_game->vZ = 0;
    if(m_game->vZ > MAX_SPEED)
        m_game->vZ = MAX_SPEED;

    m_game->leanX += deltaTime * m_game->vX;
    if(m_game->leanX > LEAN_MAX)
    {
        m_game->leanX = LEAN_MAX;
        m_game->vX = 0;
    }
    else if(m_game->leanX < -LEAN_MAX)
    {
        m_game->leanX = -LEAN_MAX;
        m_game->vX = 0;
    }

    m_game->playerZ += deltaTime * m_game->vZ;

    // loop
    if(m_game->playerZ > MAP_LENGTH - 20)
        m_game->playerZ = 0;

    // update camera location and track curvature
    auto drawStart = static_cast<int>(m_game->playerZ) + 1;
    const auto &firstSection = m_game->m_map.m_sections[drawStart - 1];
    const auto &secondSection = m_game->m_map.m_sections[drawStart];
    float a = m_game->playerZ - floorf(m_game->playerZ);        
    m_game->playerX = firstSection.m_dx + (secondSection.m_dx - firstSection.m_dx) * a;
    m_game->playerY = firstSection.m_dy + (secondSection.m_dy - firstSection.m_dy) * a;

    m_game->trackCurvature = secondSection.m_dx - firstSection.m_dx;
    m_game->trackSlope = secondSection.m_dy - firstSection.m_dy;

    raceTime = totalTime;
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
    auto drawEnd   = drawStart + drawDistance;

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

    for(int i = 0; i < numSections; i++)
    {
        auto sectionNo = drawStart + i;
        //if(sectionNo % 10 == 0)
        if(m_game->m_map.m_sections[drawStart + i].m_hasMiner)
        {
            //DrawPixel(leftTracks[i].x, leftTracks[i].y, GREEN);
            //DrawTextureRec(animator.GetSprite(), animator.GetFrameRec(), (Vector2){leftTracks[i].x, leftTracks[i].y - 32}, WHITE);
            auto size = ((64 - (i * 4)) >> 3) << 3;
            DrawTexturePro(animator.GetSprite(), animator.GetFrameRec(), (Rectangle){leftTracks[i].x, leftTracks[i].y - size, size, size}, (Vector2){0.0}, 0, WHITE);
        }
    }

    // trim view
/*    DrawRectangle(0, 0, screenWidth, viewTop, BLACK);
    DrawRectangle(0, 0, viewLeft, screenHeight, BLACK);
    DrawRectangle(0, viewBottom, screenWidth, screenHeight - viewBottom, BLACK);
    DrawRectangle(viewRight, 0, screenWidth - viewRight, screenHeight, BLACK);*/

    //DrawTextEx(font, "Score SCORE 134234", (Vector2) {5, 5}, (float)font.baseSize, 1.0f, Assets()->palette[63]);

    // draw handle (45,105 -> 10,115)
    // 36->163
    float factor = floorf(5 * m_game->vZ / MAX_SPEED) / 5.0f;
    float topX = 15 + (45 - 15) * factor;
    float topY = 115 + (105 - 115) * factor;
    DrawLineEx((Vector2){topX, topY}, (Vector2){36, 183}, 5, Assets()->palette[4]);

    DrawRectangle(hudLeft, hudTop, static_cast<int>(fabsf(m_game->vZ)), 10, RED); // player speed
    //DrawRectangle(hudLeft, hudTop + 10, static_cast<int>(fabsf(m_game->leanX)), 10, YELLOW); // lean
    DrawRectangle(hudLeft, hudTop + 20, static_cast<int>(fabsf(m_game->trackCurvature * 30)), 10, GREEN); // track curvature

    DrawTexture(Assets()->cartTexture, 0, 0, WHITE);

    DrawMap();
    DrawTrain();
    DrawTimer();
}

void TestScreen::DrawMap()
{
    //DrawRectangleLines(mapLeft, mapTop, mapWidth, mapWidth, YELLOW);
    
    auto drawStart = static_cast<int>(m_game->playerZ) + 1;
    for(int i = 0; i < mapHeight; i++)
    {
        const auto& section = m_game->m_map.m_sections[drawStart + i * 2];
        const auto& section1 = m_game->m_map.m_sections[drawStart + i * 2 + 1];
        auto dx = static_cast<int>((section.m_dx - m_game->playerX) * 0.1f) << 1;
        if(dx > -mapWidth / 2 && dx < mapWidth / 2)
        {
            auto xPos = static_cast<int>(mapLeft + (mapWidth / 2) + dx);
            DrawPixel(xPos - 3, mapTop + mapHeight - i, GRAY);
            DrawPixel(xPos - 4, mapTop + mapHeight - i, GRAY);
            DrawPixel(xPos + 0, mapTop + mapHeight - i, GRAY);
            DrawPixel(xPos + 1, mapTop + mapHeight - i, GRAY);

            if(section.m_hasMiner || section1.m_hasMiner)
            {
                DrawPixel(xPos + 4, mapTop + mapHeight - i, MAROON);
                DrawPixel(xPos + 5, mapTop + mapHeight - i, MAROON);  
            }

            if(section.m_hasSwitch || section1.m_hasSwitch)
            {
                DrawPixel(xPos - 7, mapTop + mapHeight - i, GOLD);
                DrawPixel(xPos - 8, mapTop + mapHeight - i, GOLD);  
            }
        }
    }
}

void TestScreen::DrawTrain()
{
    float curvature = m_game->trackCurvature;
    float slope = m_game->trackSlope;
    float brake = m_game->vZ;
    float netSpeed = slope - brake;
    float netForce = curvature * netSpeed - m_game->vX;

    DrawTexturePro(Assets()->cartSprite, (Rectangle){0,0,24,24}, (Rectangle){trainLeft, trainTop + 6, 24, 24}, (Vector2){12,12}, curvature * 10, WHITE);
    DrawRectangle(trainLeft + 20, barsStart, m_game->trackCurvature * 10, barsHeight, MAROON);
    DrawRectangle(trainLeft - 20 - fabs(m_game->leanX), barsStart, fabs(m_game->leanX) * 10, barsHeight, MAROON);    

    DrawTexturePro(Assets()->cartSprite, (Rectangle){24,0,48,24}, (Rectangle){train2Left, trainTop + 6, 48, 24}, (Vector2){24,12}, speed / 6.0f, WHITE);
    DrawRectangle(train2Left + 24, barsStart, speed, barsHeight, MAROON);
}

void TestScreen::DrawTimer()
{   
    auto elapsedSeconds = raceTime;
    DrawTextEx(font, std::to_string(elapsedSeconds).c_str(), (Vector2){14, 14}, 8, 1, WHITE);
}
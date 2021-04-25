#include "TestScreen.h"

int x  = screenWidth / 2;
int y  = screenHeight / 2;
int _x = 0;

Animator  animator("Test", 2, 2, 5);
Texture2D cloudsTexture;
Font      font;
Sound     fxOgg;
bool      showStats;

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

void TestScreen::Enter()
{
    raceTime    = 0;
    raceStarted = false;
}

void TestScreen::Exit() { }

void TestScreen::Unload()
{
    UnloadTexture(cloudsTexture);
    UnloadFont(font);
}

void TestScreen::Tick(double deltaTime, double totalTime)
{
    if(IsKeyPressed(KEY_S))
        showStats = !showStats;

    m_game->isDucking = IsKeyDown(KEY_DOWN);

    if(raceStarted)
        raceTime = totalTime - raceStart;

    // A and Z control *BRAKE* amount
    if(IsKeyDown(KEY_A))
    {
        m_game->brakeAmount -= 0.05f;
        raceStarted = true;
        raceStart   = totalTime;
    }
    else if(raceStarted)
    {
        // auto release
        m_game->brakeAmount -= 0.05f;
    }

    if(IsKeyDown(KEY_Z))
        m_game->brakeAmount += 0.25f;
    if(m_game->brakeAmount > 1)
        m_game->brakeAmount = 1;
    if(m_game->brakeAmount < 0)
        m_game->brakeAmount = 0;

    // left and right move WEIGHT: as you hold direction it increases
    /*if(IsKeyDown(KEY_LEFT))
    {
        m_game->weightForce -= 0.1f;
    } else if(IsKeyDown(KEY_RIGHT))
    {
        m_game->weightForce += 0.1f;
    } else 
    {
        m_game->weightForce = 0;
    }
    if(m_game->weightForce < -1)
        m_game->weightForce = -1;
    if(m_game->weightForce > 1)
        m_game->weightForce = 1;

    // by 10
    m_game->weightAmount += m_game->weightForce / 10.0f;
    m_game->weightAmount *= 0.9f;*/

    if(m_game->weightAmount > 1.0f)
        m_game->weightAmount = 1.0f;
    if(m_game->weightAmount < -1.0f)
        m_game->weightAmount = -1.0f;

    float netAccel = m_game->trackSlope; // * (1.0f - m_game->brakeAmount); // if slope negative we go backwards - that's death?
    m_game->speed += netAccel; // speed
    if(m_game->speed > 0)
    {
        m_game->speed -= m_game->brakeAmount * 0.5f;
        if(m_game->speed < 0)
            m_game->speed = 0;
    }
    else
    {
        m_game->speed += m_game->brakeAmount * 0.5f;
        if(m_game->speed > 0)
            m_game->speed = 0;
    }

    // fixed friction
    m_game->speed *= 0.9999f;

    m_game->grossInertia = fabsf(m_game->speed /** m_game->speed*/) * m_game->trackCurvature;

    m_game->weightAmount = 0;
    if(IsKeyDown(KEY_LEFT))
    {
        m_game->weightAmount -= 1;
    }
    if(IsKeyDown(KEY_RIGHT))
    {
        m_game->weightAmount = 1;
    }

    m_game->netInertia = m_game->grossInertia;
    if(m_game->grossInertia > 0 && m_game->weightAmount < 0)
    {
        m_game->netInertia *= 0.5;
    }
    else if(m_game->grossInertia < 0 && m_game->weightAmount > 0)
    {
        m_game->netInertia *= 0.5;
    }

    /*    if(IsKeyDown(KEY_LEFT))
    {
        if(m_game->weightAmount < 0)
        {
            // already fixed
        }
        else if(m_game->grossInertia > 0)
        {
            // once you press left, it fixes right inertia at that max level
            m_game->weightAmount = -m_game->grossInertia;
        }
    }
    else if(IsKeyDown(KEY_RIGHT))
    {
        if(m_game->weightAmount > 0)
        {
            // already fixed
        }
        else if(m_game->grossInertia < 0)
        {
            // once you press left, it fixes right inertia at that max level
            m_game->weightAmount = -m_game->grossInertia;
        }
    }
    else
    {
        m_game->weightAmount = 0;
    }

    if(m_game->weightAmount > 0 && m_game->grossInertia < 0)
    {
        m_game->netInertia = -m_game->weightAmount;
    }
    else if(m_game->weightAmount < 0 && m_game->grossInertia > 0)
    {
        m_game->netInertia = -m_game->weightAmount;
    }
    else
    {
        m_game->netInertia = m_game->grossInertia;
    }*/

    //    m_game->netInertia
    //m_game->netInertia = fabsf(m_game->speed * m_game->speed) * (m_game->trackCurvature + m_game->weightAmount * 0.1f);

    //    m_game->grossInertia = fabsf(m_game->speed) * m_game->trackCurvature;
    //    m_game->netInertia = fabsf(m_game->speed) * (m_game->trackCurvature + m_game->weightAmount * 0.1f);

    // move forward - based on NET speed
    auto prevPosition = m_game->playerZ;
    m_game->playerZ += deltaTime * m_game->speed;
    if(m_game->playerZ < 0)
        m_game->playerZ = 0;        

    // play track sounds?    
    auto intPosition = static_cast<int>(m_game->playerZ);
    if(static_cast<int>(prevPosition) != intPosition)
    {
        // we moved into new pos
        if(intPosition % 7 == 0)
        {
            PlaySoundMulti(Assets()->tracks2);
        }
        else if(intPosition % 7 == 5)
        {
            PlaySoundMulti(Assets()->tracks1);
        }
    }

    // loop
    if(m_game->playerZ > MAP_LENGTH - 20)
        m_game->playerZ = 0;

    // update camera location and track curvature
    auto  drawStart     = static_cast<int>(m_game->playerZ) + 1;
    auto& firstSection  = m_game->m_map.m_sections[drawStart - 1];
    auto& secondSection = m_game->m_map.m_sections[drawStart];
    float a             = m_game->playerZ - floorf(m_game->playerZ);
    m_game->playerX     = firstSection.m_dx + (secondSection.m_dx - firstSection.m_dx) * a;
    m_game->playerY     = firstSection.m_dy + (secondSection.m_dy - firstSection.m_dy) * a;

    m_game->trackCurvature = (secondSection.m_dx - firstSection.m_dx) / 10.0f;
    m_game->trackSlope     = -(secondSection.m_dy - firstSection.m_dy - 0.95f) / 10.0f; // (almost) constant downward

    // are we close to any miners?
    if(m_game->speed == 0)
    {
        for(int m = 0; m < 5; m++)
        {
            auto& minerSection = m_game->m_map.m_sections[drawStart - 1 + m];
            if(minerSection.m_hasMiner)
            {
                if(m_game->rescuedMiners < MAX_MINERS)
                {
                    m_game->rescuedMiners++;
                    minerSection.m_hasMiner = false;
                }
                else
                {
                    // no more room!
                }
            }
        }
    }
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
    animator.Play();

    // player location will be a float

    int centerY = viewCenterY;

    Vector2 fan[SECTION_POINTS + 2];
    if(rand() % 4 == 0)
    {
        centerY += rand() % 2;
    }

    //DrawPixel(viewCenterX, viewCenterY, GREEN);

    // we draw 14 sections from player location forward
    auto drawStart = static_cast<int>(m_game->playerZ) + 1;
    auto drawEnd   = drawStart + drawDistance;

    DrawRectangle(viewLeft, viewTop, viewWidth, viewHeight, Assets()->walls[0]);

    std::vector<Vector2>              leftTracks;
    std::vector<Vector2>              rightTracks;
    std::vector<Vector2>              leftBars;
    std::vector<Vector2>              rightBars;
    std::vector<std::vector<Vector2>> supports;

    const auto& firstSection  = m_game->m_map.m_sections[drawStart - 1];
    const auto& secondSection = m_game->m_map.m_sections[drawStart];
    float       a             = m_game->playerZ - floorf(m_game->playerZ);
    m_game->playerX           = firstSection.m_dx + (secondSection.m_dx - firstSection.m_dx) * a;
    m_game->playerY           = firstSection.m_dy + (secondSection.m_dy - firstSection.m_dy) * a;

    for(int i = drawStart; i < drawEnd; i++)
    {
        float scale           = PerspectiveScale(i - m_game->playerZ);
        auto  section         = m_game->m_map.m_sections[i];
        auto  adjustedCenterX = viewCenterX - (m_game->playerX - section.m_dx) * scale * 10.0f;
        auto  adjustedCenterY = centerY + (m_game->playerY - section.m_dy) * scale * 10.0f;
        section.GenerateTriangleFan(fan, adjustedCenterX, adjustedCenterY, scale, section.m_ecc);
        DrawTriangleFan(fan, SECTION_POINTS + 2, Assets()->walls[i - drawStart + 1]);

        Vector2 leftTrack, rightTrack;
        section.TrackPosition(&leftTrack, &rightTrack, adjustedCenterX, adjustedCenterY, scale, section.m_ecc);
        leftTracks.push_back(leftTrack);
        rightTracks.push_back(rightTrack);

        std::vector<Vector2> supportsPosition(4);
        section.SupportsPosition(supportsPosition.data(), adjustedCenterX, adjustedCenterY, scale, section.m_ecc);
        supports.push_back(supportsPosition);

        Vector2 leftBar, rightBar;
        section.BarPosition(&leftBar, &rightBar, adjustedCenterX, adjustedCenterY, scale, section.m_ecc);
        leftBars.push_back(leftBar);
        rightBars.push_back(rightBar);
    }

    int numSections = drawEnd - drawStart - 1;
    for(int i = 0; i < numSections; i++)
    {
        int thickNess = 1 + (drawDistance - i) / 6;
        DrawLineEx(leftBars[i], rightBars[i], thickNess, BROWN);
    }

    for(int i = 0; i < numSections; i++)
    {
        DrawLineEx(leftTracks[i], leftTracks[i + 1], 1 + (numSections - i) / 3, GRAY);
        DrawLineEx(rightTracks[i], rightTracks[i + 1], 1 + (numSections - i) / 3, GRAY);
        if((drawStart + i) % 10 == 0)
        {
            int thickNess = 1 + (drawDistance - i);
            DrawLineEx(supports[i][0], supports[i][1], thickNess, MAROON);
            DrawLineEx(supports[i][1], supports[i][2], thickNess, MAROON);
            DrawLineEx(supports[i][2], supports[i][3], thickNess, MAROON);
        }
    }

    for(int i = 0; i < numSections; i++)
    {
        auto sectionNo = drawStart + i;
        //if(sectionNo % 10 == 0)
        if(m_game->m_map.m_sections[drawStart + i].m_hasMiner)
        {
            //DrawPixel(leftTracks[i].x, leftTracks[i].y, GREEN);
            //DrawTextureRec(animator.GetSprite(), animator.GetFrameRec(), (Vector2){leftTracks[i].x, leftTracks[i].y - 32}, WHITE);
            auto size = ((64 - (i * 4)) >> 1) << 1;
            DrawTexturePro(animator.GetSprite(),
                           animator.GetFrameRec(),
                           (Rectangle) {leftTracks[i].x, leftTracks[i].y - size, size, size},
                           (Vector2) {0.0},
                           0,
                           WHITE);
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
    float factor = floorf(5 * m_game->brakeAmount) / 5.0f;
    float topX   = 15 + (45 - 15) * factor;
    float topY   = 115 + (105 - 115) * factor;
    DrawLineEx((Vector2) {topX, topY}, (Vector2) {36, 183}, 5, Assets()->palette[4]);

    //DrawRectangle(hudLeft, hudTop, static_cast<int>(fabsf(m_game->vZ)), 10, RED); // player speed
    //DrawRectangle(hudLeft, hudTop + 10, static_cast<int>(fabsf(m_game->leanX)), 10, YELLOW); // lean
    //DrawRectangle(hudLeft, hudTop + 20, static_cast<int>(fabsf(m_game->trackCurvature * 30)), 10, GREEN); // track curvature

    DrawTexture(Assets()->cartTexture, 0, 0, WHITE);
    if(m_game->isDucking)
        DrawTexture(Assets()->duckTexture, 0, 0, WHITE);      

    DrawMap();
    DrawTrain();
    DrawTimer();
    DrawMiners();
}

void TestScreen::DrawMap()
{
    //DrawRectangleLines(mapLeft, mapTop, mapWidth, mapWidth, YELLOW);

    auto drawStart = static_cast<int>(m_game->playerZ) + 1;
    int  cartX = 0, cartY = 0;
    for(int i = 0; i < MAP_LENGTH; i++)
    {
        const auto& section = m_game->m_map.m_sections[i];
        auto        dx      = static_cast<int>((section.m_dx /*- m_game->playerX*/) * 0.1f) << 1;
        auto        dy      = static_cast<int>(mapTop + mapHeight - mapHeight * (static_cast<float>(i) / MAP_LENGTH));
        if(dx > -mapWidth / 2 && dx < mapWidth / 2)
        {
            auto xPos = static_cast<int>(mapLeft + (mapWidth / 2) + dx);
            //DrawPixel(xPos - 3, dy, GRAY);
            //DrawPixel(xPos - 4, dy, GRAY);
            DrawPixel(xPos + 0, dy, GRAY);
            DrawPixel(xPos + 1, dy, GRAY);

            if(section.m_hasMiner)
            {
                DrawPixel(xPos + 4, dy, MAROON);
                DrawPixel(xPos + 5, dy, MAROON);
            }

            /*if(section.m_hasSwitch)
            {
                DrawPixel(xPos - 7, dy, GOLD);
                DrawPixel(xPos - 8, dy, GOLD);
            }*/

            if(i == drawStart)
            {
                cartX = xPos;
                cartY = dy;
            }
        }
    }

    //DrawTextureRec(Assets()->cartSprite, (Rectangle){72, 0, 14, 11}, (Vector2){cartX - 7, cartY - 6}, WHITE);
    DrawPixelV((Vector2) {cartX, cartY}, MAROON);
    DrawPixelV((Vector2) {cartX + 1, cartY}, MAROON);
}

void TestScreen::DrawBar(int center, int top, int height, int width, float value)
{
    if(value > 0)
    {
        value = std::min(value, 1.0f);
        DrawRectangle(center, top, value * width, height, MAROON);
    }
    else if(value < 0)
    {
        value   = std::max(value, -1.0f);
        int len = static_cast<int>(value * width);
        DrawRectangle(center + len, top, -len, height, MAROON);
    }
}

void TestScreen::DrawMiners()
{
    for(int i = 0; i < m_game->rescuedMiners; i++)
    {
        int x = i;
        int y = 0;
        if(i > 5)
        {
            x -= 5;
            y++;
        }
        DrawTextureRec(Assets()->minerTexture, (Rectangle) {0, 32, 9, 9}, (Vector2) {257 + x * 10, 183 + y * 10}, WHITE);
    }
}

void TestScreen::DrawTrain()
{
    float slope = m_game->trackSlope;
    float brake = m_game->brakeAmount;
    float speed = m_game->speed;

    float curvature    = m_game->trackCurvature;
    float grossInertia = m_game->grossInertia;
    float netInertia   = m_game->netInertia;
    float weightAmount = m_game->weightAmount;
    float weightForce  = m_game->weightForce;

    if(showStats)
    {
        DrawTextEx(font, (std::string("Slope = ") + std::to_string(slope)).c_str(), (Vector2) {14, 14 * 3}, 8, 1, WHITE);
        DrawTextEx(font, (std::string("Brake = ") + std::to_string(brake)).c_str(), (Vector2) {14, 14 * 4}, 8, 1, WHITE);
        DrawTextEx(font, (std::string("Speed = ") + std::to_string(speed)).c_str(), (Vector2) {14, 14 * 5}, 8, 1, WHITE);

        DrawTextEx(font, (std::string("Curv = ") + std::to_string(curvature)).c_str(), (Vector2) {14, 14 * 6}, 8, 1, WHITE);
        DrawTextEx(font, (std::string("GrossI = ") + std::to_string(grossInertia)).c_str(), (Vector2) {14, 14 * 7}, 8, 1, WHITE);
        DrawTextEx(font, (std::string("Weight = ") + std::to_string(weightAmount)).c_str(), (Vector2) {14, 14 * 9}, 8, 1, WHITE);
        DrawTextEx(font, (std::string("NetI = ") + std::to_string(netInertia)).c_str(), (Vector2) {14, 14 * 8}, 8, 1, WHITE);
        //DrawTextEx(font, (std::string("Slope = ") + std::to_string(weightForce)).c_str(), (Vector2){14, 14 * 10}, 8, 1, WHITE);
    }

    DrawTexturePro(Assets()->cartSprite,
                   (Rectangle) {0, 0, 24, 24},
                   (Rectangle) {trainLeft, trainTop + 6, 24, 24},
                   (Vector2) {12, 12},
                   netInertia * 45,
                   WHITE);
    DrawBar(trainLeft + 35, barsStart, barsHeight, barLength, m_game->grossInertia);
    DrawBar(trainLeft - 35, barsStart, barsHeight, barLength, m_game->weightAmount);

    DrawTexturePro(Assets()->cartSprite,
                   (Rectangle) {24, 0, 48, 24},
                   (Rectangle) {train2Left, trainTop + 6, 48, 24},
                   (Vector2) {24, 12},
                   speed,
                   WHITE);
    DrawBar(train2Left + 35 - barLength, barsStart, barsHeight, 2 * barLength, slope * 5);
    DrawBar(train2Left - 35 + barLength, barsStart, barsHeight, 2 * barLength, -m_game->brakeAmount);
}

void TestScreen::DrawTimer()
{
    auto elapsedMilliseconds = static_cast<int>(raceTime * 1000);
    char timeString[15];
    sprintf(timeString, "%.2d:%.2d:%.3d", elapsedMilliseconds / 60000, (elapsedMilliseconds / 1000) % 60, elapsedMilliseconds % 1000);
    DrawTextEx(font, timeString, (Vector2) {timerLeft, timerTop}, 8, 1, WHITE);
}
#include "MinerScreen.h"

Animator animator("Miner", 2, 2, 5);
bool     _showStats;

TestScreen::TestScreen(int no, TestGame* game) : Screen(no, game) { }

void TestScreen::Load()
{    
    animator.AssignSprite(Assets()->minerTexture);
    soundEnabled = true;
}

void TestScreen::Enter()
{
    message = 1;
}

void TestScreen::Exit() { }

void TestScreen::Unload() { }

void TestScreen::Tick(double deltaTime, double totalTime)
{
    if(m_game->isPaused)
    {
        if(message == MESSAGE_START)
        {
            if(IsKeyPressed(KEY_A) || IsKeyPressed(KEY_ENTER))
            {
                message          = 0;
                m_game->isPaused = false;
            }
            else if(IsKeyPressed(KEY_F1))
            {
                message = MESSAGE_HELP;
                return;
            }
        }
        else if(message == MESSAGE_HELP)
        {
            if(IsKeyPressed(KEY_ENTER))
            {
                // unpause
                message          = 0;
                m_game->isPaused = false;
            }
            else if(IsKeyPressed(KEY_F1))
            {
                message = MESSAGE_START;
                return;
            }
        }
        else if(IsKeyPressed(KEY_ENTER))
        {
            // restart
            m_game->Reset();
            message = MESSAGE_START;
        }
        return;
    }

    if(IsKeyPressed(KEY_F1))
    {
        m_game->isPaused = true;
        message          = MESSAGE_HELP;
        return;
    }

    if(IsKeyPressed(KEY_F2))
    {
        soundEnabled = !soundEnabled;
    }

    if(IsKeyPressed(KEY_S))
        _showStats = !_showStats;

    if(IsKeyPressed(KEY_R))
    {
        // restart
        m_game->Reset();
        message = MESSAGE_START;
        return;
    }

    // cheats - disable
#ifdef DEBUG
    if(IsKeyPressed(KEY_L))
    {
        // jump to finish, cheater!
        //m_game->rescuedMiners = 16;
        m_game->playerZ = MAP_LENGTH - 30;
    }
    if(IsKeyPressed(KEY_M))
    {
        // jump to finish, cheater!
        m_game->ignoreBeams = true;
    }
#endif

    m_game->isDucking = IsKeyDown(KEY_DOWN);

    if(m_game->raceStarted)
        m_game->raceTime += deltaTime;

    // A and Z control *BRAKE* amount
    if(IsKeyDown(KEY_A))
    {
        m_game->brakeAmount -= 0.05f * frameSkip;
        if(!m_game->raceStarted)
        {
            m_game->raceStarted = true;
            m_game->raceTime    = 0;
            if(soundEnabled)
                PlaySoundMulti(Assets()->sndStart);
        }
    }
    else if(m_game->raceStarted)
    {
        // auto release
        m_game->brakeAmount -= 0.05f * frameSkip;
    }

    if(IsKeyDown(KEY_Z))
    {
        if(soundEnabled)
            if(!IsSoundPlaying(Assets()->sndBrake) && m_game->speed > 0)
            {
                PlaySoundMulti(Assets()->sndBrake);
            }
        m_game->brakeAmount += 0.25f * frameSkip;
    }

    if(m_game->brakeAmount > 1)
        m_game->brakeAmount = 1;
    if(m_game->brakeAmount < 0)
        m_game->brakeAmount = 0;

    if(m_game->weightAmount > 1.0f)
        m_game->weightAmount = 1.0f;
    if(m_game->weightAmount < -1.0f)
        m_game->weightAmount = -1.0f;

    float netAccel = m_game->trackSlope; // * (1.0f - m_game->brakeAmount); // if slope negative we go backwards - that's death?
    m_game->speed += netAccel; // speed
    if(m_game->speed > 0)
    {
        m_game->speed -= m_game->brakeAmount * 0.5f * frameSkip;
        if(m_game->speed < 0)
            m_game->speed = 0;
    }
    else
    {
        m_game->speed += m_game->brakeAmount * 0.5f * frameSkip;
        if(m_game->speed > 0)
            m_game->speed = 0;
    }

    // fixed friction
    for(int s = 0; s < frameSkip; s++)
        m_game->speed *= 0.9999f;

    m_game->grossInertia = fabsf(m_game->speed * m_game->speed / 80.0f) * m_game->trackCurvature; // was 100

    m_game->weightAmount = 0;
    if(IsKeyDown(KEY_LEFT))
    {
        m_game->weightAmount -= 1 * frameSkip;
    }
    if(IsKeyDown(KEY_RIGHT))
    {
        m_game->weightAmount = 1 * frameSkip;
    }

    m_game->netInertia = m_game->grossInertia;
    if(m_game->grossInertia > 0 && m_game->weightAmount < 0)
    {
        for(int s = 0; s < frameSkip; s++)
            m_game->netInertia *= 0.5;
    }
    else if(m_game->grossInertia < 0 && m_game->weightAmount > 0)
    {
        for(int s = 0; s < frameSkip; s++)
            m_game->netInertia *= 0.5;
    }

    // move forward - based on NET speed
    auto prevPosition = m_game->playerZ;
    m_game->playerZ += deltaTime * m_game->speed;
    if(m_game->playerZ < 0)
        m_game->playerZ = 0;

    // play track sounds?
    auto intPosition  = static_cast<int>(m_game->playerZ);
    auto movedSection = static_cast<int>(prevPosition) != intPosition;
    if(soundEnabled)
        if(movedSection)
        {
            // we moved into new pos
            if(intPosition % 7 == 3)
            {
                PlaySoundMulti(Assets()->tracks2);
            }
            else if(intPosition % 7 == 1)
            {
                PlaySoundMulti(Assets()->tracks1);
            }
        }

    // update camera location and track curvature
    auto  drawStart     = static_cast<int>(m_game->playerZ) + 1;
    auto& firstSection  = m_game->m_map.m_sections[drawStart - 1];
    auto& secondSection = m_game->m_map.m_sections[drawStart];
    float a             = m_game->playerZ - floorf(m_game->playerZ);
    m_game->playerX     = firstSection.m_dx + (secondSection.m_dx - firstSection.m_dx) * a;
    m_game->playerY     = firstSection.m_dy + (secondSection.m_dy - firstSection.m_dy) * a;

    m_game->trackCurvature = (firstSection.m_slopeX + secondSection.m_slopeX) / 2.0f;
    m_game->trackSlope     = (firstSection.m_slopeY + secondSection.m_slopeY) / 2.0f;

    // missed previous miner?
    if(soundEnabled)
        if(drawStart > 5 && m_game->m_map.m_sections[drawStart - 2].m_hasMiner && movedSection)
        {
            PlaySoundMulti(Assets()->sndMiss);
        }

    // are we close to any miners?
    if(m_game->speed == 0)
    {
        for(int m = 0; m < 5; m++)
        {
            auto& minerSection = m_game->m_map.m_sections[drawStart - 1 + m];
            if(minerSection.m_hasMiner)
            {
                m_game->rescuedMiners++;
                minerSection.m_hasMiner = false;
                if(soundEnabled)
                    PlaySoundMulti(Assets()->sndMiner);
            }
        }
    }

    if(fabsf(m_game->netInertia) > MAX_INERTIA)
    {
        message = MESSAGE_OVERTURN;
        if(!m_game->isPaused)
        {
            if(soundEnabled)
                PlaySoundMulti(Assets()->sndCrash);
        }
        m_game->isPaused = true;
    }
    if(secondSection.m_isBroken && !m_game->isDucking && !m_game->ignoreBeams)
    {
        message = MESSAGE_HITBEAM;
        if(!m_game->isPaused)
        {
            if(soundEnabled)
                PlaySoundMulti(Assets()->sndCrash);
        }
        m_game->isPaused = true;
    }
    if(secondSection.m_isFinish)
    {
        message = MESSAGE_FINISH;
        if(!m_game->isPaused && m_game->rescuedMiners > 0)
        {
            if(soundEnabled)
                PlaySoundMulti(Assets()->sndFinish);
        }
        m_game->isPaused = true;
    }
}

void TestScreen::DrawBackground() { }

void TestScreen::DrawPixels(Color* framebuffer, Rectangle* rect, bool* fullFrame) { }

float PerspectiveScale(float distance)
{
    if(distance <= 0)
        return 0;

    return 1.0f / (0.75f + (distance / 4.0f));
}

void TestScreen::DrawShapes()
{
    animator.Play();

    // player location will be a float
    int centerY = viewCenterY;

    Vector2 fan[SECTION_POINTS + 2];
    if(!m_game->isPaused && rand() % 4 == 0)
    {
        auto randOffset = rand() % 2;
        if(randOffset != 0)
        {
            centerY += randOffset;
            if(soundEnabled)
                PlaySoundMulti(Assets()->sndQuake);
        }
    }

    // we draw 14 sections from player location forward
    auto drawStart = static_cast<int>(m_game->playerZ) + 1;
    auto drawEnd   = drawStart + drawDistance;
    if(drawEnd >= MAP_LENGTH)
    {
        drawEnd = MAP_LENGTH;
    }

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

        if(section.m_isFinish)
        {
            section.GenerateTriangleFan(fan, adjustedCenterX, adjustedCenterY, scale, section.m_ecc, 1);
            DrawTriangleFan(fan, SECTION_POINTS + 2, BLUE);
            section.GenerateTriangleFan(fan, adjustedCenterX, adjustedCenterY, scale, section.m_ecc, 2);
            DrawTriangleFan(fan, SECTION_POINTS + 2, GREEN);
        }
        else
        {
            section.GenerateTriangleFan(fan, adjustedCenterX, adjustedCenterY, scale, section.m_ecc, 0);
            DrawTriangleFan(fan, SECTION_POINTS + 2, Assets()->walls[i - drawStart + 1]);
        }
        Vector2 leftTrack, rightTrack;
        section.TrackPosition(&leftTrack, &rightTrack, adjustedCenterX, adjustedCenterY, scale, section.m_ecc);
        leftTracks.push_back(leftTrack);
        rightTracks.push_back(rightTrack);

        std::vector<Vector2> supportsPosition(6);
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
        int  thickNess = 1 + (drawDistance - i) / 6;
        auto color     = Assets()->palette[56];
        if(i < 5)
        {
            color.r *= 1.3f;
            color.g *= 1.3f;
            color.b *= 1.3f;
        }
        DrawLineEx(leftBars[i], rightBars[i], thickNess, color);
    }

    for(int i = 0; i < numSections; i++)
    {
        auto trackColor = Assets()->palette[1];
        if(i < 5)
        {
            trackColor.r *= 1.3f;
            trackColor.g *= 1.3f;
            trackColor.b *= 1.3f;
        }

        DrawLineEx(leftTracks[i], leftTracks[i + 1], 1 + (numSections - i) / 3, trackColor);
        DrawLineEx(rightTracks[i], rightTracks[i + 1], 1 + (numSections - i) / 3, trackColor);
        if(m_game->m_map.m_sections[drawStart + i].m_hasSupport)
        {
            int  thickNess = 1 + (drawDistance - i);
            auto color     = Assets()->palette[56];
            if(i < 5)
            {
                color.r *= 1.3f;
                color.g *= 1.3f;
                color.b *= 1.3f;
            }
            DrawLineEx(supports[i][0], supports[i][1], thickNess, color);
            DrawLineEx(supports[i][1], supports[i][2], thickNess, color);
            DrawLineEx(supports[i][2], supports[i][3], thickNess, color);
            DrawLineEx(supports[i][3], supports[i][4], thickNess, color);
            DrawLineEx(supports[i][4], supports[i][5], thickNess, color);
        }
    }

    for(int i = 0; i < numSections; i++)
    {
        if(m_game->m_map.m_sections[drawStart + i].m_hasMiner)
        {
            auto size = 64 * PerspectiveScale(i - a); // ((64 - (i * 4)) >> 1) << 1;
            DrawTexturePro(animator.GetSprite(),
                           animator.GetFrameRec(),
                           (Rectangle) {leftTracks[i].x, leftTracks[i].y - size, size, size},
                           (Vector2) {0.0},
                           0,
                           WHITE);
        }
    }

    float factor = floorf(5 * m_game->brakeAmount) / 5.0f;
    float topX   = 4 + 15 + (45 - 15) * factor;
    float topY   = 115 + (105 - 115) * factor;
    DrawLineEx((Vector2) {topX, topY}, (Vector2) {4 + 36, 183}, 5, Assets()->palette[4]);

    DrawTexture(Assets()->cartTexture, 0, 0, WHITE);
    if(m_game->isDucking)
        DrawTexture(Assets()->duckTexture, 0, 0, WHITE);

    DrawMap();
    DrawTrain();
    DrawTimer();
    DrawMiners();

    DrawMessage();
    DrawStats();
}

void TestScreen::DrawOutlined(const char* text, Rectangle wrappingRectangle)
{
    Rectangle textRectangle {wrappingRectangle.x + 5, wrappingRectangle.y + 5, wrappingRectangle.width - 10, wrappingRectangle.height - 5};
    DrawRectangleRoundedLines(wrappingRectangle, 0.1f, 40, 2, Color {96 - 20, 74 - 15, 67 - 12, 255});
    DrawRectangleRounded(wrappingRectangle, 0.1f, 40, Color {96, 74, 67, 255});
    DrawTextRec(Assets()->font, text, textRectangle, 8, FONT_SPACING, true, WHITE);
}

void TestScreen::DrawMessage()
{
    if(message == 0)
        return;

    Rectangle fullRectangle {11 - 6, 9 - 4 + 2, screenWidth - 74 - 12 + 12, screenHeight - 40 - 8 + 8};
    Rectangle halfRectangle {11, 9 + (screenHeight - 40 - 8) / 2, screenWidth - 74 - 12, (screenHeight - 40 - 8) / 2};

    switch(message)
    {
    case MESSAGE_START:

        DrawOutlined("Welcome to\n\n"
                     "Escape from a collapsing mine and rescue your fellow miners on the way!\n\n"
                     "A/Z - handbrake\n"
                     "Down - duck\n"
                     "Left/Right - shift weight\n"
                     "R - restart\n"
                     "F1 - help    F2 - sound    F3 - shader\n\n"
                     "Press A to release the brake and go!\nEnter - continue",
                     fullRectangle);
        DrawTexture(Assets()->logo, 75, 10, WHITE);
        break;
    case MESSAGE_HELP:
        DrawOutlined("Press A to release the handbrake\n"
                     "Press Z to brake and reduce speed\n"
                     "Stop by a miner to take him onboard\n"
                     "Miners are marked as red dots on map\n"
                     "Press Down to duck collapsed beams\n"
                     "Shift weight left/right against inertia\n"
                     "\nF1 - start page\nEnter - go!\nEscape - quit",
                     fullRectangle);
        break;
    case MESSAGE_OVERTURN:
        DrawOutlined("Oh no, you overturned your cart!\n\n"
                     "Next time don't go too fast and shift weight to counter intertia.\n\n"
                     "Enter - try again     Escape - quit",
                     halfRectangle);
        break;
    case MESSAGE_HITBEAM:
        DrawOutlined("Oh no, you hit a collapsed beam!\n\n"
                     "Next time duck when you see any obstacles!\n\n"
                     "Enter - try again     Escape - quit",
                     halfRectangle);
        break;
    case MESSAGE_FINISH:
        if(m_game->rescuedMiners)
        {
            auto elapsedMilliseconds = static_cast<int>(m_game->raceTime * 1000);
            char timeString[15];
            sprintf(
                timeString, "%.2d:%.2d:%.3d", elapsedMilliseconds / 60000, (elapsedMilliseconds / 1000) % 60, elapsedMilliseconds % 1000);
            int         leftBehind = m_game->m_map.numMiners - m_game->rescuedMiners;
            auto        scoreTime  = 1 + (elapsedMilliseconds / 1000) + 20 * leftBehind; // 10 secs penalty per man
            auto        score      = static_cast<int>(1000000.0f / scoreTime); // time of 50 -> score is 200; 04:26 -> 266
            std::string message;
            message.append("Congratulations, you've gotten out and saved some friends!\n\n");
            message.append("Your time:        ");
            message.append(timeString);
            message.append("\n");
            message.append("Men left behind:  ");
            message.append(std::to_string(leftBehind));
            message.append("\n");
            message.append("SCORE:            ");
            message.append(std::to_string(score));
            message.append("\n\nThank you for playing\n\n");
            message.append("Enter - try again     Escape - quit");
            DrawOutlined(message.c_str(), fullRectangle);
            DrawTexture(Assets()->logo, 141, 94, WHITE);
        }
        else
        {
            DrawOutlined("Well, you've gotten out but didn't save anyone else! How selfish!\n\n"
                         "SCORE:            ZERO\n\n"
                         "Back to the mine!\n\n"
                         "Enter - try again     Escape - quit",
                         fullRectangle);
        }
        break;
    default:
        break;
    }
}

void TestScreen::DrawMap()
{
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
            DrawPixel(xPos + 0, dy, GRAY);
            DrawPixel(xPos + 1, dy, GRAY);

            if(section.m_hasMiner)
            {
                DrawPixel(xPos + 4, dy, MAROON);
                DrawPixel(xPos + 5, dy, MAROON);
            }

            if(i == drawStart)
            {
                cartX = xPos;
                cartY = dy;
            }
        }
    }

    DrawTextureRec(
        Assets()->cartSprite, (Rectangle) {96, 0, 6, 6}, (Vector2) {cartX - static_cast<float>(3), cartY - static_cast<float>(3)}, WHITE);
}

void TestScreen::DrawBar(int left, int top, int height, int width, float value)
{
    int center = left + width;
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
        if(i > 7)
        {
            x -= 8;
            y++;
        }
        DrawTextureRec(Assets()->minerTexture,
                       (Rectangle) {0, 32, 9, 9},
                       (Vector2) {static_cast<float>(257) + x * 7, static_cast<float>(182) + y * 7},
                       WHITE);
    }
}

void TestScreen::DrawStats()
{
    if(_showStats)
    {
        DrawTextEx(
            Assets()->font, (std::string("Slope = ") + std::to_string(m_game->trackSlope)).c_str(), (Vector2) {14, 14 * 3}, 8, FONT_SPACING, WHITE);
        DrawTextEx(
            Assets()->font, (std::string("Brake = ") + std::to_string(m_game->brakeAmount)).c_str(), (Vector2) {14, 14 * 4}, 8, FONT_SPACING, WHITE);
        DrawTextEx(Assets()->font, (std::string("Speed = ") + std::to_string(m_game->speed)).c_str(), (Vector2) {14, 14 * 5}, 8, FONT_SPACING, WHITE);

        DrawTextEx(Assets()->font,
                   (std::string("Curv = ") + std::to_string(m_game->trackCurvature)).c_str(),
                   (Vector2) {14, 14 * 6},
                   8,
                   FONT_SPACING,
                   WHITE);
        DrawTextEx(Assets()->font,
                   (std::string("GrossI = ") + std::to_string(m_game->grossInertia)).c_str(),
                   (Vector2) {14, 14 * 7},
                   8,
                   FONT_SPACING,
                   WHITE);
        DrawTextEx(Assets()->font,
                   (std::string("Weight = ") + std::to_string(m_game->weightAmount)).c_str(),
                   (Vector2) {14, 14 * 9},
                   8,
                   FONT_SPACING,
                   WHITE);
        DrawTextEx(
            Assets()->font, (std::string("NetI = ") + std::to_string(m_game->netInertia)).c_str(), (Vector2) {14, 14 * 8}, 8, FONT_SPACING, WHITE);
    }
}

void TestScreen::DrawTrain()
{
    float slope      = m_game->trackSlope;
    float speed      = m_game->speed;
    float netInertia = m_game->netInertia;

    DrawTexturePro(Assets()->cartSprite,
                   (Rectangle) {0, 0, 24, 24},
                   (Rectangle) {trainLeft, trainTop + 6, 24, 24},
                   (Vector2) {12, 12},
                   netInertia * 45,
                   WHITE);
    DrawBar(207, barsStart, barsHeight, barLength, m_game->grossInertia);
    DrawBar(133, barsStart, barsHeight, barLength, m_game->weightAmount);

    DrawTexturePro(Assets()->cartSprite,
                   (Rectangle) {24, 0, 48, 24},
                   (Rectangle) {train2Left, trainTop + 6, 48, 24},
                   (Vector2) {24, 12},
                   speed,
                   WHITE);
    DrawBar(77 - 2 * barLength, barsStart, barsHeight, 2 * barLength, slope * 5);
    DrawBar(5, barsStart, barsHeight, 2 * barLength, -m_game->brakeAmount);
}

void TestScreen::DrawTimer()
{
    auto elapsedMilliseconds = static_cast<int>(m_game->raceTime * 1000);
    char timeString[15];
    sprintf(timeString, "%.2d:%.2d:%.3d", elapsedMilliseconds / 60000, (elapsedMilliseconds / 1000) % 60, elapsedMilliseconds % 1000);
    DrawTextEx(Assets()->font, timeString, (Vector2) {timerLeft, timerTop}, 8, FONT_SPACING, WHITE);
}
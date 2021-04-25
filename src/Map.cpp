#include "Map.h"

Section::Section()
{
    m_dx  = 0;
    m_dy  = 0;
    m_ecc = 0;
}

void Section::SupportsPosition(Vector2 supports[], float cx, float cy, float scale, float excen)
{
    auto  scaledDistance = DIST_TRACK * scale * 1.2f;
    float leftAngle      = M_PI * 0.25f;
    float rightAngle     = -M_PI * 0.25f;
    float rightTopAngle  = M_PI * 0.2f + M_PI;
    float leftTopAngle   = -M_PI * 0.2f + M_PI;

    if(!m_isBroken)
    {
        supports[0].x = cx + sin(leftAngle) * scaledDistance * excen;
        supports[0].y = cy + cos(leftAngle) * scaledDistance;

        supports[1].x = cx + sin(leftTopAngle) * scaledDistance * excen;
        supports[1].y = cy + cos(leftTopAngle) * scaledDistance;

        supports[2].x = cx + sin(leftTopAngle) * scaledDistance * excen + 30 * scale;
        supports[2].y = cy + cos(leftTopAngle) * scaledDistance;

        supports[3].x = cx + sin(rightTopAngle) * scaledDistance * excen - 30 * scale;
        supports[3].y = cy + cos(rightTopAngle) * scaledDistance;

        supports[4].x = cx + sin(rightTopAngle) * scaledDistance * excen;
        supports[4].y = cy + cos(rightTopAngle) * scaledDistance;

        supports[5].x = cx + sin(rightAngle) * scaledDistance * excen;
        supports[5].y = cy + cos(rightAngle) * scaledDistance;
    }
    else
    {
        supports[0].x = cx + sin(leftAngle) * scaledDistance * excen;
        supports[0].y = cy + cos(leftAngle) * scaledDistance;

        supports[1].x = cx + sin(leftTopAngle) * scaledDistance * excen;
        supports[1].y = cy + cos(leftTopAngle) * scaledDistance;

        supports[2].x = cx + sin(leftTopAngle) * scaledDistance * excen;
        supports[2].y = cy + cos(leftTopAngle) * scaledDistance;

        supports[3].x = cx + sin(rightAngle - 0.2f) * scaledDistance * excen;
        supports[3].y = cy + cos(rightAngle - 0.2f) * scaledDistance;

        supports[4].x = cx + sin(rightAngle) * scaledDistance * excen;
        supports[4].y = cy + cos(rightAngle) * scaledDistance;

        supports[5].x = cx + sin(rightTopAngle) * scaledDistance * excen;
        supports[5].y = cy + cos(rightTopAngle) * scaledDistance;
    }
}

void Section::TrackPosition(Vector2* leftTrack, Vector2* rightTrack, float cx, float cy, float scale, float excen)
{
    auto  scaledDistance = DIST_TRACK * scale;
    float leftAngle      = M_PI * 0.08f;
    float rightAngle     = -M_PI * 0.08f;
    leftTrack->x         = cx + sin(leftAngle) * scaledDistance * excen;
    leftTrack->y         = cy + cos(leftAngle) * scaledDistance;
    rightTrack->x        = cx + sin(rightAngle) * scaledDistance * excen;
    rightTrack->y        = cy + cos(rightAngle) * scaledDistance;
}

void Section::BarPosition(Vector2* leftBar, Vector2* rightBar, float cx, float cy, float scale, float excen)
{
    auto  scaledDistance = DIST_TRACK * scale * 1.1f;
    float leftAngle      = M_PI * 0.11f;
    float rightAngle     = -M_PI * 0.11f;
    leftBar->x           = cx + sin(leftAngle) * scaledDistance * excen;
    leftBar->y           = cy + cos(leftAngle) * scaledDistance;
    rightBar->x          = cx + sin(rightAngle) * scaledDistance * excen;
    rightBar->y          = cy + cos(rightAngle) * scaledDistance;
}

void Section::GenerateTriangleFan(Vector2 fanPoints[], float cx, float cy, float scale, float excen, int half)
{
    fanPoints[0].x = cx;
    fanPoints[0].y = cy;
    for(int i = 0; i < SECTION_POINTS; i++)
    {
        auto scaledDistance = m_outline[i].y * scale;
        if(half == 0)
        {
            fanPoints[i + 1].x = cx + sin(m_outline[i].x) * scaledDistance * excen;
            fanPoints[i + 1].y = cy + cos(m_outline[i].x) * scaledDistance;
        }
        else if(half == 1)
        {
            // sky            
            auto skyOffset = SECTION_POINTS / 2 - 3;
            fanPoints[i + 1].x = cx + sin(m_outline[(skyOffset + (i + 1) / 2) % SECTION_POINTS].x) * scaledDistance * excen;
            fanPoints[i + 1].y = cy + cos(m_outline[(skyOffset + (i + 1) / 2) % SECTION_POINTS].x) * scaledDistance;
        }
        else if(half == 2)
        {
            // ground
            auto groundOffset = SECTION_POINTS / 2 + 3;
            fanPoints[i + 1].x = cx + sin(m_outline[(groundOffset + (i + 1) / 2) % SECTION_POINTS].x) * scaledDistance * excen;
            fanPoints[i + 1].y = cy + cos(m_outline[(groundOffset + (i + 1) / 2) % SECTION_POINTS].x) * scaledDistance;
        }
    }
    fanPoints[SECTION_POINTS + 1] = fanPoints[1];
}

void Section::GenerateRandom()
{
    for(int i = 0; i < SECTION_POINTS; i++)
    {
        m_outline[i].x = 2.0f * M_PI * (static_cast<float>(i) / SECTION_POINTS);
        m_outline[i].y = DIST_MIN + static_cast<float>(rand() % (DIST_MAX - DIST_MIN)); // * (1.0f + EXCEN * fabs(sin(m_outline[i].x)));
    }
}

void Section::New()
{
    m_sinceFork = 50;
    m_type      = SectionType::Normal;
    m_dx        = 0;
    m_dy        = 0;    
    m_hasMiner  = false;
    m_hasSwitch = false;
    m_isBroken  = false;
    m_isFinish  = false;
    GenerateRandom();
}

void Section::Continue(Section* previous, bool startFork)
{
    GenerateMorphed(previous);
    if(startFork)
    {
        m_type      = SectionType::ForkInTheRoad;
        m_sinceFork = 0;
    }
    else
    {
        m_type = SectionType::Normal;
    }
    m_dx = 0;
    m_dy = 0;
}

void Section::GenerateMorphed(Section* section)
{
    m_sinceFork = section->m_sinceFork + 1;
    GenerateRandom();
}

Map::Map() : m_sections(MAP_LENGTH) { }

void Map::GenerateSine()
{
    m_sections[0].New();
    for(int i = 1; i < MAP_LENGTH; i++)
    {
        auto isFork = false; //(i == 30);
        m_sections[i].Continue(&m_sections[i - 1], isFork);
        m_sections[i].m_dx  = 50.0f * sin(i / 50.0f);
        m_sections[i].m_dy  = 50.0f * (cos(i / 50.0f) - 1.0f) - i * 0.1f /* constant downhill */;
        m_sections[i].m_ecc = 2.0f + 0.5f * sin(M_PI_2 + i / 50.0f); // 2.0f; // + 0.5f * sin(M_PI_2 + i / 50.0f);

        if(i % 50 == 0)
            m_sections[i].m_hasMiner = true;
        else
            m_sections[i].m_hasMiner = false;

        if(i % 100 == 0)
            m_sections[i].m_isBroken = true;
        else
            m_sections[i].m_isBroken = false;
    }
}

void Map::Reset()
{
    // reset miners
    for(auto y: m_minerLocations)
    {
        m_sections[y].m_hasMiner = true;
    }
}

void Map::Generate()
{
    numMiners = 0;
    auto map    = LoadImage("resources/map1.png");
    auto colors = LoadImageColors(map);
    m_sections[0].New();
    for(int y = 1; y < map.height; y++)
    {
        m_sections[y].Continue(&m_sections[y - 1], false);
    }
    int pr = 0, pg = 0;
    for(int y = 0; y < map.height; y++)
    {
        // red - dx
        // green - dy
        int ri = 0, gi = 0;
        for(int x = 1; x < map.width && (ri == 0 || gi == 0); x++)
        {
            if(ri == 0 && colors[y * map.width + x].g == 0)
            {
                // first red
                ri = x;
            }
            if(gi == 0 && colors[y * map.width + x].r == 0)
            {
                // first green
                gi = x;
            }
        }
        auto& section = m_sections[y];
        if(pr != 0 && pg != 0)
        {
            section.m_dx = (ri + pr) / 2.0f;
            section.m_dy = (gi + pg) / 2.0f;
        }
        else
        {
            section.m_dx = ri;
            section.m_dy = gi;
        }
        section.m_ecc = 2.0f + 0.5f * sin(M_PI_2 + y / 50.0f);

        section.m_hasMiner   = colors[y * map.width].r == 255; // miner -> red
        section.m_hasSupport = colors[y * map.width].g == 255; // support -> green
        section.m_isBroken   = colors[y * map.width].b == 255; // broken support -> cyan

        if(section.m_hasMiner)
        {
            m_minerLocations.push_back(y);
            numMiners++;
        }

        pr = ri;
        pg = gi;

        const int slopeAverage = 10;
        if(y > slopeAverage)
        {
            float totaldx = 0, totaldy = 0;
            for(int q = y - slopeAverage; q <= y; q++)
            {
                totaldx += (m_sections[q].m_dx - m_sections[q - 1].m_dx);
                totaldy += (m_sections[q].m_dy - m_sections[q - 1].m_dy);
            }
            totaldx /= slopeAverage + 1;
            totaldy /= slopeAverage + 1;

            section.m_slopeX = totaldx / 10.0f;
            section.m_slopeY = -(totaldy - 0.95f) / 10.0f;
        }
        else
        {
            section.m_slopeX = 0;
            section.m_slopeY = 0.1f; // starting slope?
        }
        section.m_isFinish = y == map.height - 1;
    }
    UnloadImage(map);
    UnloadImageColors(colors);
}
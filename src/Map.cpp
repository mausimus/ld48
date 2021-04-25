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

    supports[0].x = cx + sin(leftAngle) * scaledDistance * excen;
    supports[0].y = cy + cos(leftAngle) * scaledDistance;

    supports[1].x = cx + sin(leftTopAngle) * scaledDistance * excen;
    supports[1].y = cy + cos(leftTopAngle) * scaledDistance;

    if(m_isBroken)
    {
        supports[3].x = cx + sin(rightTopAngle) * scaledDistance * excen;
        supports[3].y = cy + cos(rightTopAngle) * scaledDistance;

        supports[2].x = cx + sin(rightAngle) * scaledDistance * excen;
        supports[2].y = cy + cos(rightAngle) * scaledDistance;
    }
    else
    {
        supports[2].x = cx + sin(rightTopAngle) * scaledDistance * excen;
        supports[2].y = cy + cos(rightTopAngle) * scaledDistance;

        supports[3].x = cx + sin(rightAngle) * scaledDistance * excen;
        supports[3].y = cy + cos(rightAngle) * scaledDistance;
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

void Section::GenerateTriangleFan(Vector2 fanPoints[], float cx, float cy, float scale, float excen)
{
    fanPoints[0].x = cx;
    fanPoints[0].y = cy;
    for(int i = 0; i < SECTION_POINTS; i++)
    {
        auto scaledDistance = m_outline[i].y * scale;
        fanPoints[i + 1].x  = cx + sin(m_outline[i].x) * scaledDistance * excen;
        fanPoints[i + 1].y  = cy + cos(m_outline[i].x) * scaledDistance;
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

void Map::Generate()
{
    m_sections[0].New();
    for(int i = 1; i < MAP_LENGTH; i++)
    {
        auto isFork = false; //(i == 30);
        m_sections[i].Continue(&m_sections[i - 1], isFork);
        m_sections[i].m_dx  = 50.0f * sin(i / 50.0f);
        m_sections[i].m_dy  = 50.0f * (cos(i / 50.0f) - 1.0f) - i * 0.1f /* constant downhill */;
        m_sections[i].m_ecc = 2.0f; // + 0.5f * sin(M_PI_2 + i / 50.0f);

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
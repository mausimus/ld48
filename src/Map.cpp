#include "Map.h"

Section::Section()
{
    m_dx  = 0;
    m_dy  = 0;
    m_ecc = 0;
}

void Section::TrackPosition(Vector2* leftTrack, Vector2* rightTrack, float cx, float cy, float scale, float excen)
{
    auto  scaledDistance = DIST_TRACK * scale;
    float leftAngle      = M_PI * 0.1f;
    float rightAngle     = -M_PI * 0.1f;
    leftTrack->x         = cx + sin(leftAngle) * scaledDistance * excen;
    leftTrack->y         = cy + cos(leftAngle) * scaledDistance;
    rightTrack->x        = cx + sin(rightAngle) * scaledDistance * excen;
    rightTrack->y        = cy + cos(rightAngle) * scaledDistance;
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
        m_sections[i].m_dy  = 50.0f * cos(M_PI_2 + i / 50.0f);
        m_sections[i].m_ecc = 2.0f; // + 0.5f * sin(M_PI_2 + i / 50.0f);

        if(i % 50 == 0)
            m_sections[i].m_hasMiner = true;
        else
            m_sections[i].m_hasMiner = false;

        if(i % 100 == 0)
            m_sections[i].m_hasSwitch = true;
        else
            m_sections[i].m_hasSwitch = false;
    }
}
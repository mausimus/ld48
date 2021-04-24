#pragma once

#include "stdafx.h"


constexpr int SECTION_POINTS = 10;
constexpr int MAP_LENGTH = 1000;
constexpr int DIST_MIN = 100;
constexpr int DIST_MAX = 120;
constexpr int DIST_TRACK = 90;
constexpr float EXCEN = 2.0f;

enum SectionType
{
    Normal        = 1,
    ForkInTheRoad = 2
};

class Section
{
protected:
    void GenerateRandom();
    void GenerateMorphed(Section* section);

public:
    Section();
    SectionType m_type;
    Vector2     m_outline[SECTION_POINTS]; // (angle, distance) representation of the section
    int         m_sinceFork;
    float       m_dx;
    float       m_dy;
    float       m_ecc;

    void New();
    void Continue(Section* previous, bool startFork);

    void GenerateTriangleFan(Vector2 fanPoints[], float cx, float cy, float scale, float excen);
    void TrackPosition(Vector2* leftTrack, Vector2* rightTrack, float cx, float cy, float scale, float excen);
};

class Map
{
public:
    Map();
    std::vector<Section> m_sections; // TODO: forks in the road

    void Generate();
};
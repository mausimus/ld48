#pragma once

#include "stdafx.h"

class Assets
{
public:
    Texture2D minerTexture;
    Texture2D wallsTexture;
    Texture2D cartTexture;
    Texture2D duckTexture;
    Texture2D cartSprite;
    Texture2D logo;

    Color palette[64];
    Color walls[14];

    Sound tracks1;
    Sound tracks2;
    Sound sndStart;
    Sound sndMiss;
    Sound sndMiner;
    Sound sndCrash;
    Sound sndQuake;
    Sound sndBrake;
    Sound sndFinish;

    Music musIntro;

    Font font;

    Assets();
    void Load();
    void Unload();
};

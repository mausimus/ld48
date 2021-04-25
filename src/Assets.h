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
    Color palette[64];
    Color walls[14];
    Sound tracks1;
    Sound tracks2;

    Assets();
    void Load();
    void Unload();
};

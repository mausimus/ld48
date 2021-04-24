#pragma once

#include "stdafx.h"

class Assets
{
public:
    Texture2D minerTexture;
    Texture2D wallsTexture;
    Texture2D cartTexture;
    Texture2D cartSprite;
    Color palette[64];
    Color walls[14];

    Assets();
    void Load();
    void Unload();
};

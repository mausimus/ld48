#pragma once

#include "stdafx.h"

class Assets
{
public:
    Texture2D spriteTexture;
    Texture2D wallsTexture;
    Color palette[64];
    Color walls[14];

    Assets();
    void Load();
    void Unload();
};

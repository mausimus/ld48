#include "Assets.h"
#include "../extras/atari64.h"

Assets::Assets()
{
    for(int i = 0; i < 64; i++)
    {
        palette[i].r = _atari64[i] >> 16;
        palette[i].g = (_atari64[i] >> 8) & 255;
        palette[i].b = _atari64[i] & 255;
        palette[i].a = 255;
    }

    Color wallColor{231,156,66,255};
    for(int i = 0; i < 14; i++)
    {
        walls[i].r = static_cast<unsigned char>(wallColor.r * (1 - i / 14.0f));
        walls[i].g = static_cast<unsigned char>(wallColor.g * (1 - i / 14.0f));
        walls[i].b = static_cast<unsigned char>(wallColor.b * (1 - i / 14.0f));
        walls[i].a = 255;
    }
}

void Assets::Load()
{
    spriteTexture = LoadTexture("resources/sprite.png");
    wallsTexture = LoadTexture("resources/walls.png");
}

void Assets::Unload()
{
    UnloadTexture(spriteTexture);
}

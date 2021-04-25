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
    minerTexture = LoadTexture("resources/miner.png");
    wallsTexture = LoadTexture("resources/walls.png");
    cartTexture = LoadTexture("resources/cart.png");
    duckTexture = LoadTexture("resources/duck.png");
    cartSprite = LoadTexture("resources/cart-sprite.png");
    tracks1 = LoadSound("resources/audio/tracks1.wav");
    tracks2 = LoadSound("resources/audio/tracks2.wav");    
}

void Assets::Unload()
{
    UnloadTexture(minerTexture);
}

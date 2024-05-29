#pragma once

// ----------------------------------------------------------------------------
//                             Asset Constants
// ----------------------------------------------------------------------------
enum SpriteID
{
    SPRITE_PLAYER,
    //SPRITE_ENEMY,
    SPRITE_COUNT,
};

struct Sprite
{
    IVec2 atlasOffset;
    IVec2 spriteSize;
};

// ----------------------------------------------------------------------------
//                             Asset Functions
// ----------------------------------------------------------------------------
Sprite get_sprite(SpriteID spriteID)
{
    Sprite sprite = {};

    switch (spriteID)
    {
        case SPRITE_PLAYER:
        {
            sprite.atlasOffset = {0, 0};
            sprite.spriteSize = {16, 16};
        } break;
    }

    return sprite;
}
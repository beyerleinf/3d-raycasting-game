#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>

#include "engine/player.hpp"
#include "engine/world.hpp"
#include "main.hpp"

// #define MAP_WIDTH 32
// #define MAP_HEIGHT 32

namespace ray3d
{

class Core
{
  public:
    Core(SDL_Renderer *renderer, Uint16 screenWidth, Uint16 screenHeight);
    ~Core();

    bool init();

  private:
    World *world;
    Player *player;
    SDL_Surface *textures[2];
    SDL_Renderer *renderer;
    SDL_Texture *framebuffer;
    Uint32 tempbuffer[SCREEN_HEIGHT][SCREEN_WIDTH];

    void handleMouseMovement(SDL_MouseMotionEvent mouseMotion);
    void handleKeyboardEvent(const Uint8 *keyStates);
    void render();
    void updatePlayerPosition(int x, int y);

    Uint16 mScreenWidth;
    Uint16 mScreenHeight;

    double mMovementSpeed;
    double mRotationSpeed;

    double mMovementSpeedModifier = 1.0;

    double mPreviousTime = 0;
    double mCurrentTime = 0;
    double mFrameTime = 0;
};

} // namespace ray3d
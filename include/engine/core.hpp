#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>

#include "engine/player.hpp"
#include "engine/world.hpp"

// #define MAP_WIDTH 32
// #define MAP_HEIGHT 32

namespace ray3d
{

class Core
{
  public:
    Core(SDL_Renderer *renderer, int screenWidth, int screenHeight);
    ~Core();

    bool init();

  private:
    World *world;
    Player *player;
    SDL_Surface *textures[2];
    SDL_Renderer *renderer;
    SDL_Texture *framebuffer;
    Uint32 *tempbuffer;

    void handleMouseMovement(SDL_MouseMotionEvent mouseMotion);
    void handleKeyboardEvent(const Uint8 *keyStates);
    void render();
    void updatePlayerPosition(int x, int y);

    int screenWidth;
    int screenHeight;

    double movementSpeed;
    double rotationSpeed;

    double previousTime = 0;
    double currentTime = 0;
    double frameTime = 0;
};

} // namespace ray3d
#include "main.hpp"
#include "engine/core.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cmath>
#include <limits>
#include <stdio.h>

// const int SCREEN_FPS = 60;
// const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;

// int worldMap[MAP_WIDTH][MAP_HEIGHT] =
//     {
//         {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
//         {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
//         {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
//         {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
//         {1, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 0, 0, 3, 0, 3, 0, 3, 0, 0, 0, 1},
//         {1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
//         {1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 1},
//         {1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
//         {1, 0, 0, 0, 0, 0, 2, 2, 0, 2, 2, 0, 0, 0, 0, 3, 0, 3, 0, 3, 0, 0, 0, 1},
//         {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
//         {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
//         {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
//         {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
//         {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
//         {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
//         {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
//         {1, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
//         {1, 4, 0, 4, 0, 0, 0, 0, 4, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
//         {1, 4, 0, 0, 0, 0, 5, 0, 4, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
//         {1, 4, 0, 4, 0, 0, 0, 0, 4, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
//         {1, 4, 0, 4, 4, 4, 4, 4, 4, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
//         {1, 4, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
//         {1, 4, 4, 4, 4, 4, 4, 4, 4, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
//         {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;

int main(int argv, char **args)
{
    printf("Welcome.\n");

    // double posX = 22, posY = 12;      // x and y start position
    // double dirX = -1, dirY = 0;       // initial direction vector
    // double planeX = 0, planeY = 0.66; // the 2d raycaster version of camera plane

    // double time = 0;    // time of current frame
    // double oldTime = 0; // time of previous frame

    // double moveSpeed = 0;
    // double rotSpeed = 0;

    if (!init())
    {
        printf("Failed to initialize.\n");
        return 1;
    }
    else
    {
        if (window == NULL)
        {
            return 1;
        }
        else
        {
            SDL_CaptureMouse(SDL_TRUE);
            ray3d::Core core(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
            core.init();

            // bool quit = false;
            // SDL_Event event;

            // while (!quit)
            // {
            //     while (SDL_PollEvent(&event))
            //     {
            //         if (event.type == SDL_QUIT)
            //         {
            //             quit = true;
            //         }

            //         if (event.type == SDL_KEYDOWN)
            //         {
            //             if (event.key.keysym.sym == SDLK_ESCAPE)
            //             {
            //                 quit = true;
            //             }
            //         }

            //         if (event.type == SDL_MOUSEMOTION)
            //         {
            //             const double rotation = ((double(event.motion.xrel)) / 100) * -1;

            //             // both camera direction and camera plane must be rotated
            //             double oldDirX = dirX;
            //             dirX = dirX * cos(rotation) - dirY * sin(rotation);
            //             dirY = oldDirX * sin(rotation) + dirY * cos(rotation);
            //             double oldPlaneX = planeX;
            //             planeX = planeX * cos(rotation) - planeY * sin(rotation);
            //             planeY = oldPlaneX * sin(rotation) + planeY * cos(rotation);
            //         }
            //     }

            //     const Uint8 *currentKeyStates = SDL_GetKeyboardState(NULL);

            //     if (currentKeyStates[SDL_SCANCODE_W])
            //     {
            //         if (worldMap[int(posX + dirX * moveSpeed)][int(posY)] == false)
            //         {
            //             posX += dirX * moveSpeed;
            //         }

            //         if (worldMap[int(posX)][int(posY + dirY * moveSpeed)] == false)
            //         {
            //             posY += dirY * moveSpeed;
            //         }
            //     }
            //     else if (currentKeyStates[SDL_SCANCODE_S])
            //     {
            //         if (worldMap[int(posX - dirX * moveSpeed)][int(posY)] == false)
            //         {
            //             posX -= dirX * moveSpeed;
            //         }

            //         if (worldMap[int(posX)][int(posY - dirY * moveSpeed)] == false)
            //         {
            //             posY -= dirY * moveSpeed;
            //         }
            //     }

            //     if (currentKeyStates[SDL_SCANCODE_A])
            //     {
            //         if (worldMap[int(posX - dirY * moveSpeed)][int(posY)] == false)
            //         {
            //             posX -= dirY * moveSpeed;
            //         }

            //         if (worldMap[int(posX)][int(posY - (dirX * -1) * moveSpeed)] == false)
            //         {
            //             posY -= (dirX * -1) * moveSpeed;
            //         }
            //     }
            //     else if (currentKeyStates[SDL_SCANCODE_D])
            //     {
            //         if (worldMap[int(posX + dirY * moveSpeed)][int(posY)] == false)
            //         {
            //             posX += dirY * moveSpeed;
            //         }

            //         if (worldMap[int(posX)][int(posY + (dirX * -1) * moveSpeed)] == false)
            //         {
            //             posY += (dirX * -1) * moveSpeed;
            //         }
            //     }

            //     SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
            //     SDL_RenderClear(renderer);

            //     for (int x = 0; x < SCREEN_WIDTH; x++)
            //     {
            //         // calculate ray position and direction
            //         double cameraX = 2 * x / double(SCREEN_WIDTH) - 1; //x-coordinate in camera space
            //         double rayDirX = dirX + planeX * cameraX;
            //         double rayDirY = dirY + planeY * cameraX;

            //         // which box of the map we're in
            //         int mapX = int(posX);
            //         int mapY = int(posY);

            //         // length of ray from current position to next x or y-side
            //         double sideDistX;
            //         double sideDistY;

            //         // length of ray from one x or y-side to next x or y-side
            //         double deltaDistX = std::abs(1 / rayDirX);
            //         double deltaDistY = std::abs(1 / rayDirY);
            //         double perpWallDist;

            //         // what direction to step in x or y-direction (either +1 or -1)
            //         int stepX;
            //         int stepY;

            //         int hit = 0; // was there a wall hit?
            //         int side;    // was a NS or a EW wall hit?

            //         // calculate step and initial sideDist
            //         if (rayDirX < 0)
            //         {
            //             stepX = -1;
            //             sideDistX = (posX - mapX) * deltaDistX;
            //         }
            //         else
            //         {
            //             stepX = 1;
            //             sideDistX = (mapX + 1.0 - posX) * deltaDistX;
            //         }

            //         if (rayDirY < 0)
            //         {
            //             stepY = -1;
            //             sideDistY = (posY - mapY) * deltaDistY;
            //         }
            //         else
            //         {
            //             stepY = 1;
            //             sideDistY = (mapY + 1.0 - posY) * deltaDistY;
            //         }

            //         // perform DDA
            //         while (hit == 0)
            //         {
            //             // jump to next map square, OR in x-direction, OR in y-direction
            //             if (sideDistX < sideDistY)
            //             {
            //                 sideDistX += deltaDistX;
            //                 mapX += stepX;
            //                 side = 0;
            //             }
            //             else
            //             {
            //                 sideDistY += deltaDistY;
            //                 mapY += stepY;
            //                 side = 1;
            //             }

            //             // check if ray has hit a wall
            //             if (worldMap[mapX][mapY] > 0)
            //             {
            //                 hit = 1;
            //             }
            //         }

            //         // calculate distance projected on camera direction (Euclidean distance will give fisheye
            //         effect!) if (side == 0)
            //         {
            //             perpWallDist = (mapX - posX + (1 - stepX) / 2) / rayDirX;
            //         }
            //         else
            //         {
            //             perpWallDist = (mapY - posY + (1 - stepY) / 2) / rayDirY;
            //         }

            //         // calculate height of line to draw on screen
            //         int lineHeight = (int)(SCREEN_HEIGHT / perpWallDist);

            //         // calculate lowest and highest pixel to fill in current stripe
            //         int drawStart = -lineHeight / 2 + SCREEN_HEIGHT / 2;
            //         if (drawStart < 0)
            //         {
            //             drawStart = 0;
            //         }

            //         int drawEnd = lineHeight / 2 + SCREEN_HEIGHT / 2;
            //         if (drawEnd >= SCREEN_HEIGHT)
            //         {
            //             drawEnd = SCREEN_HEIGHT - 1;
            //         }

            //         SDL_Color color;

            //         switch (worldMap[mapX][mapY])
            //         {
            //         case 1:
            //             color = {255, 0, 0}; // red
            //             break;
            //         case 2:
            //             color = {0, 255, 0}; // green
            //             break;
            //         case 3:
            //             color = {0, 0, 255}; // blue
            //             break;
            //         case 4:
            //             color = {255, 255, 255}; // white
            //             break;
            //         default:;
            //             color = {0, 255, 255}; // yellow
            //             break;
            //         }

            //         // give x and y sides different brightness
            //         if (side == 1)
            //         {
            //             color.r = color.r / 2;
            //             color.g = color.g / 2;
            //             color.b = color.b / 2;
            //         }

            //         // draw the pixels of the stripe as a vertical line
            //         SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            //         SDL_RenderDrawLineF(renderer, x, drawStart, x, drawEnd);
            //     }

            //     oldTime = time;
            //     time = SDL_GetTicks();

            //     double frameTime = (time - oldTime) / 1000; // frameTime is the time this frame has taken, in
            //     seconds;
            //     // printf("FPS %f\n", frameTime);

            //     moveSpeed = frameTime * 5;
            //     rotSpeed = frameTime * 3;

            //     SDL_RenderPresent(renderer);
            //     SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTime);
            // }

            close();
            return 0;
        }
    }
}

bool init()
{
    bool success = true;

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        printf("Failed to initialize SDL. SDL Error: %s\n", SDL_GetError());
        success = false;
    }
    else
    {
        window = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT,
                                  SDL_WINDOW_SHOWN);

        if (window == NULL)
        {
            printf("Failed to create window. SDL Error: %s\n", SDL_GetError());
            success = false;
        }
        else
        {
            int imgFlags = IMG_INIT_PNG;

            if (!(IMG_Init(imgFlags) & imgFlags))
            {
                printf("Failed to initialize SDL_image. SDL_image Error: %s\n", IMG_GetError());
                success = false;
            }
            else
            {
                renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

                if (renderer == NULL)
                {
                    printf("Failed to create renderer. SDL Error: %s\n", SDL_GetError());
                    success = false;
                }
            }
        }
    }

    return success;
}

void close()
{
    SDL_DestroyRenderer(renderer);
    renderer = NULL;

    SDL_DestroyWindow(window);
    window = NULL;

    printf("Goodbye.\n");

    SDL_Quit();
}
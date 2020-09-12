#include "engine/core.hpp"
#include <stdio.h>

namespace ray3d
{

Core::Core(SDL_Renderer *r, int screenW, int screenH)
{
    player = new Player(22, 12, -1, 0);
    world = new World();
    world->load();

    screenWidth = screenW;
    screenHeight = screenH;
    renderer = r;
}

Core::~Core()
{
    SDL_DestroyTexture(framebuffer);
}

bool Core::init()
{
    bool result = true;

    framebuffer =
        SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, screenWidth, screenHeight);

    textures[0] = IMG_Load("res/tex/test.png");
    textures[1] = IMG_Load("res/tex/tex2.png");

    if (textures[0] == NULL || textures[1] == NULL)
    {
        printf("Failed to load textures. SDL Error: %s\n", IMG_GetError());
    }

    // Initialize the game loop
    bool quit = false;
    SDL_Event event;

    while (!quit)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                quit = true;
            }

            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    quit = true;
                }
            }

            if (event.type == SDL_MOUSEMOTION)
            {
                handleMouseMovement(event.motion);
            }
        }

        handleKeyboardEvent(SDL_GetKeyboardState(NULL));
        render();
    }

    return result;
}

void Core::render()
{
    tempbuffer = new Uint32[screenWidth * screenHeight];

    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer);

    for (int x = 0; x < screenWidth; x++)
    {
        // calculate ray position and direction
        double cameraX = 2 * x / static_cast<double>(screenWidth) - 1; // x-coordinate in camera space
        double rayDirX = player->direction.x + player->cameraPlane.x * cameraX;
        double rayDirY = player->direction.y + player->cameraPlane.y * cameraX;

        // which box of the map we're in
        int mapX = static_cast<int>(player->position.x);
        int mapY = static_cast<int>(player->position.y);

        // length of ray from current position to next x or y-side
        double sideDistX;
        double sideDistY;

        // length of ray from one x or y-side to next x or y-side
        double deltaDistX = std::abs(1 / rayDirX);
        double deltaDistY = std::abs(1 / rayDirY);
        double perpWallDist;

        // what direction to step in x or y-direction (either +1 or -1)
        int stepX;
        int stepY;

        int hit = 0; // was there a wall hit?
        int side;    // was a NS or a EW wall hit?

        // calculate step and initial sideDist
        if (rayDirX < 0)
        {
            stepX = -1;
            sideDistX = (player->position.x - mapX) * deltaDistX;
        }
        else
        {
            stepX = 1;
            sideDistX = (mapX + 1.0 - player->position.x) * deltaDistX;
        }

        if (rayDirY < 0)
        {
            stepY = -1;
            sideDistY = (player->position.y - mapY) * deltaDistY;
        }
        else
        {
            stepY = 1;
            sideDistY = (mapY + 1.0 - player->position.y) * deltaDistY;
        }

        // perform DDA
        while (hit == 0)
        {
            // jump to next map square, OR in x-direction, OR in y-direction
            if (sideDistX < sideDistY)
            {
                sideDistX += deltaDistX;
                mapX += stepX;
                side = 0;
            }
            else
            {
                sideDistY += deltaDistY;
                mapY += stepY;
                side = 1;
            }

            WorldTile *tile = world->getTile(mapX, mapY);

            if (tile != NULL)
            {
                if (tile->type != WorldTileType::Floor)
                {
                    hit = 1;
                }
            }
            else
            {
                printf("Tile was a nullptr... x%i y%i. Assuming I didn't hit a wall...\n", mapX, mapY);
            }
        }

        // calculate distance projected on camera direction
        // (Euclidean distance will give fisheye effect!)
        if (side == 0)
        {
            perpWallDist = (mapX - player->position.x + (1 - stepX) / 2) / rayDirX;
        }
        else
        {
            perpWallDist = (mapY - player->position.y + (1 - stepY) / 2) / rayDirY;
        }

        // calculate height of line to draw on screen
        int lineHeight = static_cast<int>(screenHeight / perpWallDist);

        // calculate lowest and highest pixel to fill in current stripe
        int drawStart = -lineHeight / 2 + screenHeight / 2;
        if (drawStart < 0)
        {
            drawStart = 0;
        }

        int drawEnd = lineHeight / 2 + screenHeight / 2;
        if (drawEnd >= screenHeight)
        {
            drawEnd = screenHeight - 1;
        }

        WorldTile *tile = world->getTile(mapX, mapY);
        int textureNumber = 0;

        if (tile != NULL)
        {
            textureNumber = tile->type - 1;
        }
        else
        {
            printf("Tile was a nullptr... x%i y%i. Cannot determine textureNumber. Will use %i\n", mapX, mapY,
                   textureNumber);
        }

        double wallX;
        if (side == 0)
        {
            wallX = player->position.y + perpWallDist * rayDirY;
        }
        else
        {
            wallX = player->position.x + perpWallDist * rayDirX;
        }

        wallX -= floor(wallX);

        SDL_Surface *surface = textures[textureNumber];

        int texWidth = surface->w;
        int texHeight = surface->h;
        int texX = static_cast<int>(wallX * static_cast<double>(texWidth));

        if (side == 0 && rayDirX > 0)
        {
            texX = texWidth - texX - 1;
        }

        if (side == 1 && rayDirY < 0)
        {
            texX = texWidth - texX - 1;
        }

        // TODO: an integer-only bresenham or DDA like algorithm could make the
        // texture coordinate stepping faster

        // How much to increase the texture coordinate per screen pixel
        double step = static_cast<double>(texHeight) / static_cast<double>(lineHeight);
        double texPos = (drawStart - screenHeight / 2 + lineHeight / 2) * step;

        for (int y = drawStart; y < drawEnd; y++)
        {
            int texY = static_cast<int>(texPos) & (texHeight - 1);
            texPos += step;

            // TODO: put into function
            int bpp = surface->format->BytesPerPixel;
            Uint8 *p = (Uint8 *)surface->pixels + texY * surface->pitch + texX * bpp;
            Uint32 pixel = *(Uint32 *)p;

            // switch (bpp)
            // {
            // case 1:
            //     pixel = *p;
            //     break;
            // case 2:
            //     pixel = *(Uint16 *)p;
            //     break;
            // case 3:
            //     if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
            //     {
            //         pixel = p[0] << 16 | p[1] << 8 | p[2];
            //     }
            //     else
            //     {
            //         pixel = p[0] | p[1] << 8 | p[2] << 16;
            //     }
            //     break;
            // case 4:
            //     pixel = *(Uint32 *)p;
            //     break;
            // default:
            //     break;
            // }

            // make color darker for y-sides: R, G and B byte each divided through
            // two with a "shift" and an "and"
            if (side == 1)
            {
                pixel = (pixel >> 1) & 8355711;
            }

            tempbuffer[y * screenWidth + x] = pixel;

            // TODO: this shit is hella inefficient
            // SDL_Color color;
            // SDL_GetRGBA(pixel, surface->format, &color.r, &color.g, &color.b, &color.a);
            // SDL_SetRenderDrawColor(mRenderer, color.r, color.g, color.b, color.a);
            // SDL_RenderDrawPoint(mRenderer, x, y);
        }

        // switch (world->getTile(mapX, mapY)->type)
        // {
        // case 1:
        //     color = {255, 0, 0}; // red
        //     break;
        // case 2:
        //     color = {0, 255, 0}; // green
        //     break;
        // case 3:
        //     color = {0, 0, 255}; // blue
        //     break;
        // case 4:
        //     color = {255, 255, 255}; // white
        //     break;
        // default:;
        //     color = {0, 255, 255}; // yellow
        //     break;
        // }

        // give x and y sides different brightness
        // if (side == 1)
        // {
        //     color.r = color.r / 2;
        //     color.g = color.g / 2;
        //     color.b = color.b / 2;
        // }

        // // draw the pixels of the stripe as a vertical line
        // SDL_SetRenderDrawColor(mRenderer, color.r, color.g, color.b, color.a);
        // SDL_RenderDrawLineF(mRenderer, x, drawStart, x, drawEnd);
    }

    previousTime = currentTime;
    currentTime = SDL_GetTicks();
    frameTime = (currentTime - previousTime) / 1000.0;

    if (static_cast<int>(frameTime) % 1000 == 0)
        printf("FPS: %i\n", static_cast<int>(1.0 / frameTime));

    movementSpeed = frameTime * movementSpeedModifier;
    rotationSpeed = frameTime * 3.0;

    SDL_UpdateTexture(framebuffer, NULL, tempbuffer, screenWidth * sizeof(Uint32));
    SDL_RenderCopy(renderer, framebuffer, NULL, NULL);
    SDL_RenderPresent(renderer);

    // clean up
    delete[] tempbuffer;
}

void Core::handleKeyboardEvent(const Uint8 *keyStates)
{
    if (keyStates[SDL_SCANCODE_LSHIFT])
    {
        movementSpeedModifier = 5.0;
    }
    else
    {
        movementSpeedModifier = 2.0;
    }

    if (keyStates[SDL_SCANCODE_W] || keyStates[SDL_SCANCODE_UP])
    {
        const double newXPosition = player->position.x + player->direction.x * movementSpeed;
        if (world->isValidPosition(static_cast<int>(newXPosition), static_cast<int>(player->position.y)))
        {
            player->position.x = newXPosition;
        }

        const double newYPosition = player->position.y + player->direction.y * movementSpeed;
        if (world->isValidPosition(static_cast<int>(player->position.x), static_cast<int>(newYPosition)))
        {
            player->position.y = newYPosition;
        }
    }
    else if (keyStates[SDL_SCANCODE_S] || keyStates[SDL_SCANCODE_DOWN])
    {
        const double newXPosition = player->position.x - player->direction.x * movementSpeed;
        if (world->isValidPosition(static_cast<int>(newXPosition), static_cast<int>(player->position.y)))
        {
            player->position.x = newXPosition;
        }

        const double newYPosition = player->position.y - player->direction.y * movementSpeed;
        if (world->isValidPosition(static_cast<int>(player->position.x), static_cast<int>(newYPosition)))
        {
            player->position.y = newYPosition;
        }
    }

    if (keyStates[SDL_SCANCODE_A] || keyStates[SDL_SCANCODE_LEFT])
    {
        const double newXPosition = player->position.x - player->direction.y * movementSpeed;
        if (world->isValidPosition(static_cast<int>(newXPosition), static_cast<int>(player->position.y)))
        {
            player->position.x = newXPosition;
        }

        const double newYPosition = player->position.y - (player->direction.x * -1) * movementSpeed;
        if (world->isValidPosition(static_cast<int>(player->position.x), static_cast<int>(newYPosition)))
        {
            player->position.y = newYPosition;
        }
    }
    else if (keyStates[SDL_SCANCODE_D] || keyStates[SDL_SCANCODE_RIGHT])
    {
        const double newXPosition = player->position.x + player->direction.y * movementSpeed;
        if (world->isValidPosition(static_cast<int>(newXPosition), static_cast<int>(player->position.y)))
        {
            player->position.x = newXPosition;
        }

        const double newYPosition = player->position.y + (player->direction.x * -1) * movementSpeed;
        if (world->isValidPosition(static_cast<int>(player->position.x), static_cast<int>(newYPosition)))
        {
            player->position.y = newYPosition;
        }
    }
}

void Core::handleMouseMovement(SDL_MouseMotionEvent mouseMotion)
{
    const double rotation = (double(mouseMotion.xrel) / 100) * -1;

    // both camera direction and camera plane must be rotated
    double oldDirX = player->direction.x;
    double newDirX = player->direction.x * cos(rotation) - player->direction.y * sin(rotation);
    double newDirY = oldDirX * sin(rotation) + player->direction.y * cos(rotation);
    player->direction.set(newDirX, newDirY);

    double oldPlaneX = player->cameraPlane.x;
    double newPlaneX = player->cameraPlane.x * cos(rotation) - player->cameraPlane.y * sin(rotation);
    double newPlaneY = oldPlaneX * sin(rotation) + player->cameraPlane.y * cos(rotation);
    player->cameraPlane.set(newPlaneX, newPlaneY);
}

} // namespace ray3d

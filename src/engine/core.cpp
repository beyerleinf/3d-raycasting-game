#include "engine/core.hpp"
#include <stdio.h>

namespace ray3d
{

Core::Core(SDL_Renderer *r, Uint16 screenW, Uint16 screenH): renderer(r), mScreenWidth(screenW), mScreenHeight(screenH)
{
    player = new Player(22, 12, -1, 0);
    world = new World();
    world->load();
}

Core::~Core()
{
    SDL_DestroyTexture(framebuffer);
}

bool Core::init()
{
    bool result = true;

    framebuffer =
        SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, mScreenWidth, mScreenHeight);

    textures[0] = IMG_Load("res/tex/test.png");
    textures[1] = IMG_Load("res/tex/floor.png");

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
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer);

    // render floor and ceiling
    for (int y = 0; y < mScreenHeight; y++)
    {
        // rayDir for leftmost ray (x = 0) and rightmost ray (x = w)
        float rayDirX0 = player->direction.x - player->cameraPlane.x;
        float rayDirY0 = player->direction.y - player->cameraPlane.y;
        float rayDirX1 = player->direction.x + player->cameraPlane.x;
        float rayDirY1 = player->direction.y + player->cameraPlane.y;

        // Current y position compared to the center of the screen (the horizon)
        int p = y - mScreenHeight / 2;

        // Vertical position of the camera.
        float posZ = 0.5 * mScreenHeight;

        // Horizontal distance from the camera to the floor for the current row.
        // 0.5 is the z position exactly in the middle between floor and ceiling.
        float rowDistance = posZ / p;

        // calculate the real world step vector we have to add for each x (parallel to camera plane)
        // adding step by step avoids multiplications with a weight in the inner loop
        float floorStepX = rowDistance * (rayDirX1 - rayDirX0) / mScreenWidth;
        float floorStepY = rowDistance * (rayDirY1 - rayDirY0) / mScreenWidth;

        // real world coordinates of the leftmost column. This will be updated as we step to the right.
        float floorX = player->position.x + rowDistance * rayDirX0;
        float floorY = player->position.y + rowDistance * rayDirY0;

        for (int x = 0; x < mScreenWidth; ++x)
        {
            // the cell coord is simply got from the integer parts of floorX and floorY
            int cellX = static_cast<int>(floorX);
            int cellY = static_cast<int>(floorY);

            // choose texture and draw the pixel
            int floorTexture = 1;
            SDL_Surface *floorSurface = textures[floorTexture];
            int floorTexWidth = floorSurface->w;
            int floorTexHeight = floorSurface->h;

            // get the texture coordinate from the fractional part
            int ftx = static_cast<int>(floorTexWidth * (floorX - cellX)) & (floorTexWidth - 1);
            int fty = static_cast<int>(floorTexHeight * (floorY - cellY)) & (floorTexHeight - 1);

            int ceilingTexture = 1;
            SDL_Surface *ceilingSurface = textures[ceilingTexture];
            int ceilingTexWidth = ceilingSurface->w;
            int ceilingTexHeight = ceilingSurface->h;

            // get the texture coordinate from the fractional part
            int ctx = static_cast<int>(ceilingTexWidth * (floorX - cellX)) & (ceilingTexWidth - 1);
            int cty = static_cast<int>(ceilingTexHeight * (floorY - cellY)) & (ceilingTexHeight - 1);

            floorX += floorStepX;
            floorY += floorStepY;

            Uint32 color;

            Uint8 *p;
            Uint32 pixel;

            int fbpp = floorSurface->format->BytesPerPixel;
            int cbpp = ceilingSurface->format->BytesPerPixel;

            // floor
            p = static_cast<Uint8 *>(static_cast<Uint8 *>(floorSurface->pixels) + fty * floorSurface->pitch +
                                     ftx * fbpp);
            pixel = *(Uint32 *)p;
            pixel = (pixel >> 1) & 8355711;
            tempbuffer[y][x] = pixel;

            // ceiling (symmetrical, at screenHeight - y - 1 instead of y)
            p = static_cast<Uint8 *>(static_cast<Uint8 *>(ceilingSurface->pixels) + cty * ceilingSurface->pitch +
                                     ctx * cbpp);
            pixel = *(Uint32 *)p;
            pixel = (pixel >> 1) & 8355711;
            tempbuffer[mScreenHeight - y - 1][x] = pixel;
        }
    }

    // render walls
    for (int x = 0; x < mScreenWidth; x++)
    {
        // calculate ray position and direction
        double cameraX = 2 * x / static_cast<double>(mScreenWidth) - 1; // x-coordinate in camera space
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
        int lineHeight = static_cast<int>(mScreenHeight / perpWallDist);

        // calculate lowest and highest pixel to fill in current stripe
        int drawStart = -lineHeight / 2 + mScreenHeight / 2;
        if (drawStart < 0)
        {
            drawStart = 0;
        }

        int drawEnd = lineHeight / 2 + mScreenHeight / 2;
        if (drawEnd >= mScreenHeight)
        {
            drawEnd = mScreenHeight - 1;
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
        double texPos = (drawStart - mScreenHeight / 2 + lineHeight / 2) * step;

        for (int y = drawStart; y < drawEnd; y++)
        {
            int texY = static_cast<int>(texPos) & (texHeight - 1);
            texPos += step;

            // TODO: put into function
            int bpp = surface->format->BytesPerPixel;
            Uint8 *p = static_cast<Uint8 *>(static_cast<Uint8 *>(surface->pixels) + texY * surface->pitch + texX * bpp);
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

            tempbuffer[y][x] = pixel;
        }
    }

    mPreviousTime = mCurrentTime;
    mCurrentTime = SDL_GetTicks();
    mFrameTime = (mCurrentTime - mPreviousTime) / 1000.0;

    // if (static_cast<int>(frameTime) % 1000 == 0)
    //     printf("FPS: %i\n", static_cast<int>(1.0 / frameTime));

    mMovementSpeed = mFrameTime * mMovementSpeedModifier;
    mRotationSpeed = mFrameTime * 3.0;

    SDL_UpdateTexture(framebuffer, NULL, tempbuffer, mScreenWidth * sizeof(Uint32));
    SDL_RenderCopy(renderer, framebuffer, NULL, NULL);
    SDL_RenderPresent(renderer);

    // clean up
    memset(tempbuffer, 0, mScreenHeight * mScreenWidth);
}

void Core::handleKeyboardEvent(const Uint8 *keyStates)
{
    if (keyStates[SDL_SCANCODE_LSHIFT])
    {
        mMovementSpeedModifier = 5.0;
    }
    else
    {
        mMovementSpeedModifier = 2.0;
    }

    if (keyStates[SDL_SCANCODE_W] || keyStates[SDL_SCANCODE_UP])
    {
        const double newXPosition = player->position.x + player->direction.x * mMovementSpeed;
        if (world->isValidPosition(static_cast<int>(newXPosition), static_cast<int>(player->position.y)))
        {
            player->position.x = newXPosition;
        }

        const double newYPosition = player->position.y + player->direction.y * mMovementSpeed;
        if (world->isValidPosition(static_cast<int>(player->position.x), static_cast<int>(newYPosition)))
        {
            player->position.y = newYPosition;
        }
    }
    else if (keyStates[SDL_SCANCODE_S] || keyStates[SDL_SCANCODE_DOWN])
    {
        const double newXPosition = player->position.x - player->direction.x * mMovementSpeed;
        if (world->isValidPosition(static_cast<int>(newXPosition), static_cast<int>(player->position.y)))
        {
            player->position.x = newXPosition;
        }

        const double newYPosition = player->position.y - player->direction.y * mMovementSpeed;
        if (world->isValidPosition(static_cast<int>(player->position.x), static_cast<int>(newYPosition)))
        {
            player->position.y = newYPosition;
        }
    }

    if (keyStates[SDL_SCANCODE_A] || keyStates[SDL_SCANCODE_LEFT])
    {
        const double newXPosition = player->position.x - player->direction.y * mMovementSpeed;
        if (world->isValidPosition(static_cast<int>(newXPosition), static_cast<int>(player->position.y)))
        {
            player->position.x = newXPosition;
        }

        const double newYPosition = player->position.y - (player->direction.x * -1) * mMovementSpeed;
        if (world->isValidPosition(static_cast<int>(player->position.x), static_cast<int>(newYPosition)))
        {
            player->position.y = newYPosition;
        }
    }
    else if (keyStates[SDL_SCANCODE_D] || keyStates[SDL_SCANCODE_RIGHT])
    {
        const double newXPosition = player->position.x + player->direction.y * mMovementSpeed;
        if (world->isValidPosition(static_cast<int>(newXPosition), static_cast<int>(player->position.y)))
        {
            player->position.x = newXPosition;
        }

        const double newYPosition = player->position.y + (player->direction.x * -1) * mMovementSpeed;
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

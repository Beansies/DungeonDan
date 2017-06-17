#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_events.h>
#include <SDL_timer.h>
#include <math.h>
#include "DungeonGenerator.h"

#define screenWidth 600
#define screenHeight 480

#define mapWidth 64
#define mapHeight 64

#define texWidth 32
#define texHeight 32

struct texture readBMP(char* address);
void drawScreenBuffer();
void setup();

/*int worldMap[mapWidth][mapHeight]=
{
  {4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5},
  {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,5},
  {4,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7},
  {4,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7},
  {4,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,5},
  {4,0,5,0,0,0,0,5,5,5,5,5,5,5,5,5,5,5,0,5,5,5,5,5},
  {4,0,5,0,0,0,0,5,0,5,0,5,0,5,0,5,5,0,0,0,5,5,5,1},
  {4,0,5,0,0,0,0,5,0,0,0,0,0,0,0,5,5,0,0,0,0,0,0,8},
  {4,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,5,5,1},
  {4,0,5,0,0,0,0,5,0,0,0,0,0,0,0,5,5,0,0,0,0,0,0,8},
  {4,0,0,0,0,0,0,5,0,0,0,0,0,0,0,5,5,0,0,0,5,5,5,1},
  {4,0,0,0,0,0,0,5,5,5,5,0,5,5,5,5,5,5,5,5,5,5,5,1},
  {6,6,6,6,6,6,6,6,6,6,6,0,6,6,6,6,6,6,6,6,6,6,6,6},
  {8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
  {6,6,6,6,6,6,0,6,6,6,6,0,6,6,6,6,6,6,6,6,6,6,6,6},
  {4,4,4,4,4,4,0,4,4,4,6,0,6,4,4,4,4,4,4,4,3,3,3,3},
  {4,0,0,0,0,0,0,0,0,4,6,0,6,4,0,0,0,0,0,4,0,0,0,4},
  {4,0,0,0,0,0,0,0,0,0,0,0,6,4,0,0,5,0,0,4,0,0,0,4},
  {4,0,0,0,0,0,0,0,0,4,6,0,6,4,0,0,0,0,0,4,4,0,4,4},
  {4,0,6,0,6,0,0,0,0,4,6,0,0,0,0,0,5,0,0,0,0,0,0,4},
  {4,0,0,5,0,0,0,0,0,4,6,0,6,4,0,0,0,0,0,4,4,0,4,4},
  {4,0,6,0,6,0,0,0,0,4,6,0,6,4,0,0,5,0,0,4,0,0,0,4},
  {4,0,0,0,0,0,0,0,0,4,6,0,6,4,0,0,0,0,0,4,0,0,0,4},
  {4,4,4,4,4,4,4,4,4,4,1,1,1,4,4,4,4,4,4,3,3,3,3,3}
};*/
levelMap worldMap;

typedef struct coord
{
    double x;
    double y;
} coord;

typedef struct transform
{
    struct coord position;
    struct coord rotation;
} transform;

typedef struct timeFrame
{
    double time;
    double oldTime;
    double deltaTime;
} timeFrame;

typedef struct texture
{
    int pix[texWidth][texHeight];
} texture;

typedef struct screenBuffer
{
    Uint32 pixels[screenHeight][screenWidth];
} screenBuffer;

void drawScreenBuffer(screenBuffer* buffer, SDL_Surface* screen)
{
    Uint32* bufp;
    bufp = (Uint32*)screen->pixels;

    int y, x;
    for(y = 0; y < screenHeight; y++)
    {
        for(x = 0; x < screenWidth; x++)
        {
            *bufp = buffer->pixels[y][x];
            bufp++;
            buffer->pixels[y][x] = 0;
        }
        bufp += screen->pitch / 4;
        bufp -= screenWidth;
    }
}

texture readBMP(char* address)
{
    int tempColor;
    int dataOffset;
    texture currentTex;
    FILE* t;
    t = fopen(address, "rb");
    fseek(t, 10, SEEK_SET);
    fread(&dataOffset, 4, 1, t);
    printf("%d", dataOffset);
    fseek(t, dataOffset, SEEK_SET);
    int x, y;
    for(y = texHeight - 1; y >= 0; y--)
    {
        for(x = 0; x < texWidth; x++)
        {
            fread(&tempColor, 3, 1, t);
            currentTex.pix[y][x] = tempColor;
        }
    }
    fclose(t);
    return currentTex;
}

void movePlayer(struct transform* plyr, coord* inpt, double moveSpeed)
{
    if(inpt->y == 1)
    {
        if(worldMap.mapData[(int)(plyr->position.x + plyr->rotation.x * moveSpeed)][(int)(plyr->position.y)] == 0) plyr->position.x += plyr->rotation.x * moveSpeed;
        if(worldMap.mapData[(int)(plyr->position.x)][(int)(plyr->position.y + plyr->rotation.y * moveSpeed)] == 0) plyr->position.y += plyr->rotation.y * moveSpeed;
    }
    else if(inpt->y == -1)
    {
        if(worldMap.mapData[(int)(plyr->position.x - plyr->rotation.x * moveSpeed)][(int)(plyr->position.y)] == 0) plyr->position.x -= plyr->rotation.x * moveSpeed;
        if(worldMap.mapData[(int)(plyr->position.x)][(int)(plyr->position.y - plyr->rotation.y * moveSpeed)] == 0) plyr->position.y -= plyr->rotation.y * moveSpeed;
    }
    if(inpt->x == 1)
    {
        if(worldMap.mapData[(int)(plyr->position.x + plyr->rotation.y * moveSpeed)][(int)(plyr->position.y)] == 0) plyr->position.x += plyr->rotation.y * moveSpeed;
        if(worldMap.mapData[(int)(plyr->position.x)][(int)(plyr->position.y - plyr->rotation.x * moveSpeed)] == 0) plyr->position.y -= plyr->rotation.x * moveSpeed;
    }
    else if(inpt->x == -1)
    {
        if(worldMap.mapData[(int)(plyr->position.x - plyr->rotation.y * moveSpeed)][(int)(plyr->position.y)] == 0) plyr->position.x -= plyr->rotation.y * moveSpeed;
        if(worldMap.mapData[(int)(plyr->position.x)][(int)(plyr->position.y + plyr->rotation.x * moveSpeed)] == 0) plyr->position.y += plyr->rotation.x * moveSpeed;
    }
}

int main(int argc, char* argv[])
{
    double planeX = 0, planeY = 0.66; //the 2d raycaster version of camera plane

    transform player;
    player.position.x = 3.5;
    player.position.y = 3.5;
    player.rotation.x = -1;
    player.rotation.y = 0;

    struct coord input;
    input.x = 0;
    input.y = 0;

    struct timeFrame time;
    time.deltaTime = 0;
    time.oldTime = 0;
    time.time = 0;

    int oldMouseX = 0;
    SDL_GetMouseState(&oldMouseX, NULL);
    int currentMouseX = 0;
    int deltaMouseX = 0;

    screenBuffer pixelBuffer;
    texture textures[8];

    worldMap = generateDungeon();


    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *myWindow;
    SDL_Renderer *myRenderer;
    SDL_Surface *scr;
    SDL_Texture *myTexture;
    SDL_Event myEvent;
    SDL_CreateWindowAndRenderer(screenWidth, screenHeight, 0, &myWindow, &myRenderer);
    //SDL_SetWindowFullscreen(myWindow, SDL_WINDOW_FULLSCREEN);
    scr = SDL_CreateRGBSurface(0, screenWidth, screenHeight, 32,
                               0x00FF0000,
                               0x0000FF00,
                               0x000000FF,
                               0xFF000000);
    myTexture = SDL_CreateTexture(myRenderer,
                               SDL_PIXELFORMAT_ARGB8888,
                               SDL_TEXTUREACCESS_STREAMING,
                               screenWidth, screenHeight);
    SDL_SetRenderDrawColor(myRenderer, 0, 0, 0, 255);
    SDL_CaptureMouse(SDL_ENABLE);
    SDL_ShowCursor(SDL_DISABLE);
    SDL_SetRelativeMouseMode(SDL_TRUE);

    //generate some textures

    int itx, ity;
    for(itx = 0; itx < texWidth; itx++)
    for(ity = 0; ity < texHeight; ity++)
    {
        int xorcolor = (itx * 256 / texWidth) ^ (ity * 256 / texHeight);
        int ycolor = ity * 256 / texHeight;
        int xycolor = ity * 128 / texHeight + itx * 128 / texWidth;
        textures[blankTile].pix[ity][itx] = 65536 * 254 * (itx != ity && itx != texWidth - ity); //flat red texture with black cross
        textures[roomPillarTile].pix[ity][itx] = xycolor + 256 * xycolor + 65536 * xycolor; //sloped greyscale
        textures[doorTile].pix[ity][itx] = 256 * xycolor + 65536 * xycolor; //sloped yellow gradient
        //textures[3].pix[ity][itx] = xorcolor + 256 * xorcolor + 65536 * xorcolor; //xor greyscale
        //textures[4].pix[ity][itx] = 256 * xorcolor; //xor green
        //textures[5].pix[ity][itx] = 65536 * 192 * (itx % 16 && ity % 16); //red bricks
        //textures[6].pix[ity][itx] = 65536 * ycolor; //red gradient
        //textures[7].pix[ity][itx] = 128 + 256 * 128 + 65536 * 128; //flat grey texture
    }
    textures[roomPillarTile] = readBMP("Textures/SteelWall.bmp");
    textures[steelWallTile] = readBMP("Textures/BrickWall.bmp");


    while(1)
    {
        int x;
        for(x = 0; x < screenWidth; x++)
        {
            //calculate ray position and direction
            double cameraX = 2 * x / (double)(screenWidth) - 1; //x-coordinate in camera space
            double rayPosX = player.position.x;
            double rayPosY = player.position.y;
            double rayDirX = player.rotation.x + planeX * cameraX;
            double rayDirY = player.rotation.y + planeY * cameraX;
            //which box of the map we're in
            int mapX = (int)(rayPosX);
            int mapY = (int)(rayPosY);
            //length of ray from current position to next x or y-side
            double sideDistX;
            double sideDistY;

            //length of ray from one x or y-side to next x or y-side
            double deltaDistX = sqrt(1 + (rayDirY * rayDirY) / (rayDirX * rayDirX));
            double deltaDistY = sqrt(1 + (rayDirX * rayDirX) / (rayDirY * rayDirY));
            double perpWallDist;

            //what direction to step in x or y-direction (either +1 or -1)
            int stepX;
            int stepY;

            int hit = 0; //was there a wall hit?
            int side; //was a NS or a EW wall hit?
            //calculate step and initial sideDist
            if (rayDirX < 0)
            {
                stepX = -1;
                sideDistX = (rayPosX - mapX) * deltaDistX;
            }
            else
            {
                stepX = 1;
                sideDistX = (mapX + 1.0 - rayPosX) * deltaDistX;
            }
            if (rayDirY < 0)
            {
                stepY = -1;
                sideDistY = (rayPosY - mapY) * deltaDistY;
            }
            else
            {
                stepY = 1;
                sideDistY = (mapY + 1.0 - rayPosY) * deltaDistY;
            }
            //perform DDA
            while (hit == 0)
            {
                //jump to next map square, OR in x-direction, OR in y-direction
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
                //Check if ray has hit a wall
                if (worldMap.mapData[mapX][mapY] > 0) hit = 1;
            }
            //Calculate distance projected on camera direction (oblique distance will give fisheye effect!)
            if (side == 0) perpWallDist = (mapX - rayPosX + (1 - stepX) / 2) / rayDirX;
            else           perpWallDist = (mapY - rayPosY + (1 - stepY) / 2) / rayDirY;

            //Calculate height of line to draw on screen
            int lineHeight = (int)(screenHeight / perpWallDist);

            //calculate lowest and highest pixel to fill in current stripe
            int drawStart = -lineHeight / 2 + screenHeight / 2;
            if(drawStart < 0)drawStart = 0;
            int drawEnd = lineHeight / 2 + screenHeight / 2;
            if(drawEnd >= screenHeight)drawEnd = screenHeight - 1;

            //texturing calculations
            int texNum = worldMap.mapData[mapX][mapY]; //1 subtracted from it so that texture 0 can be used!

            //calculate value of wallX
            double wallX; //where exactly the wall was hit
            if (side == 0)
            {
                wallX = rayPosY + perpWallDist * rayDirY;
            }
            else
            {
                wallX = rayPosX + perpWallDist * rayDirX;
            }
            wallX -= floor((wallX));

            //x coordinate on the texture
            int texX = (int)(wallX * (double)(texWidth));
            if(side == 0 && rayDirX > 0) texX = texWidth - texX - 1;
            if(side == 1 && rayDirY < 0) texX = texWidth - texX - 1;
            Uint32 color1;
            int ity2;
            for(ity2 = drawStart; ity2 < drawEnd; ity2++)
            {
                int d = ity2 * 256 - screenHeight * 128 + lineHeight * 128;  //256 and 128 factors to avoid floats
                int texY = ((d * texHeight) / lineHeight) / 256;
                color1 = textures[texNum].pix[texY][texX];
                //make color darker for y-sides: R, G and B byte each divided through two with a "shift" and an "and"
                if(side == 1)
                {
                    if(side == 1) color1 = (color1 >> 1) & 8355711;
                }
                pixelBuffer.pixels[ity2][x] = color1;
            }
            for(ity2 = drawEnd; ity2 < screenHeight; ity2++)
            {
                pixelBuffer.pixels[ity2][x] =6579300;
            }
            for(ity2 = 0; ity2 < drawStart; ity2++)
            {
                pixelBuffer.pixels[ity2][x] = 7895160;
            }
        }

        drawScreenBuffer(&pixelBuffer, scr);


        time.oldTime = time.time;
        time.time = SDL_GetPerformanceCounter();
        time.deltaTime = ((time.time - time.oldTime)/(double)SDL_GetPerformanceFrequency());

        oldMouseX = currentMouseX;
        SDL_GetMouseState(&currentMouseX, NULL);
        deltaMouseX = currentMouseX - oldMouseX;

        double moveSpeed = time.deltaTime * 5;
        double turnSpeed = time.deltaTime * deltaMouseX / 2;

        while(SDL_PollEvent(&myEvent) == 1)
        {
            if(myEvent.type == SDL_QUIT)
            {
                printf("goodbye..\n");
                SDL_Quit();
                return 0;
            }
            if(myEvent.type == SDL_KEYDOWN && myEvent.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
            {
                printf("goodbye..\n");
                SDL_Quit();
                return 0;
            }
            if(myEvent.type == SDL_KEYDOWN)
            {
                if(myEvent.key.keysym.scancode == SDL_SCANCODE_A && myEvent.key.repeat == 0)
                {
                    printf("you pressed a didn't you?\n");
                    input.x = -1;
                }
                else if(myEvent.key.keysym.scancode == SDL_SCANCODE_D && myEvent.key.repeat == 0)
                {
                    printf("you pressed d didn't you?\n");
                    input.x = 1;
                }
                else if(myEvent.key.keysym.scancode == SDL_SCANCODE_S && myEvent.key.repeat == 0)
                {
                    printf("you pressed s didn't you?\n");
                    input.y = -1;
                }
                else if(myEvent.key.keysym.scancode == SDL_SCANCODE_W && myEvent.key.repeat == 0)
                {
                    printf("you pressed w didn't you?\n");
                    input.y = 1;
                }
            }
            if(myEvent.type == SDL_KEYUP)
            {
                if(myEvent.key.keysym.scancode == SDL_SCANCODE_A && myEvent.key.repeat == 0)
                {
                    input.x = 0;
                }
                else if(myEvent.key.keysym.scancode == SDL_SCANCODE_D && myEvent.key.repeat == 0)
                {
                    input.x = 0;
                }
                else if(myEvent.key.keysym.scancode == SDL_SCANCODE_S && myEvent.key.repeat == 0)
                {
                    input.y = 0;
                }
                else if(myEvent.key.keysym.scancode == SDL_SCANCODE_W && myEvent.key.repeat == 0)
                {
                    input.y = 0;
                }
            }
        }

        movePlayer(&player, &input, moveSpeed);

        //both camera direction and camera plane must be rotated
        double oldDirX = player.rotation.x;
        player.rotation.x = player.rotation.x * cos(-turnSpeed) - player.rotation.y * sin(-turnSpeed);
        player.rotation.y = oldDirX * sin(-turnSpeed) + player.rotation.y * cos(-turnSpeed);
        double oldPlaneX = planeX;
        planeX = planeX * cos(-turnSpeed) - planeY * sin(-turnSpeed);
        planeY = oldPlaneX * sin(-turnSpeed) + planeY * cos(-turnSpeed);

        //scr = SDL_LoadBMP("Textures/M1911.bmp");

        SDL_SetRenderDrawColor(myRenderer, 0, 0, 0, 255);
        SDL_UpdateTexture(myTexture, NULL, scr->pixels, scr->pitch);
        SDL_RenderClear(myRenderer);
        SDL_RenderCopy(myRenderer, myTexture, NULL, NULL);
        SDL_RenderPresent(myRenderer);
    }

    SDL_Quit();
    return 0;
}

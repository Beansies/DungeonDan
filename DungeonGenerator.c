#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "DungeonGenerator.h"

void printMap(levelMap* map)
{
    int x, y;
    for(y = 0; y < levelHeight; y++)
    {
        for(x = 0; x < levelWidth; x++)
        {
           printf("%d ", map->mapData[x][y]);
        }
        printf("\n");
    }
}

void initMap(levelMap* map)
{
    int x, y;
    for(x = 0; x < levelWidth; x++)
    {
        for(y = 0; y < levelHeight; y++)
        {
           map->mapData[x][y] = blankTile;
        }
    }
}

void addRoom(levelMap* map, intCoord* location, int height, int width)
{
    int x, y;
    for(x = 0; x < width; x++)
    {
        for(y = 0; y < height; y++)
        {
            map->mapData[location->x + x][location->y + y] = floorTile;
            if(x == 0) map->mapData[location->x - 1][location->y + y] = tempWall;
            if(x == width-1) map->mapData[location->x + width][location->y + y] = tempWall;
            if(y == 0) map->mapData[location->x + x][location->y - 1] = tempWall;
            if(y == height-1) map->mapData[location->x + x][location->y + height] = tempWall;
        }
    }
    map->mapData[location->x - 1][location->y - 1] = roomPillarTile;
    map->mapData[location->x - 1][location->y + height] = roomPillarTile;
    map->mapData[location->x + width][location->y - 1] = roomPillarTile;
    map->mapData[location->x + width][location->y + height] = roomPillarTile;
}

int checkSpace(levelMap* map, intCoord* location, int height, int width)
{
    int x, y;
    for(x = -1; x <= width; x++)
    {
        for(y = -1; y <= height; y++)
        {
            if(map->mapData[location->x + x][location->y + y] != blankTile && map->mapData[location->x + x][location->y + y] != tempWall && map->mapData[location->x + x][location->y + y] != roomPillarTile)
                return 1; //failure
            if((location->x + width >= levelWidth) || (location->y + height >= levelHeight) || (location->x <= 0) || (location->y <= 0))
                return 1;
        }
    }
    return 0;
}

void fireTempWalls(levelMap* map, int tileType)
{
    int x, y;
    for(x = 0; x < levelWidth; x++)
    {
        for(y = 0; y < levelHeight; y++)
        {
            if(map->mapData[x][y] == tempWall)
            {
                map->mapData[x][y] = tileType;
            }
        }
    }
}

intCoord selectTempWall(levelMap* map)
{
    intCoord locations[64];
    int count = 0;
    int x, y;
    for(x = 0; x < levelWidth; x++)
    {
        for(y = 0; y < levelHeight; y++)
        {
            if(map->mapData[x][y] == tempWall)
            {
                locations[count].x = x;
                locations[count].y = y;
                count++;
            }
        }
    }
    intCoord selected;
    selected = locations[rand()%count];
    return selected;
}

int attemptToAddFeature(levelMap* map)
{
    intCoord doorPos, firstRoomTile;
    doorPos = selectTempWall(map);
    int direction;
    if((map->mapData[doorPos.x][doorPos.y + 1] == tempWall || map->mapData[doorPos.x][doorPos.y + 1] == roomPillarTile) &&
       (map->mapData[doorPos.x][doorPos.y - 1] == tempWall || map->mapData[doorPos.x][doorPos.y - 1] == roomPillarTile))
    {
        if(map->mapData[doorPos.x + 1][doorPos.y] == floorTile)
        {
            firstRoomTile.x = doorPos.x - 1;
            firstRoomTile.y = doorPos.y;
            direction = left;
        }
        else
        {
            firstRoomTile.x = doorPos.x + 1;
            firstRoomTile.y = doorPos.y;
            direction = right;
        }
    }
    else if((map->mapData[doorPos.x + 1][doorPos.y] == tempWall || map->mapData[doorPos.x + 1][doorPos.y] == roomPillarTile) &&
            (map->mapData[doorPos.x - 1][doorPos.y] == tempWall || map->mapData[doorPos.x - 1][doorPos.y] == roomPillarTile))
    {
        if(map->mapData[doorPos.x][doorPos.y + 1] == floorTile)
        {
            firstRoomTile.x = doorPos.x;
            firstRoomTile.y = doorPos.y - 1;
            direction = up;
        }
        else
        {
            firstRoomTile.x = doorPos.x;
            firstRoomTile.y = doorPos.y + 1;
            direction = down;
        }
    }
    else
    {
        return 1; //if tiles around the door tile aren't good, abort
    }
    //generate room type, 0 for room, 1 for corridor
    int roomType;
    roomType = rand()%2;
    if(roomType == 0)
    {
        int width = 3 + (rand()%10);
        int height = 3 + (rand()%10);
        int offset;

        switch(direction)
        {
        case up:
            offset = rand()%width;
            firstRoomTile.x -= offset;
            firstRoomTile.y -= (height-1);
            break;
        case down:
            offset = rand()%width;
            firstRoomTile.x -= offset;
            break;
        case right:
            offset = rand()%height;
            firstRoomTile.y -= offset;
            break;
        case left:
            offset = rand()%height;
            firstRoomTile.y -= offset;
            firstRoomTile.x -= (width-1);
            break;
        }
        if(checkSpace(map, &firstRoomTile, height, width) == 0)
        {
            fireTempWalls(map, steelWallTile);
            addRoom(map, &firstRoomTile, height, width);
            map->mapData[doorPos.x][doorPos.y] = floorTile;
            return 0;
        }
        else
        {
            return 1;
        }
    }
    else
    {
        int length = 3 + (rand()%10);
        switch(direction)
        {
        case up:
            firstRoomTile.y -= (length-1);
            if(checkSpace(map, &firstRoomTile, length, 1) == 0)
            {
                fireTempWalls(map, steelWallTile);
                addRoom(map, &firstRoomTile, length, 1);
                map->mapData[doorPos.x][doorPos.y] = floorTile;
                return 0;
            }
            return 1;
            break;
        case down:
            if(checkSpace(map, &firstRoomTile, length, 1) == 0)
            {
                fireTempWalls(map, steelWallTile);
                addRoom(map, &firstRoomTile, length, 1);
                map->mapData[doorPos.x][doorPos.y] = floorTile;
                return 0;
            }
            return 1;
            break;
        case right:
            if(checkSpace(map, &firstRoomTile, 1, length) == 0)
            {
                fireTempWalls(map, steelWallTile);
                addRoom(map, &firstRoomTile, 1, length);
                map->mapData[doorPos.x][doorPos.y] = floorTile;
                return 0;
            }
            return 1;
            break;
        case left:
            firstRoomTile.x -= (length-1);
            if(checkSpace(map, &firstRoomTile, 1, length) == 0)
            {
                fireTempWalls(map, steelWallTile);
                addRoom(map, &firstRoomTile, 1, length);
                map->mapData[doorPos.x][doorPos.y] = floorTile;
                return 0;
            }
            return 1;
            break;
        }
    }
    return 1;
}

levelMap generateDungeon()
{
    printf("Dungeon Generation started. \n");
    srand(time(NULL));
    levelMap dungeonMap;
    initMap(&dungeonMap);
    struct intCoord loc;
    loc.x = 1;
    loc.y = 1;
    addRoom(&dungeonMap, &loc, 5, 6);
    int featureCount = 0;
    int attemptCount = 0;
    while(featureCount < 50)
    {
        if(attemptToAddFeature(&dungeonMap) == 0)
        {
            featureCount++;
        }
        attemptCount++;
        if(attemptCount > 300)
        {
            initMap(&dungeonMap);
            loc.x = 1;
            loc.y = 1;
            addRoom(&dungeonMap, &loc, 5, 6);
            attemptCount = 0;
            featureCount = 0;
        }
    }
    fireTempWalls(&dungeonMap, steelWallTile);
    printMap(&dungeonMap);


    return dungeonMap;
}

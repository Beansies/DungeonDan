#define levelWidth 64
#define levelHeight 64

enum
{
    floorTile,
    blankTile,
    brickWallTile,
    steelWallTile,
    roomPillarTile,
    tempWall,
    doorTile
};

enum
{
    up,
    right,
    down,
    left
};

typedef struct intCoord
{
    int x;
    int y;
} intCoord;

typedef struct levelMap
{
    int mapData[levelWidth][levelHeight];
} levelMap;

void printMap(levelMap* map);
void initMap(levelMap* map);
void addRoom(levelMap* map, intCoord* location, int height, int width);
int checkSpace(levelMap* map, intCoord* location, int height, int width);
void fireTempWalls(levelMap* map, int tileType);
intCoord selectTempWall(levelMap* map);
int attemptToAddFeature(levelMap* map);
levelMap generateDungeon();

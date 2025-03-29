#ifndef HT_DATA_STRUCTURES_H
#define HT_DATA_STRUCTURES_H

#ifndef MAX_PLATFORMS
#define MAX_PLATFORMS 150
#endif

#ifndef MAX_COINS
#define MAX_COINS 10
#endif

typedef struct {
    float x, y;
    float vx, vy;
    float width, height;
    bool onGround;
} Player;

typedef struct {
    float x, y;
    float width, height;
    bool semisolid;
    bool lava;
    bool decorative;
} Platform;

typedef struct {
    const char* layout;
    int width, height;
    Platform platforms[MAX_PLATFORMS];
    int platformCount;
    float levelWidth;
} Level;

typedef struct {
    float x, y;
    float width, height;
    bool active;
} Item;

typedef struct {
    float x, y;
    float width, height;
    bool active;
} Coin;

typedef enum {
    STATE_MENU,
    STATE_GAME,
    STATE_CREDITS
} GameState;

typedef enum {
    MENU_PLAY,
    MENU_CREDITS,
    MENU_EXIT,
    MENU_COUNT
} MenuOption;

#endif
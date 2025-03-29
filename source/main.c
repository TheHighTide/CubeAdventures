#include <3ds.h>
#include <citro2d.h>

#include "dats.h"	    // Include the library I wrote to add new data structures and data sets
#include "savesys.h"	// Include the library I wrote to manage game save data

#define SCREEN_WIDTH 400  // Top screen width
#define SCREEN_HEIGHT 240 // Top screen height
#define TILE_SIZE 20	  // Size of each tile (20x20 pixels)
#define MAX_PLATFORMS 150 // Max platforms per level

// Player Settings
#define PLAYER_JUMP_HEIGHT 5.0f

// Game Self Directives
bool hasCoinsUpdated = true;
bool hasPosUpdated = true;

bool hasPlayerDied = false;
bool isPlayerMoving = false;

// Check collision between two rectangles
bool checkCollision(float x1, float y1, float w1, float h1,
					float x2, float y2, float w2, float h2)
{
	return (x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2);
}

// Define level layouts as string literals
static const char level1_layout[] =
	"0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
	"0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
	"0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
	"0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
	"0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
	"0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
	"0000000000000000000000000000000000000000000002222220000000000000000000000000000000000000000000000000000000000"
	"0000000000000000000000000000000000000000000004444440000000000000000000000000000000000000000000000000000000000"
	"0000000000000000000000000000000000000000000004444440000000000000000000000000000222220000222220000000000000000"
	"0000000000000000000000000000000000001111111333333333311110000000000000000010000444440000444440000000000000000"
	"0000000000000000000001111111111111111111111111111111111110000000000000000110000444440000444440000000000000000"
	"3333111111111111111111111111111111111111111111111111111111111111133111111111111333333333333331111111111111111";

static const char level2_layout[] =
	"00000000000000000000000000000000000000000000000000000133333111111111100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
	"00000000000000000000000000000000000000000000000000000013333111111111100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
	"00000000000000000000000000000000000000000000000000000001333111111110000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
	"00000000000000000000000000000000000000000000000000000000133111111110000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
	"00000000000000000000000000000000000000000000000000000000013111111000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
	"00000000000000000000000000000000000000000000000000000000001111111000000000000000000000000000000000000000000000000000000000000000000000022222220000000000000000000000"
	"00000000000000000000000000000000000000000000000000000000000111100000000000000000000000000000000000000000000000000000000000000000000000004444400000000000000000000000"
	"10000000000000000000000000000000000000000111111111111111100111100000000022222000111100000000000000000000000000000000000000000000000000114444411000000000000000000000"
	"11000000000000000000000000000000000000000111111000000000000110000000000044444000111111110000000000000000000000000000000000000000110000113333311110000000000000000000"
	"11100000000000000001100112222222211110011111111001111111111110000000222244444000111111111111000000000000000000000000000000000000110000113333311111100000000000000000"
	"11110000000000000001133114400000000000011111111000000000000000000000444444444000111111111111111100000000000000000000000111110000112000113333311111111000000000000000"
	"11111111111111111111133113311111111111111111111111111111111111111133333333333333111111111111111133311111113111113111113111111111111111113333311111111333111111111111";

static const char level3_layout[] =
	"0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
	"0000000000000000000000000000000000000000000000000001100000000000000000000000000000000000000000000000000000000"
	"0000000000000000000000000000000000000000000000000011110000000000000000000000000000000000000000000000000000000"
	"0000000000000000000000022221100000000000000000000111111000000000000000000000000000000000000000000000000000000"
	"0000000000000000000000044441100000000000000000001111111100000000000000000000000000000000000000000000000000000"
	"0000000000000000000002244441100000000000000000011111111110000000000000000000000000000000000000000000000000000"
	"0000000000000000000004444441100000000000000000111111111111000000000000000000000000000000000000000000000000000"
	"0000000000000000000224444441100000000000000000014444444410000000000000000000000000000000000000000000000000000"
	"3310000000000000000444444441100000000000000000014444444410000000000000222211000000000000000000000000000000000"
	"3311000000000000022444444441100000000000000000014444444410000002220000000011000000000000000000001131000000000"
	"3311100000000000044444444441100000001110000000024444444420000004440000000011000000000000000000001131100000000"
	"3311111111111111113333333331133333331111111111111111111111111133333111111111333311113311113311111131111111111";

// Define levels with constant expressions
static Level levels[] = {
	{level1_layout, 109, 12, {{0}}, 0, 20 * 109},       // Level 1
	{level2_layout, 164, 12, {{0}}, 0, 20 * 164},       // Level 2
    {level3_layout, 109, 12, {{0}}, 0, 20 * 109}        // Level 3
};
static const int levelCount = sizeof(levels) / sizeof(levels[0]);
static int currentLevel = 0;

// Define Ending Items
static Item items[] = {
	{2080, SCREEN_HEIGHT - 60, 10, 10, true},       // Item in Level 1
	{3200, SCREEN_HEIGHT - 60, 10, 10, true},       // Item in Level 2
    {2108, SCREEN_HEIGHT - 50, 10, 10, true}        // Item in Level 3
};

static Coin coins[][MAX_COINS] = {
    { // Level 1 coins
        //{100, SCREEN_HEIGHT - 60, 10, 10, true},
        //{300, SCREEN_HEIGHT - 80, 10, 10, true},
        //{500, SCREEN_HEIGHT - 100, 10, 10, true},
        //{700, SCREEN_HEIGHT - 120, 10, 10, true},
        //{900, SCREEN_HEIGHT - 140, 10, 10, true},
        //{0, 0, 0, 0, false}, {0, 0, 0, 0, false}, {0, 0, 0, 0, false}, {0, 0, 0, 0, false}, {0, 0, 0, 0, false}
    },
    { // Level 2 coins
        //{50, SCREEN_HEIGHT - 60, 10, 10, true},
        //{150, SCREEN_HEIGHT - 80, 10, 10, true},
        //{250, SCREEN_HEIGHT - 100, 10, 10, true},
        //{0, 0, 0, 0, false}, {0, 0, 0, 0, false}, {0, 0, 0, 0, false}, {0, 0, 0, 0, false}, {0, 0, 0, 0, false}, {0, 0, 0, 0, false}, {0, 0, 0, 0, false}
    }
};

// Function to generate platforms based on the level data
void generatePlatforms(Level *level) {
    level->platformCount = 0;
    for (int y = 0; y < level->height; y++) {
        for (int x = 0; x < level->width; x++) {
            char tile = level->layout[y * level->width + x];
            if (tile == '1' || tile == '3' || tile == '2' || tile == '4') {
                int width = 1;
                while (x + width < level->width &&
                       level->layout[y * level->width + (x + width)] == tile) {
                    width++;
                }
                if (level->platformCount < MAX_PLATFORMS) {
                    level->platforms[level->platformCount] = (Platform){
                        (float)(x * TILE_SIZE), (float)(y * TILE_SIZE),
                        (float)(width * TILE_SIZE), (float)TILE_SIZE,
                        tile == '2',	// Semisolid Platform Tile
                        tile == '3',	// Lava Tile
                        tile == '4'     // Decorative Tile
                    };
                    level->platformCount++;
                }
                x += width - 1;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    // Initialize graphics
    gfxInitDefault();
    PrintConsole bottomScreen;
    consoleInit(GFX_BOTTOM, &bottomScreen);
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();
    C3D_RenderTarget *top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);

    // Game state setup
    GameState state = STATE_MENU;
    int menuSelection = MENU_PLAY;
    bool devMode = false;
    int coinCount = loadCoins();

    // Player setup
    Player player = {SCREEN_WIDTH / 2 - 10, SCREEN_HEIGHT - 40, 0, 0, 20, 20, true};
    float cameraX = 0;
    float jumpVelocity = -PLAYER_JUMP_HEIGHT;

    // Generate platforms for all levels
    for (int i = 0; i < levelCount; i++) {
        generatePlatforms(&levels[i]);
    }

    consoleSelect(&bottomScreen);
    printf("\x1b[26;1HSTART          = Main Menu");
    printf("\x1b[27;1HLEFT & RIGHT   = Main Menu");
    printf("\x1b[28;1HA              = Jump");

    int jumpCounter = 0; // A counter for counting the amount of times the player jumps in one session

    // Main loop
    while (aptMainLoop()) {
        hidScanInput();
        u32 kDown = hidKeysDown();
        u32 kHeld = hidKeysHeld();

        if (kHeld & KEY_L && kDown & KEY_R) {
            devMode = !devMode;

            consoleSelect(&bottomScreen);
            if (devMode) {
                printf("\x1b[29;1HSELECT         = Change Level (Dev Mode)");
                printf("\x1b[5;1HSession Jumps: %i", jumpCounter);
            }
            else {
                printf("\x1b[29;1H                                        ");
                printf("\x1b[3;1H                                          ");
                printf("\x1b[4;1H                                          ");
                printf("\x1b[5;1H                                          ");
            }
        }

        switch (state) {
            case STATE_MENU:
                // Menu navigation
                if (kDown & KEY_DOWN) {
                    menuSelection = (menuSelection + 1) % MENU_COUNT;
                }
                if (kDown & KEY_UP) {
                    menuSelection = (menuSelection - 1 + MENU_COUNT) % MENU_COUNT;
                }
                if (kDown & KEY_A) {
                    switch (menuSelection) {
                        case MENU_PLAY:
                            state = STATE_GAME;
                            currentLevel = 0;
                            player.x = SCREEN_WIDTH / 2 - 10;
                            player.y = SCREEN_HEIGHT - 40;
                            player.vx = 0;
                            player.vy = 0;
                            cameraX = 0;
                            for (int i = 0; i < levelCount; i++) items[i].active = true;
                            for (int i = 0; i < levelCount; i++)
                                for (int j = 0; j < MAX_COINS; j++) coins[i][j].active = true;
                            break;
                        case MENU_CREDITS:
                            state = STATE_CREDITS;
                            break;
                        case MENU_EXIT:
                            goto cleanup; // Exit the game
                    }
                }
                break;

            case STATE_GAME:
                // Return to menu on START
                if (kDown & KEY_START) {
                    state = STATE_MENU;
                    break;
                }
                if ((kDown & KEY_SELECT) && devMode) {
                    currentLevel = (currentLevel + 1) % levelCount;
                    player.x = SCREEN_WIDTH / 2 - 10;
                    player.y = SCREEN_HEIGHT - 40;
                    player.vx = 0;
                    player.vy = 0;
                    cameraX = 0;
                    items[currentLevel].active = true;
                    for (int j = 0; j < MAX_COINS; j++) coins[currentLevel][j].active = true;
                }

                // Player movement
                player.vx = 0;
                if (!hasPlayerDied) {
                    if (kHeld & KEY_LEFT) { player.vx = -2.0f; }
                    if (kHeld & KEY_RIGHT) { player.vx = 2.0f; }
                }

                if (kDown & KEY_A && player.onGround) {
                    player.vy = jumpVelocity;
                    player.onGround = false;
                    jumpCounter++;
                    if (devMode) printf("\x1b[5;1HSession Jumps: %i", jumpCounter);
                }

                if (hasPlayerDied) {
                    if (!(kHeld & KEY_LEFT) && !(kHeld & KEY_RIGHT)) hasPlayerDied = false;
                }

                // Apply gravity
                player.vy += 0.2f;

                // Update horizontal position with collision
                float newX = player.x + player.vx;
                for (int i = 0; i < levels[currentLevel].platformCount; i++) {
                    Platform p = levels[currentLevel].platforms[i];
                    if (p.decorative) continue;
                    if (!p.semisolid && !p.lava && !p.decorative && checkCollision(newX, player.y, player.width, player.height,
                                                                  p.x, p.y, p.width, p.height)) {
                        if (player.vx > 0) newX = p.x - player.width;
                        else if (player.vx < 0) newX = p.x + p.width;
                    }
                }
                player.x = newX;

                // Update vertical position with collision
                float newY = player.y + player.vy;
                player.onGround = false;
                for (int i = 0; i < levels[currentLevel].platformCount; i++) {
                    Platform p = levels[currentLevel].platforms[i];
                    if (checkCollision(player.x, newY, player.width, player.height,
                                       p.x, p.y, p.width, p.height)) {
                        if (p.decorative) continue;
                        if (p.lava) {
                            player.x = SCREEN_WIDTH / 2 - 10;
                            player.y = SCREEN_HEIGHT - 1000;
                            player.vx = 0;
                            player.vy = 0;
                            cameraX = 0;
                            hasPlayerDied = true;
                            break;
                        } else if (p.semisolid && player.vy > 0 && player.y + player.height - player.vy <= p.y) {
                            newY = p.y - player.height;
                            player.vy = 0;
                            player.onGround = true;
                        } else if (!p.semisolid) {
                            if (player.vy > 0) {
                                newY = p.y - player.height;
                                player.vy = 0;
                                player.onGround = true;
                            } else if (player.vy < 0) {
                                newY = p.y + p.height;
                                player.vy = 0;
                            }
                        }
                    }
                }
                player.y = newY;

                // Keep player within level bounds
                if (player.x < 0) player.x = 0;
                if (player.x + player.width > levels[currentLevel].levelWidth) {
                    player.x = levels[currentLevel].levelWidth - player.width;
                }
                if (player.y > SCREEN_HEIGHT) player.y = SCREEN_HEIGHT;

                // Coin collection
                for (int i = 0; i < MAX_COINS; i++) {
                    Coin *coin = &coins[currentLevel][i];
                    if (coin->active && checkCollision(player.x, player.y, player.width, player.height,
                                                       coin->x, coin->y, coin->width, coin->height)) {
                        coin->active = false;
                        coinCount++;
                        hasCoinsUpdated = true;
                    }
                }

                // Item collection (level completion)
                Item *item = &items[currentLevel];
                if (item->active && checkCollision(player.x, player.y, player.width, player.height,
                                                   item->x, item->y, item->width, item->height)) {
                    item->active = false;
                    saveCoins(coinCount); // Save coins on level completion
                    currentLevel = (currentLevel + 1) % levelCount;
                    player.x = SCREEN_WIDTH / 2 - 10;
                    player.y = SCREEN_HEIGHT - 40;
                    player.vx = 0;
                    player.vy = 0;
                    cameraX = 0;
                    items[currentLevel].active = true;
                    for (int j = 0; j < MAX_COINS; j++) coins[currentLevel][j].active = true;
                }

                // Update camera
                cameraX = player.x - (SCREEN_WIDTH / 2);
                if (cameraX < 0) cameraX = 0;
                if (cameraX > levels[currentLevel].levelWidth - SCREEN_WIDTH) {
                    cameraX = levels[currentLevel].levelWidth - SCREEN_WIDTH;
                }
                break;

            case STATE_CREDITS:
                if (kDown & KEY_START) {
                    state = STATE_MENU;
                }
                break;
        }

        // Rendering
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

        // Top screen
        C2D_TargetClear(top, C2D_Color32(0x00, 0x00, 0x00, 0xFF)); // Black background
        C2D_SceneBegin(top);

        if (state == STATE_MENU) {
            const char *menuItems[] = {"Play", "Credits", "Exit"};
            for (int i = 0; i < MENU_COUNT; i++) {
                float x = SCREEN_WIDTH / 2 - strlen(menuItems[i]) * 8; // Rough centering
                float y = SCREEN_HEIGHT / 2 - 20 + i * 40;
                C2D_DrawRectSolid(x - 10, y - 10, 0, strlen(menuItems[i]) * 16 + 20, 30,
                                  (i == menuSelection) ? C2D_Color32(0xFF, 0xFF, 0xFF, 0x80) : C2D_Color32(0x80, 0x80, 0x80, 0x80));
            }
        } else if (state == STATE_GAME) {
            // Draw platforms
            for (int i = 0; i < levels[currentLevel].platformCount; i++) {
                Platform p = levels[currentLevel].platforms[i];
                C2D_DrawRectSolid(p.x - cameraX, p.y, 0, p.width, p.height,
                                  p.lava ? C2D_Color32(0xFF, 0x33, 0x00, 0xFF) :
                                  p.semisolid ? C2D_Color32(0x00, 0xFF, 0x00, 0xFF) :
                                  p.decorative ? C2D_Color32(0x20, 0x20, 0x20, 0xFF) :
                                                C2D_Color32(0x00, 0x00, 0xFF, 0xFF));
            }

            // Draw coins
            for (int i = 0; i < MAX_COINS; i++) {
                Coin *coin = &coins[currentLevel][i];
                if (coin->active) {
                    C2D_DrawRectSolid(coin->x - cameraX, coin->y, 0, coin->width, coin->height,
                                      C2D_Color32(0xFF, 0xD7, 0x00, 0xFF)); // Gold
                }
            }

            // Draw item if active
            if (items[currentLevel].active) {
                C2D_DrawRectSolid(items[currentLevel].x - cameraX, items[currentLevel].y, 0,
                                  items[currentLevel].width, items[currentLevel].height,
                                  C2D_Color32(0xFF, 0xFF, 0x00, 0xFF));
            }

            // Draw player
            C2D_DrawRectSolid(player.x - cameraX, player.y, 0, player.width, player.height,
                              C2D_Color32(0xFF, 0x00, 0x00, 0xFF));
        } else if (state == STATE_CREDITS) {
            // Placeholder credits screen
            C2D_DrawRectSolid(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 20, 0, 200, 40,
                              C2D_Color32(0x00, 0x00, 0x00, 0x80));
            // Citro2D lacks text; this is a gray box
        }

        consoleSelect(&bottomScreen);
        if (hasCoinsUpdated) {
            printf("\x1b[1;1HCoins: %d", coinCount);
            hasCoinsUpdated = false;
        }

        if (player.vx != 0 || player.vy != 0) isPlayerMoving = true;
        else isPlayerMoving = false;

        if (devMode) {
            printf("\x1b[3;1Hvx: %.2f  vy: %.2f", player.vx, player.vy);
            printf("\x1b[4;1Hx: %.2f  y: %.2f", player.x, player.y);
        }

        gfxFlushBuffers();
		gfxSwapBuffers();

        C3D_FrameEnd(0);
    }

cleanup:
    // Cleanup
    C2D_Fini();
    C3D_Fini();
    gfxExit();
    return 0;
}
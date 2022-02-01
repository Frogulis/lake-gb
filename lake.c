#include <gb/gb.h>
#include <stdio.h>
#include <rand.h>
#include "sprites.c"

typedef struct {
    uint8_t x, y, dir, currentTile, northTile, eastTile, spriteID, changedDir, isActive;
} Actor;

typedef struct {
    uint8_t x, y, spriteID;
} Pearl;

int sign(int x)
{
    if(x > 0) return 1;
    if(x < 0) return -1;
    return 0;
}

void setSquidBearing_Strategy1(uint8_t targetX, uint8_t targetY, Actor *squid)
{
    int8_t diffX = targetX - squid->x;
    int8_t diffY = targetY - squid->y;

    if ((sign(diffX) == -1 && squid->dir == 3) ||
        (sign(diffX) ==  1 && squid->dir == 1) ||
        (sign(diffY) == -1 && squid->dir == 0) ||
        (sign(diffY) ==  1 && squid->dir == 2)) {
        return;
    } else {
        if (sign(diffX) == -1) {
            squid->dir = 3;
        } else if (sign(diffX) == 1) {
            squid->dir = 1;
        } else if (sign(diffY) == -1) {
            squid->dir = 0;
        } else if (sign(diffY) == 1) {
            squid->dir = 2;
        } else {
            squid->dir = 4;
        }
        squid->changedDir = 1;
    }
}

void setSquidBearing_Strategy2(uint8_t targetX, uint8_t targetY, Actor *squid)
{
    if (targetX == squid->x) {
        if (targetY < squid->y) {
            if (squid->dir != 0) squid->changedDir = 1;
            squid->dir = 0;
        }
        else {
            if (squid->dir != 2) squid->changedDir = 1;
            squid->dir = 2;
        }
    } else {
        if (targetX < squid->x) {
            if (squid->dir != 3) squid->changedDir = 1;
            squid->dir = 3;
        } else {
            if (squid->dir != 1) squid->changedDir = 1;
            squid->dir = 1;
        }
    }
}

void moveActor(uint8_t speed, Actor *a)
{
    if (!a->isActive) return;

    switch (a->dir) {
        case 0: {
            if (a->changedDir) {
                a->currentTile = a->northTile;
                a->changedDir = 0;
                set_sprite_tile(a->spriteID, a->currentTile);
                set_sprite_prop(a->spriteID, get_sprite_prop(a->spriteID) & ~S_FLIPY);
            }
            a->y -= speed;
            break;
        }
        case 1: {
            if (a->changedDir) {
                a->currentTile = a->eastTile;
                a->changedDir = 0;
                set_sprite_tile(a->spriteID, a->currentTile);
                set_sprite_prop(a->spriteID, get_sprite_prop(a->spriteID) & ~S_FLIPX);
            }
            a->x += speed;
            break;
        }
        case 2: {
            if (a->changedDir) {
                a->currentTile = a->northTile;
                a->changedDir = 0;
                set_sprite_tile(a->spriteID, a->currentTile);
                set_sprite_prop(a->spriteID, get_sprite_prop(a->spriteID) | S_FLIPY);
            }
            a->y += speed;
            break;
        }
        case 3: {
            if (a->changedDir) {
                a->currentTile = a->eastTile;
                a->changedDir = 0;
                set_sprite_tile(a->spriteID, a->currentTile);
                set_sprite_prop(a->spriteID, get_sprite_prop(a->spriteID) | S_FLIPX);
            }
            a->x -= speed;
            break;
        }
        default: break;
    }

    if (a->x < 8) {
        a->x = 8;
    } else if (a->x >= 160) {
        a->x = 160;
    }
    if (a->y < 12) {
        a->y = 12;
    } else if (a->y >= 150) {
        a->y = 150;
    }

    move_sprite(a->spriteID, a->x, a->y);
}

void runGame() {
    /* load sprites */
    set_sprite_data(0, 21, TileLabel); 

    set_sprite_tile(0, 0); // boat

    set_sprite_tile(1, 2); // bubbles
    set_sprite_tile(2, 3);
    set_sprite_tile(3, 4);

    set_sprite_tile(4, 5); // lilypad

    set_sprite_tile(5, 7); // squid
    set_sprite_tile(6, 7);
    set_sprite_tile(8, 7);

    set_sprite_tile(7, 9); // pearl

    const uint8_t NUMERAL_0 = 10;
    const uint8_t NUMERAL_1 = 11;
    const uint8_t NUMERAL_2 = 12;
    const uint8_t NUMERAL_3 = 13;
    const uint8_t NUMERAL_4 = 14;
    const uint8_t NUMERAL_5 = 15;
    const uint8_t NUMERAL_6 = 16;
    const uint8_t NUMERAL_7 = 17;
    const uint8_t NUMERAL_8 = 18;
    const uint8_t NUMERAL_9 = 19;
    set_sprite_tile(9, NUMERAL_0); // score digits
    move_sprite(9, 130, 130);
    move_sprite(10, 138, 130);
    set_sprite_tile(10, NUMERAL_0);

    set_sprite_tile(11, 20); // fuel sprite
    move_sprite(11, 10, 130);
    set_sprite_tile(12, NUMERAL_0); // fuel digit
    move_sprite(12, 18, 130);
    SHOW_SPRITES;

    /* load background and spread it across the whole back */
    set_bkg_data(0, 7, TileLabel);
    const uint8_t tiles[64] = {
        6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6
    };
    for (uint8_t x = 0; x < 4; x++) {
        for (uint8_t y = 0; y < 4; y++) {
            set_bkg_tiles(x * 8, y * 8, 8, 8, tiles);
        }
    }
    SHOW_BKG;

    /* game state */
    Actor player;
    player.x = 88;
    player.y = 78;
    player.dir = 4;
    player.spriteID = 0;
    player.northTile = 0;
    player.eastTile = 1;
    player.currentTile = player.northTile;
    player.isActive = 1;
    move_sprite(player.spriteID, player.x, player.y);
    uint16_t score = 0;
    uint8_t fuel = 99;

    Pearl pearl;
    pearl.spriteID = 7;
    pearl.x = ((uint8_t)rand()) % (uint8_t)130 + 20;
    pearl.y = ((uint8_t)rand()) % (uint8_t)110 + 20;
    move_sprite(pearl.spriteID, pearl.x, pearl.y);

    /* setup AI */
    Actor squid1;
    squid1.x = 60;
    squid1.y = 60;
    squid1.dir = 0;
    squid1.northTile = 7;
    squid1.eastTile = 8;
    squid1.currentTile = squid1.northTile;
    squid1.spriteID = 5;
    squid1.isActive = 1;
    setSquidBearing_Strategy1(player.x, player.y, &squid1);
    move_sprite(squid1.spriteID, squid1.x, squid1.y);

    Actor squid2;
    squid2.x = 20;
    squid2.y = 20;
    squid2.dir = 0;
    squid2.northTile = 7;
    squid2.eastTile = 8;
    squid2.currentTile = squid2.northTile;
    squid2.spriteID = 6;
    squid2.isActive = 1;
    setSquidBearing_Strategy1(player.x, player.y, &squid2);
    move_sprite(squid2.spriteID, squid2.x, squid2.y);

    Actor squid3;
    squid3.x = 0;
    squid3.y = 0;
    squid3.dir = 0;
    squid3.northTile = 7;
    squid3.eastTile = 8;
    squid3.currentTile = squid3.northTile;
    squid3.spriteID = 8;
    squid3.isActive = 0;
    setSquidBearing_Strategy1(player.x, player.y, &squid3);
    move_sprite(squid3.spriteID, squid3.x, squid3.y);
    
    uint8_t goFast = 0;

    while (1) {
        uint8_t keys;
        keys = joypad();
        if (keys & J_UP) {
            player.dir = 0;
            player.changedDir = 1;
        }
        else if (keys & J_DOWN) {
            player.dir = 2;
            player.changedDir = 1;
        }
        else if (keys & J_LEFT) {
            player.dir = 3;
            player.changedDir = 1;
        }
        else if (keys & J_RIGHT) {
            player.dir = 1;
            player.changedDir = 1;
        }
        else {
            player.dir = 4;
        }

        uint8_t boostSpeed = 0;
        if (keys & J_A) {
            if (fuel > 3) {
                fuel -= 4;
                boostSpeed = 1;
            }
            else {
                fuel = 0;
            }
        }
        else {
            if (fuel < 98) {
                fuel += 2;
            }
            else {
                fuel = 99;
            }
        }
        set_sprite_tile(12, NUMERAL_0 + (fuel / 10 % 10));

        if ((player.x + 3 >= squid1.x && player.x <= squid1.x + 3 && player.y + 3 >= squid1.y && player.y <= squid1.y + 3) ||
            (player.x + 3 >= squid2.x && player.x <= squid2.x + 3 && player.y + 3 >= squid2.y && player.y <= squid2.y + 3) ||
            (squid3.isActive && player.x + 3 >= squid3.x && player.x <= squid3.x + 3 && player.y + 3 >= squid3.y && player.y <= squid3.y + 3)) {
            break;
        }

        if (player.x + 5 >= pearl.x && player.x <= pearl.x + 5 && player.y + 5 >= pearl.y && player.y <= pearl.y + 5) {
            pearl.x = ((uint8_t)rand()) % (uint8_t)130 + 20;
            pearl.y = ((uint8_t)rand()) % (uint8_t)110 + 20;

            move_sprite(pearl.spriteID, pearl.x, pearl.y);
            score += 1;

            set_sprite_tile( 9, NUMERAL_0 + (score / 10 % 10));
            set_sprite_tile(10, NUMERAL_0 + (score % 10));

            if (score == 15) {
                squid3.isActive = 1;
            }
        }

        delay(32);
        setSquidBearing_Strategy1(player.x, player.y, &squid1);
        setSquidBearing_Strategy1(player.x, player.y, &squid2);
        setSquidBearing_Strategy2(player.x, player.y, &squid3);
        moveActor(1, &squid1);
        if (goFast) {
            goFast = 0;
        } else {
            goFast = 1;
        }
        moveActor(1 + goFast, &squid2);
        moveActor(1 + goFast, &squid3);
        moveActor(2 + boostSpeed, &player);
    }

    HIDE_SPRITES;
    printf("Game over!\nYour score is %u!\n", score);
}

void main(){
    printf("Press START to begin");
    waitpad(J_START);

    uint16_t seed = LY_REG;
    seed |= (uint16_t)DIV_REG << 8;
    initrand(seed);
    UINT8 r = ((uint8_t)rand()) % (uint8_t)2;
            printf("%d", r);

    while (1) {
        runGame();
        printf("Press START to try again.");
        waitpad(J_START);
    }
}

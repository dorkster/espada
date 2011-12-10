#include "SDL/SDL.h"
#include "clips.h"

void set_clips()
{
    // normal player
    clipPlayerNorm[0].x = 0;
    clipPlayerNorm[0].y = 0;
    clipPlayerNorm[0].w = 64;
    clipPlayerNorm[0].h = 64;

    clipPlayerNorm[1].x = 64;
    clipPlayerNorm[1].y = 0;
    clipPlayerNorm[1].w = 64;
    clipPlayerNorm[1].h = 64;
    
    // invulnerable player
    clipPlayerInvuln[0].x = 0;
    clipPlayerInvuln[0].y = 64;
    clipPlayerInvuln[0].w = 64;
    clipPlayerInvuln[0].h = 64;

    clipPlayerInvuln[1].x = 64;
    clipPlayerInvuln[1].y = 64;
    clipPlayerInvuln[1].w = 64;
    clipPlayerInvuln[1].h = 64;
    
    // normal enemy
    clipEnemyNorm[0].x = 0;
    clipEnemyNorm[0].y = 0;
    clipEnemyNorm[0].w = 64;
    clipEnemyNorm[0].h = 32;

    clipEnemyNorm[1].x = 64;
    clipEnemyNorm[1].y = 0;
    clipEnemyNorm[1].w = 64;
    clipEnemyNorm[1].h = 32;
}

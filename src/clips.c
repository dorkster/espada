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
    
    // enemy type 1
    clipEnemyType1[0].x = 0;
    clipEnemyType1[0].y = 0;
    clipEnemyType1[0].w = 64;
    clipEnemyType1[0].h = 32;

    clipEnemyType1[1].x = 64;
    clipEnemyType1[1].y = 0;
    clipEnemyType1[1].w = 64;
    clipEnemyType1[1].h = 32;
    
    // enemy type 2
    clipEnemyType2[0].x = 0;
    clipEnemyType2[0].y = 0;
    clipEnemyType2[0].w = 64;
    clipEnemyType2[0].h = 64;

    clipEnemyType2[1].x = 64;
    clipEnemyType2[1].y = 0;
    clipEnemyType2[1].w = 64;
    clipEnemyType2[1].h = 64;
    
    // explosion
    clipExplosion[0].x = 0;
    clipExplosion[0].y = 0;
    clipExplosion[0].w = 64;
    clipExplosion[0].h = 64;

    clipExplosion[1].x = 64;
    clipExplosion[1].y = 0;
    clipExplosion[1].w = 64;
    clipExplosion[1].h = 64;
    
    clipExplosion[2].x = 128;
    clipExplosion[2].y = 0;
    clipExplosion[2].w = 64;
    clipExplosion[2].h = 64;
    
    clipExplosion[3].x = 192;
    clipExplosion[3].y = 0;
    clipExplosion[3].w = 64;
    clipExplosion[3].h = 64;
    
    clipExplosion[4].x = 64;
    clipExplosion[4].y = 0;
    clipExplosion[4].w = 64;
    clipExplosion[4].h = 64;
    
    clipExplosion[5].x = 128;
    clipExplosion[5].y = 0;
    clipExplosion[5].w = 64;
    clipExplosion[5].h = 64;
    
    clipExplosion[6].x = 192;
    clipExplosion[6].y = 0;
    clipExplosion[6].w = 64;
    clipExplosion[6].h = 64;
    
    clipExplosion[7].x = 192;
    clipExplosion[7].y = 0;
    clipExplosion[7].w = 64;
    clipExplosion[7].h = 64;
}

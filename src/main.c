/*
    Espada - A retro 2D space shooter
    Copyright (C) 2011  Justin Jacobs

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"

#include "iniparser.h"

#include <stdlib.h>
#include <time.h>

#include "main.h"
#include "clips.h"

//------------------------------
// System functions
//------------------------------
int sys_rand(int low, int high) // Generate a random number in a specific range
{
    int r = rand() % (high - low + 1) + low;
    return r;
}

bool sys_collide( SDL_Rect A, SDL_Rect B ) // Thanks to lazyfoo.net
{    
    //If any of the sides from A are outside of B
    if( (A.y + A.h) <= B.y )
        return false;
    
    if( A.y >= (B.y + B.h) )
        return false;
    
    if( (A.x + A.w) <= B.x )
        return false;
    
    if( A.x >= (B.x + B.w) )
        return false;
    
    //If none of the sides from A are outside B
    return true;
}

bool sys_init()
{
    if(SDL_Init(SDL_INIT_EVERYTHING) == -1) { return false; }
    
    screen = SDL_SetVideoMode(SCREEN_WIDTH,SCREEN_HEIGHT,SCREEN_BPP,SDL_SWSURFACE);
    if(screen == NULL) { return false; }
    
    if( TTF_Init() == -1 ) { return false; }
    
    if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 ) { return false; }
    
    SDL_WM_SetCaption("Espada",NULL);
    
    sys_configpath = strcat(getenv("XDG_CONFIG_HOME"),"/espada.ini");
    
    return true;
}

void sys_configcreate()
{
    FILE* f;
    
    f = fopen(sys_configpath, "w");
    if(f != NULL)
    {
        fprintf(f,
        "[config]\n"
        "sound=6;\n"
        "music=8;\n"
        "\n");
        fclose(f);
    }
}

void sys_configupdate()
{
    FILE* f;
    
    f = fopen(sys_configpath, "w");
    if(f != NULL)
    {
        fprintf(f,
        "[config]\n"
        "sound=%d;\n"
        "music=%d;\n"
        "\n",sound_volfx,sound_volmus);
        fclose(f);
    }
}

void sys_configload()
{
    dictionary* f;
    
    f = iniparser_load(sys_configpath);
    if(f == NULL)
    {
        sys_configcreate();
        sys_configload();
    }
    else
    {
        sound_setvolumes(iniparser_getint(f,"config:sound",-1),iniparser_getint(f,"config:music",-1));
    }
}

bool sys_loadfiles()
{
    //Font
    font = TTF_OpenFont( "res/LCD_Solid.ttf", 20 );
    if( font == NULL ) { return false; }
    
    //Textures
    background = image_load("res/background.png",false);
    if(background == NULL) { return false; }
    
    title_graphic = image_load("res/title.png",true);
    if(title_graphic == NULL) { return false; }
    
    menu_cursor = image_load("res/menu_cursor.png",true);
    if(menu_cursor == NULL) { return false; }
    
    sprite_player = image_load("res/player_ship.png",true);
    if(sprite_player == NULL) { return false; }
    
    sprite_health_full = image_load("res/health_full.png",true);
    if(sprite_health_full == NULL) { return false; }
    
    sprite_health_empty = image_load("res/health_empty.png",true);
    if(sprite_health_empty == NULL) { return false; }
    
    sprite_laser = image_load("res/laser.png",true);
    if(sprite_laser == NULL) { return false; }
    
    sprite_laser_enemy = image_load("res/laser_enemy.png",true);
    if(sprite_laser_enemy == NULL) { return false; }
    
    sprite_enemy = image_load("res/enemy_ship.png",true);
    if(sprite_enemy == NULL) { return false; }
    
    sprite_explosion = image_load("res/explosion.png",true);
    if(sprite_explosion == NULL) { return false; }
    
    //Sound
    music = Mix_LoadMUS("res/music1.ogg");
    if(music == NULL) { return false; }
    
    snd_player_fire = Mix_LoadWAV("res/player_fire.wav");
    if(snd_player_fire == NULL) { return false; }
    
    snd_enemy_fire = Mix_LoadWAV("res/enemy_fire.wav");
    if(snd_enemy_fire == NULL) { return false; }
    
    snd_explosion = Mix_LoadWAV("res/explosion.wav");
    if(snd_explosion == NULL) { return false; }
    
    return true;
}

void sys_cleanup()
{
    SDL_FreeSurface(background);
    SDL_FreeSurface(title_graphic);
    SDL_FreeSurface(menu_cursor);
    SDL_FreeSurface(sprite_player);
    SDL_FreeSurface(sprite_health_full);
    SDL_FreeSurface(sprite_health_empty);
    SDL_FreeSurface(sprite_laser);
    SDL_FreeSurface(sprite_laser_enemy);
    SDL_FreeSurface(sprite_enemy);
    SDL_FreeSurface(sprite_explosion);
    
    Mix_FreeMusic(music);
    Mix_FreeChunk(snd_player_fire);
    Mix_FreeChunk(snd_enemy_fire);
    Mix_FreeChunk(snd_explosion);
    
    TTF_CloseFont(font);
    
    SDL_Quit();
}

//------------------------------
// Image functions
//------------------------------

SDL_Surface *image_load(char * filename, bool withalpha)
{
    SDL_Surface *loadedImage = NULL;
    SDL_Surface *optimizedImage = NULL;
    
    loadedImage = IMG_Load(filename);
    
    if(loadedImage != NULL)
    {
        optimizedImage = SDL_DisplayFormat(loadedImage);
        if(optimizedImage != NULL && withalpha == true)
        {
            Uint32 colorkey = SDL_MapRGB( optimizedImage->format, 0xFF, 0, 0xFF );
            SDL_SetColorKey( optimizedImage, SDL_SRCCOLORKEY, colorkey );
        }
        SDL_FreeSurface(loadedImage);
    }
    
    return optimizedImage;
}

void image_apply( int x, int y, int alpha, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip )
{
    //Make a temporary rectangle to hold the offsets
    SDL_Rect offset;

    //Give the offsets to the rectangle
    offset.x = x;
    offset.y = y;
    
    //Blit the surface
    SDL_SetAlpha( source, SDL_SRCALPHA, alpha);
    SDL_BlitSurface( source, clip, destination, &offset );
}

//------------------------------
// Sound functions
//------------------------------
void sound_playfx(Mix_Chunk* snd)
{
    if(sound_enabled == true)
    {
        Mix_VolumeChunk(snd, sound_volfx*10);
        Mix_PlayChannel( -1, snd, 0 );
    }
}

void sound_playmus()
{
    if(sound_enabled == true)
    {
        if(Mix_FadeInMusic(music, -1, sound_fadetime) == -1)
            return;
        else
            Mix_VolumeMusic(sound_volmus*10);
    }
}

void sound_setvolumes(int snd, int mus)
{
    sound_volfx = snd;
    sound_volmus = mus;
    sound_volmus_paused = mus/2;
    
    if(sound_volmus == 1)
        sound_volmus_paused = 1;
}

//------------------------------
// Drawing functions
//------------------------------
void draw_frameadvance(int* frame,int totalframes)
{
    if(animationTimer == 0)
    {
        *frame += 1;
        if(*frame > totalframes-1)
            *frame = 0;
    }
}

void draw_background()
{
    int scrollspeed = 10;
    
    if(background_y < 640)
    {
        background_y += scrollspeed;
    }
    else
        background_y = 0;
        
    image_apply(0,background_y,255,background,screen,NULL);
    image_apply(0,background_y-640,255,background,screen,NULL);

}

void draw_titlescreen()
{
    char tempstr[16];
    
    image_apply((SCREEN_WIDTH-486)/2,50,255,title_graphic,screen,NULL);
    
    int i;
    for(i=0;i<3;i++)
    {
        if(menu_level == 1)
        {
            if(i == 0)
                sprintf(tempstr,"%s%d",menu_main[menu_level][i],sound_volfx);
            else if(i == 1)
                sprintf(tempstr,"%s%d",menu_main[menu_level][i],sound_volmus);
            else
                sprintf(tempstr,"%s",menu_main[menu_level][i]);
        }
        else
        {
            sprintf(tempstr,"%s",menu_main[menu_level][i]);
        }
        
        text_titlescreen = TTF_RenderText_Solid( font, tempstr, textColor );
        if(text_titlescreen == NULL){ return; }
        
        image_apply(280, 300+(i*20), 255, text_titlescreen, screen, NULL);
        SDL_FreeSurface(text_titlescreen);
    }
    
    image_apply(260, 300+(menu_selection*20), 255, menu_cursor, screen, NULL);
}

void draw_info()
{
    int i;
    
    char score[64];
    
    sprintf(score,"Score: %d",obj_player.score);
    text_score = TTF_RenderText_Solid( font, score, textColor );
    
    if(text_score == NULL){ return; }
    image_apply(5, 5+SCREEN_BOTTOM, 255, text_score, screen, NULL);
    SDL_FreeSurface(text_score);
    
    char health[64];
    
    sprintf(health,"Health:");
    text_health = TTF_RenderText_Solid( font, health, textColor );
    
    if(text_health == NULL){ return; }
    image_apply(SCREEN_WIDTH-200, 5+SCREEN_BOTTOM, 255, text_health, screen, NULL);
    SDL_FreeSurface(text_health);
    
    for(i=1;i<=obj_player.health;i++)
        image_apply((SCREEN_WIDTH-120)+(i*18), 3+SCREEN_BOTTOM, 255, sprite_health_full, screen, NULL);
    
    for(i=obj_player.health+1;i<=5;i++)
        image_apply((SCREEN_WIDTH-120)+(i*18), 3+SCREEN_BOTTOM, 255, sprite_health_empty, screen, NULL);
}

void draw_statustext(char* text)
{
    int len = strlen(text);
    int xpos = (SCREEN_WIDTH-(len*12))/2;
    text_status = TTF_RenderText_Solid( font, text, textColor );
    
    if(text_status == NULL){ return; }
    image_apply(xpos, 200, 255, text_status, screen, NULL);
    SDL_FreeSurface(text_status);
}

void draw_player()
{
    int alpha;
        
    if(obj_player.alive == true)
    {
        if(obj_player.invuln == false)
        {
            alpha = 255;
            image_apply(obj_player.dim.x,obj_player.dim.y,alpha,sprite_player,screen,&clipPlayerNorm[obj_player.frame]);
        }
        else
        {
            alpha = 127;
            image_apply(obj_player.dim.x,obj_player.dim.y,alpha,sprite_player,screen,&clipPlayerInvuln[obj_player.frame]);
        }
        
        
        int totalframes = sizeof(clipPlayerNorm)/sizeof(SDL_Rect);
        draw_frameadvance(&obj_player.frame,totalframes);
    }
}

void draw_enemies()
{
    int i;
    
    for(i=0;i<MAXENEMIES;i++)
    {
        if(obj_enemy[i].alive == true)
        {
            image_apply(obj_enemy[i].dim.x,obj_enemy[i].dim.y,255,sprite_enemy,screen,&clipEnemyNorm[obj_enemy[i].frame]);
            
            int totalframes = sizeof(clipEnemyNorm)/sizeof(SDL_Rect);
            draw_frameadvance(&obj_enemy[i].frame,totalframes);
        }
    }
}

void draw_lasers()
{
    int i,j;
    
    //player lasers
    for(i=0;i<MAXLASERS;i++)
    {
        if(obj_player.laz[i].alive == true)
        {
            image_apply(obj_player.laz[i].dim.x,obj_player.laz[i].dim.y,255,sprite_laser,screen,NULL);
        }
    }
    
    // enemy lasers
    for(j=0;j<MAXENEMIES;j++)
    {
        for(i=0;i<MAXLASERS;i++)
        {
            if(obj_enemy[j].laz[i].alive == true)
            {
                image_apply(obj_enemy[j].laz[i].dim.x,obj_enemy[j].laz[i].dim.y,255,sprite_laser_enemy,screen,NULL);
            }
        }
    }
}

void draw_explosions()
{
    int i;
    
    for(i=0;i<MAXEXPLOSIONS;i++)
    {
        if(obj_explosion[i].alive == true)
        {
            image_apply(obj_explosion[i].dim.x,obj_explosion[i].dim.y,255,sprite_explosion,screen,&clipExplosion[obj_explosion[i].frame]);
            
            int totalframes = sizeof(clipExplosion)/sizeof(SDL_Rect);
            draw_frameadvance(&obj_explosion[i].frame,totalframes);
            
            if(obj_explosion[i].frame == totalframes-1)
            {
                obj_explosion[i].alive = false;
            }
        }
    }
}

//------------------------------
// Gameplay & Logic functions
//------------------------------
void game_newgame()
{
    gamestate_init = true;
    gamestate_title = false;
    gamestate_over = false;
    gamestate_pause = false;
    action_fire = false;
    action_moveleft = false;
    action_moveright = false;
    action_moveup = false;
    action_movedown = false;
    game_lasersdestroy();
    game_playerspawn();
    game_enemyspawn();
    gamestate_init = false;
    
    sound_playmus();
    
    int i;
    for(i=0;i<MAXEXPLOSIONS;i++)
    {
        obj_explosion[i].alive = false;
    }
}

void game_titlescreen()
{
    gamestate_over = true;
    gamestate_pause = false;
    gamestate_title = true;
    Mix_FadeOutMusic(sound_fadetime);
    Mix_HaltChannel(-1);
}

void game_pause()
{
    if(gamestate_pause == false)
    {
        Mix_VolumeMusic(sound_volmus_paused*10);
        gamestate_pause = true;
    }
    else
    {
        Mix_VolumeMusic(sound_volmus*10);
        gamestate_pause = false;
    }
}

void game_testcollisions()
{
    int i,j;
    
    // Check if player lasers hit enemies
    for(i=0;i<MAXLASERS;i++)
    {
        for(j=0;j<MAXENEMIES;j++)
        {
            if(obj_player.alive && obj_player.laz[i].alive == true && obj_enemy[j].alive == true && (obj_enemy[j].dim.y + obj_enemy[j].dim.h) >= 0)
            {
                if(sys_collide(obj_player.laz[i].dim,obj_enemy[j].dim) == true)
                {
                    obj_enemy[j].alive = false;
                    obj_player.laz[i].alive = false;
                    enemyTimer = 30;
                    obj_player.score += 10;
                    game_explosionspawn(obj_enemy[j].dim.x,obj_enemy[j].dim.y);
                    sound_playfx(snd_explosion);
                    break;
                }
            }
        }
    }
    
    // Check if enemy lasers hit player
    for(j=0;j<MAXENEMIES;j++)
    {
        for(i=0;i<MAXLASERS;i++)
        {
            if(obj_player.alive == true && obj_enemy[j].laz[i].alive == true)
            {
                if(sys_collide(obj_player.dim,obj_enemy[j].laz[i].dim) == true)
                {
                    if(obj_player.invuln == false)
                    {
                        obj_enemy[j].laz[i].alive = false;
                        game_playerdamage(1);
                    }
                    break;
                }
            }
        }
    }
    
    //Check if enemies hit the player
    for(j=0;j<MAXENEMIES;j++)
    {
        if(obj_enemy[j].alive == true && obj_player.alive == true)
        {
            if(sys_collide(obj_enemy[j].dim,obj_player.dim) == true)
            {
                if(obj_player.invuln == false)
                {
                    obj_enemy[j].alive = false;
                    game_playerdamage(2);
                }
                break;
            }
        }
    }
}

void game_playerspawn()
{
    obj_player.alive = true;
    obj_player.invuln = false;
    obj_player.invulnTimer = 0;
    obj_player.score = 0;
    obj_player.health = 5;
    
    obj_player.dim.w = 64;
    obj_player.dim.h = 64;
    obj_player.dim.x = 295;
    obj_player.dim.y = SCREEN_BOTTOM - obj_player.dim.h;
    
    obj_player.frame = 0;
}

void game_playermove()
{
    int movespeed = 8;
    
    if(action_moveleft == true)
    {
        obj_player.dim.x -= movespeed;
        if(obj_player.dim.x < 0)
            obj_player.dim.x = 0;
    }
    else if(action_moveright == true)
    {
        obj_player.dim.x += movespeed;
        if((obj_player.dim.x + obj_player.dim.w) > SCREEN_WIDTH)
            obj_player.dim.x = SCREEN_WIDTH - obj_player.dim.w;
    }
    if(action_moveup == true)
    {
        obj_player.dim.y -= movespeed/2;
        if(obj_player.dim.y < 0)
            obj_player.dim.y = 0;
    }
    else if(action_movedown == true)
    {
        obj_player.dim.y += movespeed/2;
        if((obj_player.dim.y + obj_player.dim.h) > SCREEN_BOTTOM)
            obj_player.dim.y = SCREEN_BOTTOM - obj_player.dim.h;
    }
}

void game_playerfire()
{
    int i;
    
    if(action_fire == true && obj_player.laserTimer == 0)
    {
        for(i=0;i<MAXLASERS;i++)
        {
            obj_player.laz[i].dim.w = 8;
            obj_player.laz[i].dim.h = 16;
            
            if(obj_player.laz[i].alive != true)
            {
                obj_player.laz[i].alive = true;
                obj_player.laz[i].dim.x = obj_player.dim.x + (obj_player.dim.w/2);
                obj_player.laz[i].dim.y = obj_player.dim.y - obj_player.laz[i].dim.h;
                obj_player.laserTimer = 15;
                sound_playfx(snd_player_fire);
                break;
            }
        }
    }
    
    if(obj_player.laserTimer > 0)
        obj_player.laserTimer--;
}

void game_playerdamage(int d)
{
    obj_player.invuln = true;
    obj_player.invulnTimer = 100;
    obj_player.health -= d;
    sound_playfx(snd_explosion);
    
    // Check if player is dead
    if(obj_player.health <= 0)
    {
        obj_player.health = 0;
        obj_player.alive = false;
        game_explosionspawn(obj_player.dim.x,obj_player.dim.y);
        gamestate_over = true;
    }
}

void game_playerinvulntick()
{
    if(obj_player.invulnTimer != 0)
        obj_player.invulnTimer--;
    else
        obj_player.invuln = false;
}

void game_enemyspawn()
{
    int i;
    
    if(gamestate_init == true)
    {
        for(i=0;i<MAXENEMIES;i++)
        {
            obj_enemy[i].alive = false;
        }
    }

        for(i=0;i<MAXENEMIES;i++)
        {
            obj_enemy[i].dim.w = 64;
            obj_enemy[i].dim.h = 32;
            
            if(obj_enemy[i].alive != true && enemyTimer == 0)
            {
                obj_enemy[i].alive = true;
                obj_enemy[i].frame = 0;
                obj_enemy[i].pathlength = 0;
                obj_enemy[i].laserTimer = 0;
                obj_enemy[i].dir = sys_rand(0,1);
                obj_enemy[i].dim.x = sys_rand(0,SCREEN_WIDTH - obj_enemy[i].dim.w);
                obj_enemy[i].dim.y = sys_rand(-500,-50);
                break;
            }
        }
       
}

void game_enemymove()
{
    int movespeed = 2;
    
    int i;

    for(i=0;i<MAXENEMIES;i++)
    {
        if(obj_enemy[i].alive == true)
        {
            if(obj_enemy[i].pathlength == 0)
            {
                obj_enemy[i].pathlength = sys_rand(10,SCREEN_WIDTH/2);
            }
            if(obj_enemy[i].pathlength != 0)
            {
                if(obj_enemy[i].dir == 0)
                {
                    if(obj_enemy[i].dim.x + obj_enemy[i].dim.w < SCREEN_WIDTH)
                    {
                        obj_enemy[i].dim.x += movespeed;
                        obj_enemy[i].pathlength--;
                    }
                    if(obj_enemy[i].dim.x + obj_enemy[i].dim.w >= SCREEN_WIDTH || obj_enemy[i].pathlength == 0)
                    {
                        obj_enemy[i].dir = 1;
                        obj_enemy[i].pathlength = 0;
                    }
                }
                else if(obj_enemy[i].dir == 1)
                {
                    if(obj_enemy[i].dim.x > 0)
                    {
                        obj_enemy[i].dim.x -= movespeed;
                        obj_enemy[i].pathlength--;
                    }
                    if(obj_enemy[i].dim.x <= 0 || obj_enemy[i].pathlength == 0)
                    {
                        obj_enemy[i].dir = 0;
                        obj_enemy[i].pathlength = 0;
                    }
                }
            }
            
            obj_enemy[i].dim.y += 1;
        }
        
        if(obj_enemy[i].dim.y > SCREEN_BOTTOM+obj_enemy[i].dim.h)
        {
            obj_enemy[i].alive = false;
            obj_enemy[i].dim.x = 0;
            obj_enemy[i].dim.y = 0;
            if(gamestate_over == false)
                obj_player.score -= 50;
            if(obj_player.score < 0)
                obj_player.score = 0;
            break;
        }
    }
    if(enemyTimer > 0)
        enemyTimer--;
}

void game_enemyfire()
{
    int i,j;
    
    for(j=0;j<MAXENEMIES;j++)
    {
        if(obj_enemy[j].laserTimer == 0 && obj_enemy[j].alive && (obj_enemy[j].dim.y + obj_enemy[j].dim.h) >= 0)
        {
            for(i=0;i<MAXLASERS;i++)
            {
                obj_enemy[j].laz[i].dim.w = 8;
                obj_enemy[j].laz[i].dim.h = 16;
                
                if(obj_enemy[j].laz[i].alive != true)
                {
                    obj_enemy[j].laz[i].alive = true;
                    obj_enemy[j].laz[i].dim.x = obj_enemy[j].dim.x + (obj_enemy[j].dim.w/2);
                    obj_enemy[j].laz[i].dim.y = obj_enemy[j].dim.y + obj_enemy[j].laz[i].dim.h;
                    obj_enemy[j].laserTimer = sys_rand(100,250);
                    sound_playfx(snd_enemy_fire);
                    break;
                }
            }
        }
        
        if(obj_enemy[j].laserTimer > 0)
            obj_enemy[j].laserTimer--;
    }
}

void game_lasersmove()
{
    int movespeed = 10;
    int i,j;
    
    for(i=0;i<MAXLASERS;i++)
    {
        if(obj_player.laz[i].alive == true)
        {
            obj_player.laz[i].dim.y -= movespeed;
        }
        if(obj_player.laz[i].dim.y < 0)
            obj_player.laz[i].alive = false;
    }
    
    for(j=0;j<MAXENEMIES;j++)
    {
        for(i=0;i<MAXLASERS;i++)
        {
            if(obj_enemy[j].laz[i].alive == true)
            {
                obj_enemy[j].laz[i].dim.y += movespeed/2;
            }
            if(obj_enemy[j].laz[i].dim.y > SCREEN_HEIGHT)
                obj_enemy[j].laz[i].alive = false;
        }
    }
}

void game_lasersdestroy()
{
    int i,j;
    
    for(i=0;i<MAXLASERS;i++)
        obj_player.laz[i].alive = false;
    
    for(j=0;j<MAXENEMIES;j++)
        for(i=0;i<MAXLASERS;i++)
            obj_enemy[j].laz[i].alive = false;
}

void game_explosionspawn(int x, int y)
{
    int i;
    
    for(i=0;i<MAXEXPLOSIONS;i++)
    {
        if(obj_explosion[i].alive == false)
        {
            obj_explosion[i].alive = true;
            obj_explosion[i].dim.x = x;
            obj_explosion[i].dim.y = y;
            obj_explosion[i].dim.w = 64;
            obj_explosion[i].dim.h = 64;
            obj_explosion[i].frame = 0;
            break;
        }
    }
}

//------------------------------
// Main game loop
//------------------------------
int main(int argc, char* argv[])
{
    srand(time(0));
    
    bool quit = false;
    
    if(sys_init() == false) { return 1; }
    if(sys_loadfiles() == false) { return 1; }
    
    set_clips();
    sys_configload();
    
    while(quit == false)
    {
        startTimer = SDL_GetTicks();
        
        while(SDL_PollEvent(&event))
        {
            if( event.type == SDL_KEYDOWN )
            {                
                if(gamestate_over == false && gamestate_title == false)
                {
                    if(event.key.keysym.sym == SDLK_LEFT)
                        action_moveleft = true;
                    if(event.key.keysym.sym == SDLK_RIGHT)
                        action_moveright = true;
                    if(event.key.keysym.sym == SDLK_UP)
                        action_moveup = true;
                    if(event.key.keysym.sym == SDLK_DOWN)
                        action_movedown = true;
                    if(event.key.keysym.sym == 'z')
                        action_fire = true;
                    
                    if(event.key.keysym.sym == 'p' || event.key.keysym.sym == SDLK_ESCAPE)
                        game_pause();
                    if(gamestate_pause == true)
                    {
                        if(event.key.keysym.sym == 'q')
                            game_titlescreen();
                    }
                }
                if(gamestate_over == true && gamestate_title == true)
                {
                    if(event.key.keysym.sym == SDLK_DOWN && menu_selection+1 < 3)
                        menu_selection += 1;
                    if(event.key.keysym.sym == SDLK_UP && menu_selection > 0)
                        menu_selection -= 1;

                    if(menu_level == 0) // main menu
                    {
                        if(event.key.keysym.sym == 'z')
                        {
                            if(menu_selection == 0)
                                game_newgame();
                            if(menu_selection == 1)
                            {
                                menu_level = 1;
                                menu_selection = 0;
                            }
                            if(menu_selection == 2)
                                quit = true;
                        }
                    }
                    else if (menu_level == 1) // options menu
                    {
                        if(event.key.keysym.sym == SDLK_LEFT)
                        {
                            if(menu_selection == 0)
                                if(sound_volfx-1 >= 0)
                                    sound_setvolumes(sound_volfx-1,sound_volmus);
                            if(menu_selection == 1)
                                if(sound_volmus-1 >= 0)
                                    sound_setvolumes(sound_volfx,sound_volmus-1);
                        }
                        if(event.key.keysym.sym == SDLK_RIGHT)
                        {
                            if(menu_selection == 0)
                                if(sound_volfx+1 <= 12)
                                    sound_setvolumes(sound_volfx+1,sound_volmus);
                            if(menu_selection == 1)
                                if(sound_volmus+1 <= 12)
                                    sound_setvolumes(sound_volfx,sound_volmus+1);
                        }
                        if(event.key.keysym.sym == 'z')
                        {
                            if(menu_selection == 2)
                            {
                                menu_level = 0;
                                menu_selection = 0;
                            }
                        }
                    }
                }
                if(gamestate_over == true && gamestate_title == false)
                {
                    if(event.key.keysym.sym == 'q')
                        game_titlescreen();
                }
            }
            else if( event.type == SDL_KEYUP )
            {
                if(gamestate_over == false)
                {
                    if(event.key.keysym.sym == SDLK_LEFT)
                        action_moveleft = false;
                    if(event.key.keysym.sym == SDLK_RIGHT)
                        action_moveright = false;
                    if(event.key.keysym.sym == SDLK_UP)
                        action_moveup = false;
                    if(event.key.keysym.sym == SDLK_DOWN)
                        action_movedown = false;
                    if(event.key.keysym.sym == 'z')
                        action_fire = false;
                }
            }
            if(event.type == SDL_QUIT)
            {
                quit = true;
            }
        }
        
        if(gamestate_pause != true)
        {
            // Fill the screen with black
            SDL_FillRect(screen,NULL, 0x000000);
            
            // Draw background
            draw_background();
            
            if(gamestate_title == true)
            {
                draw_titlescreen();
            }
            else
            {
                // Draw explosions
                draw_explosions();
                
                if(gamestate_over == false)
                {
                    // Move and draw the player
                    game_playerinvulntick();
                    game_playermove();
                    draw_player();
                    
                    // Fire player lasers
                    game_playerfire();
                }
                
                // Spawn and draw enemies
                game_enemyspawn();
                game_enemymove();
                game_enemyfire();
                draw_enemies();
                
                game_lasersmove();
                draw_lasers();
                
                
                // Draw the gameover text when the game is over
                if(gamestate_over == true)
                {
                    draw_statustext("Game Over | Press 'q' to continue");
                }
                
                // Draw score and lives
                draw_info();
                
                // Collision Detection
                game_testcollisions();
            }
            
            //Update animations
            if(animationTimer > 0)
                animationTimer--;
            else
                animationTimer = 2;
        }
        else
        {
            draw_statustext("Game Paused | Press 'q' to quit");
        }
        
        //Update the screen
        if(SDL_Flip(screen) == -1) { return 1; }
            
        endTimer = SDL_GetTicks();
        deltaTimer = endTimer - startTimer;
        if ( deltaTimer < ( 1000 / FPS ))
        {
            SDL_Delay( ( 1000 / FPS ) - deltaTimer );
        }
    }
    
    sys_configupdate();
    sys_cleanup();
    
    return 0;
}

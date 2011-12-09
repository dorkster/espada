/*
    Simple Space Shooter
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
#include <stdlib.h>
#include <time.h>

#define MAXLASERS 5
#define MAXENEMIES 4

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define SCREEN_BPP 32
#define SCREEN_BOTTOM SCREEN_HEIGHT-30

typedef enum { false = 0, true = 1 } bool;

int randrange(int low, int high);
bool init();
SDL_Surface *load_image(char * filename);
bool load_files();
void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination );
void clean_up();

const int FPS = 60;
int startTimer;
int endTimer;
int deltaTimer;
int enemyTimer;

// The main drawing area
SDL_Surface* screen = NULL;

// Background image
SDL_Surface* background = NULL;
int background_y = 0;

// Sprites
SDL_Surface* sprite_player = NULL;
SDL_Surface* sprite_laser = NULL;
SDL_Surface* sprite_laser_enemy = NULL;
SDL_Surface* sprite_enemy = NULL;

// Text surfaces
TTF_Font *font = NULL;
SDL_Color textColor = { 255, 255, 255 };
SDL_Surface* text_score = NULL;
SDL_Surface* text_health = NULL;
SDL_Surface* text_gameover = NULL;

// Sound and music
Mix_Music* music = NULL;
Mix_Chunk* snd_player_fire = NULL;
Mix_Chunk* snd_explosion = NULL;

// Event structure
SDL_Event event;

bool game_init = true;
bool game_over = false;

typedef struct laser{
    bool alive;
    SDL_Rect dim;
}laser;

typedef struct player{
    bool alive;
    SDL_Rect dim;
    int score;
    int health;
    int laserTimer;
    laser l[MAXLASERS];
}player;

player p;

typedef struct enemy{
    bool alive;
    SDL_Rect dim;
    int pathlength;
    int dir;
    int laserTimer;
    laser l[MAXLASERS];
}enemy;

enemy e[MAXENEMIES];

// Input actions
bool action_moveleft = false;
bool action_moveright = false;
bool action_moveup = false;
bool action_movedown = false;
bool action_fire = false;

// Generate a random number in a specific range
int randrange(int low, int high)
{
    int r = rand() % (high - low + 1) + low;
    return r;
}

bool init()
{
    if(SDL_Init(SDL_INIT_EVERYTHING) == -1) { return false; }
    
    screen = SDL_SetVideoMode(SCREEN_WIDTH,SCREEN_HEIGHT,SCREEN_BPP,SDL_SWSURFACE);
    
    if(screen == NULL) { return false; }
    
    if( TTF_Init() == -1 ) { return false; }
    
    if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 ) { return false; }
    
    SDL_WM_SetCaption("Space Shooter",NULL);
    
    return true;
}


SDL_Surface *load_image(char * filename)
{
    SDL_Surface *loadedImage = NULL;
    SDL_Surface *optimizedImage = NULL;
    
    loadedImage = IMG_Load(filename);
    
    if(loadedImage != NULL)
    {
        optimizedImage = SDL_DisplayFormatAlpha(loadedImage);
        SDL_FreeSurface(loadedImage);
    }
    
    return optimizedImage;
}

bool load_files()
{
    //Font
    font = TTF_OpenFont( "res/LCD_Solid.ttf", 20 );
    if( font == NULL ) { return false; }
    
    //Textures
    background = load_image("res/background.png");
    if(background == NULL) { return false; }
    
    sprite_player = load_image("res/player_ship.png");
    if(sprite_player == NULL) { return false; }
    
    sprite_laser = load_image("res/laser.png");
    if(sprite_laser == NULL) { return false; }
    
    sprite_laser_enemy = load_image("res/laser_enemy.png");
    if(sprite_laser_enemy == NULL) { return false; }
    
    sprite_enemy = load_image("res/enemy_ship.png");
    if(sprite_enemy == NULL) { return false; }
    
    //Sound
    music = Mix_LoadMUS("res/music.ogg");
    if(music == NULL) { return false; }
    
    snd_player_fire = Mix_LoadWAV("res/player_fire.wav");
    if(snd_player_fire == NULL) { return false; }
    
    snd_explosion = Mix_LoadWAV("res/explosion.wav");
    if(snd_explosion == NULL) { return false; }
    
    return true;    
}

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination )
{
    //Make a temporary rectangle to hold the offsets
    SDL_Rect offset;

    //Give the offsets to the rectangle
    offset.x = x;
    offset.y = y;
    
    //Blit the surface
    SDL_BlitSurface( source, NULL, destination, &offset );
}

void clean_up()
{
    SDL_FreeSurface(background);
    SDL_FreeSurface(sprite_player);
    SDL_FreeSurface(sprite_laser);
    SDL_FreeSurface(sprite_laser_enemy);
    SDL_FreeSurface(sprite_enemy);
    
    Mix_FreeMusic(music);
    Mix_FreeChunk(snd_player_fire);
    Mix_FreeChunk(snd_explosion);
    
    SDL_Quit();
}

void drawbackground()
{
    int scrollspeed = 10;
    
    if(background_y < 640)
    {
        background_y += scrollspeed;
    }
    else
        background_y = 0;
        
    apply_surface(0,background_y,background,screen);
    apply_surface(0,background_y-640,background,screen);

}

void drawplayer()
{
    if(p.alive == true)
        apply_surface(p.dim.x,p.dim.y,sprite_player,screen);
}

void drawlasers()
{
    int i,j;
    
    //player lasers
    for(i=0;i<MAXLASERS;i++)
    {
        if(p.l[i].alive == true)
        {
            apply_surface(p.l[i].dim.x,p.l[i].dim.y,sprite_laser,screen);
        }
    }
    
    // enemy lasers
    for(j=0;j<MAXLASERS;j++)
    {
        for(i=0;i<MAXLASERS;i++)
        {
            if(e[j].l[i].alive == true)
            {
                apply_surface(e[j].l[i].dim.x,e[j].l[i].dim.y,sprite_laser_enemy,screen);
            }
        }
    }
}

void drawenemies()
{
    int i;
    
    for(i=0;i<MAXENEMIES;i++)
    {
        if(e[i].alive == true)
        {
            apply_surface(e[i].dim.x,e[i].dim.y,sprite_enemy,screen);
        }
    }
}

void drawinfo()
{
    char score[64];
    
    sprintf(score,"Score: %d",p.score);
    text_score = TTF_RenderText_Solid( font, score, textColor );
    
    if(text_score == NULL){ return; }
    apply_surface(5, 5+SCREEN_BOTTOM, text_score, screen);
    SDL_FreeSurface(text_score);
    
    char health[64];
    
    sprintf(health,"Health: %d",p.health);
    text_health = TTF_RenderText_Solid( font, health, textColor );
    
    if(text_health == NULL){ return; }
    apply_surface(SCREEN_WIDTH-200, 5+SCREEN_BOTTOM, text_health, screen);
    SDL_FreeSurface(text_health);
}

void drawgameover()
{
    text_gameover = TTF_RenderText_Solid( font, "Game Over", textColor );
    
    if(text_gameover == NULL){ return; }
    apply_surface(250, 200, text_gameover, screen);
    SDL_FreeSurface(text_gameover);
}

void spawnplayer()
{
    p.alive = true;
    p.score = 0;
    p.health = 5;
    p.dim.w = 64;
    p.dim.h = 64;
    p.dim.x = 295;
    p.dim.y = SCREEN_BOTTOM - p.dim.h;
}

void moveplayer()
{
    int movespeed = 8;
    
    if(action_moveleft == true)
    {
        p.dim.x -= movespeed;
        if(p.dim.x < 0)
            p.dim.x = 0;
    }
    else if(action_moveright == true)
    {
        p.dim.x += movespeed;
        if((p.dim.x + p.dim.w) > SCREEN_WIDTH)
            p.dim.x = SCREEN_WIDTH - p.dim.w;
    }
    if(action_moveup == true)
    {
        p.dim.y -= movespeed/2;
        if(p.dim.y < 0)
            p.dim.y = 0;
    }
    else if(action_movedown == true)
    {
        p.dim.y += movespeed/2;
        if((p.dim.y + p.dim.h) > SCREEN_BOTTOM)
            p.dim.y = SCREEN_BOTTOM - p.dim.h;
    }
}

void fire()
{
    int i;
    
    if(action_fire == true && p.laserTimer == 0)
    {
        for(i=0;i<MAXLASERS;i++)
        {
            p.l[i].dim.w = 8;
            p.l[i].dim.h = 16;
            
            if(p.l[i].alive != true)
            {
                p.l[i].alive = true;
                p.l[i].dim.x = p.dim.x + (p.dim.w/2);
                p.l[i].dim.y = p.dim.y - p.l[i].dim.h;
                p.laserTimer = 15;
                Mix_VolumeChunk(snd_player_fire, 32);
                Mix_PlayChannel( -1, snd_player_fire, 0 );
                break;
            }
        }
    }
    
    if(p.laserTimer > 0)
        p.laserTimer--;
}

void enemyfire()
{
    int i,j;
    
    for(j=0;j<MAXENEMIES;j++)
    {
        if(e[j].laserTimer == 0 && e[j].alive && (e[j].dim.y + e[j].dim.h) >= 0)
        {
            for(i=0;i<MAXLASERS;i++)
            {
                e[j].l[i].dim.w = 8;
                e[j].l[i].dim.h = 16;
                
                if(e[j].l[i].alive != true)
                {
                    e[j].l[i].alive = true;
                    e[j].l[i].dim.x = e[j].dim.x + (e[j].dim.w/2);
                    e[j].l[i].dim.y = e[j].dim.y + e[j].l[i].dim.h;
                    e[j].laserTimer = randrange(100,250);
                    Mix_VolumeChunk(snd_player_fire, 8);
                    Mix_PlayChannel( -1, snd_player_fire, 0 );
                    break;
                }
            }
        }
        
        if(e[j].laserTimer > 0)
            e[j].laserTimer--;
    }
}

void movelasers()
{
    int movespeed = 10;
    int i,j;
    
    for(i=0;i<MAXLASERS;i++)
    {
        if(p.l[i].alive == true)
        {
            p.l[i].dim.y -= movespeed;
        }
        if(p.l[i].dim.y < 0)
            p.l[i].alive = false;
    }
    
    for(j=0;j<MAXENEMIES;j++)
    {
        for(i=0;i<MAXLASERS;i++)
        {
            if(e[j].l[i].alive == true)
            {
                e[j].l[i].dim.y += movespeed/2;
            }
            if(e[j].l[i].dim.y > SCREEN_HEIGHT)
                e[j].l[i].alive = false;
        }
    }
}

void spawnenemies()
{
    int i;
    
    if(game_init == true)
    {
        for(i=0;i<MAXENEMIES;i++)
        {
            e[i].alive = false;
        }
    }

        for(i=0;i<MAXENEMIES;i++)
        {
            e[i].dim.w = 64;
            e[i].dim.h = 32;
            
            if(e[i].alive != true && enemyTimer == 0)
            {
                e[i].alive = true;
                e[i].pathlength = 0;
                e[i].laserTimer = 0;
                e[i].dir = randrange(0,1);
                e[i].dim.x = randrange(0,SCREEN_WIDTH - e[i].dim.w);
                e[i].dim.y = randrange(-500,-50);
                break;
            }
        }
       
}

void moveenemies()
{
    int movespeed = 2;
    
    int i;

    for(i=0;i<MAXENEMIES;i++)
    {
        if(e[i].alive == true)
        {
            if(e[i].pathlength == 0)
            {
                e[i].pathlength = randrange(10,SCREEN_WIDTH/2);
            }
            if(e[i].pathlength != 0)
            {
                if(e[i].dir == 0)
                {
                    if(e[i].dim.x + e[i].dim.w < SCREEN_WIDTH)
                    {
                        e[i].dim.x += movespeed;
                        e[i].pathlength--;
                    }
                    if(e[i].dim.x + e[i].dim.w >= SCREEN_WIDTH || e[i].pathlength == 0)
                    {
                        e[i].dir = 1;
                        e[i].pathlength = 0;
                    }
                }
                else if(e[i].dir == 1)
                {
                    if(e[i].dim.x > 0)
                    {
                        e[i].dim.x -= movespeed;
                        e[i].pathlength--;
                    }
                    if(e[i].dim.x <= 0 || e[i].pathlength == 0)
                    {
                        e[i].dir = 0;
                        e[i].pathlength = 0;
                    }
                }
            }
            
            e[i].dim.y += 1;
        }
        
        if(e[i].dim.y > SCREEN_BOTTOM+e[i].dim.h)
        {
            e[i].alive = false;
            e[i].dim.x = 0;
            e[i].dim.y = 0;
            if(game_over == false)
                p.score -= 50;
            if(p.score < 0)
                p.score = 0;
            break;
        }
    }
    if(enemyTimer > 0)
        enemyTimer--;
}

bool rect_col( SDL_Rect A, SDL_Rect B ) // Thanks to lazyfoo.net
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

void testcollisions()
{
    int i,j;
    
    // Check if player lasers hit enemies
    for(i=0;i<MAXLASERS;i++)
    {
        for(j=0;j<MAXENEMIES;j++)
        {
            if(p.alive && p.l[i].alive == true && e[j].alive == true && (e[j].dim.y + e[j].dim.h) >= 0)
            {
                if(rect_col(p.l[i].dim,e[j].dim) == true)
                {
                    e[j].alive = false;
                    p.l[i].alive = false;
                    enemyTimer = 30;
                    p.score += 10;
                    Mix_VolumeChunk(snd_explosion, 64);
                    Mix_PlayChannel( -1, snd_explosion, 0 );
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
            if(p.alive == true && e[j].l[i].alive == true)
            {
                if(rect_col(p.dim,e[j].l[i].dim) == true)
                {
                    e[j].l[i].alive = false;
                    p.health -= 1;
                    Mix_VolumeChunk(snd_explosion, 64);
                    Mix_PlayChannel( -1, snd_explosion, 0 );
                    break;
                }
            }
        }
    }
    
    //Check if enemies hit the player
    for(j=0;j<MAXENEMIES;j++)
    {
        if(e[j].alive == true && p.alive == true)
        {
            if(rect_col(e[j].dim,p.dim) == true)
            {
                p.health -= 2;
                Mix_VolumeChunk(snd_explosion, 64);
                Mix_PlayChannel( -1, snd_explosion, 0 );
                break;
            }
        }
    }
}

void musicplay()
{
    if(Mix_PlayMusic(music, -1) == -1)
        return;
    else
        Mix_VolumeMusic(128);
}

void destroylasers()
{
    int i,j;
    
    for(i=0;i<MAXLASERS;i++)
        p.l[i].alive = false;
    
    for(j=0;j<MAXENEMIES;j++)
        for(i=0;i<MAXLASERS;i++)
            e[j].l[i].alive = false;
}

void newgame()
{
    game_init = true;
    game_over = false;
    action_fire = false;
    action_moveleft = false;
    action_moveright = false;
    action_moveup = false;
    action_movedown = false;
    destroylasers();
    spawnplayer();
    spawnenemies();
    game_init = false;
}

int main(int argc, char* argv[])
{
    srand(time(0));
    
    bool quit = false;
    
    if(init() == false) { return 1; }
    if(load_files() == false) { return 1; }
    
    //Start a new game
    newgame();
    musicplay();
    
    while(quit == false)
    {
        startTimer = SDL_GetTicks();
        
        while(SDL_PollEvent(&event))
        {
            if( event.type == SDL_KEYDOWN )
            {
                if( event.key.keysym.sym == SDLK_ESCAPE )
                {
                    quit = true;
                }
                if(game_over == false)
                {
                    if(event.key.keysym.sym == SDLK_LEFT)
                    {
                        action_moveleft = true;
                    }
                    if(event.key.keysym.sym == SDLK_RIGHT)
                    {
                        action_moveright = true;
                    }
                    if(event.key.keysym.sym == SDLK_UP)
                    {
                        action_moveup = true;
                    }
                    if(event.key.keysym.sym == SDLK_DOWN)
                    {
                        action_movedown = true;
                    }
                    if(event.key.keysym.sym == SDLK_SPACE)
                    {
                        action_fire = true;
                    }
                }
                if(game_over == true)
                {
                    if(event.key.keysym.sym == SDLK_SPACE)
                    {
                        newgame();
                    }
                }
            }
            else if( event.type == SDL_KEYUP )
            {
                if(game_over == false)
                {
                    if(event.key.keysym.sym == SDLK_LEFT)
                    {
                        action_moveleft = false;
                    }
                    if(event.key.keysym.sym == SDLK_RIGHT)
                    {
                        action_moveright = false;
                    }
                    if(event.key.keysym.sym == SDLK_UP)
                    {
                        action_moveup = false;
                    }
                    if(event.key.keysym.sym == SDLK_DOWN)
                    {
                        action_movedown = false;
                    }
                    if(event.key.keysym.sym == SDLK_SPACE)
                    {
                        action_fire = false;
                    }
                }
            }
            if(event.type == SDL_QUIT)
            {
                quit = true;
            }
        }
        
        // Fill the screen with black
        SDL_FillRect(screen,NULL, 0x000000);
        
        // Draw background
        drawbackground();
        
        if(game_over == false)
        {
            // Move and draw the player
            moveplayer();
            drawplayer();
            
            // Draw fire and lasers
            fire();
        }
        
        // Spawn and draw enemies
        spawnenemies();
        moveenemies();
        enemyfire();
        drawenemies();
        
        movelasers();
        drawlasers();
        
        // Check if player is dead
        if(p.health <= 0)
        {
            p.health = 0;
            p.alive = false;
            game_over = true;
        }
        
        // Draw the gameover text when the game is over
        if(game_over == true)
        {
            drawgameover();
        }
        
        // Draw score and lives
        drawinfo();
        
        // Collision Detection
        testcollisions();
        
        //Update the screen
        if(SDL_Flip(screen) == -1) { return 1; }
            
        endTimer = SDL_GetTicks();
        deltaTimer = endTimer - startTimer;
        if ( deltaTimer < ( 1000 / FPS ))
        {
            SDL_Delay( ( 1000 / FPS ) - deltaTimer );
        }
    }
    
    clean_up();
    
    return 0;
}

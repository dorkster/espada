typedef enum { false = 0, true = 1 } bool;

//------------------------------
// Funtion declarations
//------------------------------
int sys_rand(int low, int high);
bool sys_collide();
bool sys_init();
bool sys_loadfiles();
void sys_cleanup();

SDL_Surface *image_load(char * filename, bool withalpha);
void image_apply( int x, int y, int alpha, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip );

void sound_playfx(Mix_Chunk* snd);
void sound_playmus();

void draw_frameadvance(int* frame,int totalframes);
void draw_background();
void draw_titlescreen();
void draw_info();
void draw_statustext();
void draw_player();
void draw_enemies();
void draw_lasers();
void draw_explosions();

void game_newgame();
void game_titlescreen();
void game_pause();
void game_testcollisions();
void game_playerspawn();
void game_playermove();
void game_playerfire();
void game_playerdamage(int d);
void game_playerinvulntick();
void game_enemyspawn();
void game_enemymove();
void game_enemyfire();
void game_lasersmove();
void game_lasersdestroy();
void game_explosionspawn(int x, int y);

//------------------------------
// Gameplay constants
//------------------------------
#define MAXLASERS 5
#define MAXENEMIES 4
#define MAXEXPLOSIONS 16

//------------------------------
// Screen dimensions
//------------------------------
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define SCREEN_BPP 32
#define SCREEN_BOTTOM SCREEN_HEIGHT-32
#define FPS 60

//------------------------------
// Timers
//------------------------------
int startTimer;
int endTimer;
int deltaTimer;
int enemyTimer;
int animationTimer;

//------------------------------
// Image surfaces
//------------------------------
SDL_Surface* screen = NULL;

SDL_Surface* background = NULL;
int background_y = 0;

SDL_Surface* title_graphic = NULL;
SDL_Surface* menu_cursor = NULL;

SDL_Surface* sprite_player = NULL;
SDL_Surface* sprite_health_full = NULL;
SDL_Surface* sprite_health_empty = NULL;
SDL_Surface* sprite_laser = NULL;
SDL_Surface* sprite_laser_enemy = NULL;
SDL_Surface* sprite_enemy = NULL;
SDL_Surface* sprite_explosion = NULL;

//------------------------------
// Text surfaces
//------------------------------
TTF_Font *font = NULL;
SDL_Color textColor = { 255, 255, 255 };
SDL_Surface* text_score = NULL;
SDL_Surface* text_health = NULL;
SDL_Surface* text_status = NULL;
SDL_Surface* text_titlescreen = NULL;

//------------------------------
// Sounds
//------------------------------
Mix_Music* music = NULL;
Mix_Chunk* snd_player_fire = NULL;
Mix_Chunk* snd_enemy_fire = NULL;
Mix_Chunk* snd_explosion = NULL;

bool sound_enabled = true;
int sound_fadetime = 500;
int sound_volfx;
int sound_volmus;
int sound_volmus_paused;

//------------------------------
// Menus
//------------------------------
int menu_selection = 0;
int menu_level = 0;
char* menu_main[2][3] = {{"Start","Options","Quit"},{"SFX: ","Music: ","Back"}};

//------------------------------
// Gameplay states
//------------------------------
SDL_Event event;

bool gamestate_init = true;
bool gamestate_title = true;
bool gamestate_over = true;
bool gamestate_pause = false;

bool action_moveleft = false;
bool action_moveright = false;
bool action_moveup = false;
bool action_movedown = false;
bool action_fire = false;

//------------------------------
// Game object structures
//------------------------------
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
    laser laz[MAXLASERS];
    bool invuln;
    int invulnTimer;
    int frame;
}player;

typedef struct enemy{
    bool alive;
    SDL_Rect dim;
    int pathlength;
    int dir;
    int laserTimer;
    laser laz[MAXLASERS];
    int frame;
}enemy;

typedef struct explosion{
    bool alive;
    SDL_Rect dim;
    int frame;
}explosion;

player obj_player;
enemy obj_enemy[MAXENEMIES];
explosion obj_explosion[MAXEXPLOSIONS];

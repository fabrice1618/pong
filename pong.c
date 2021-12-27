//Using libs SDL2, glibc
#include <SDL.h>	//SDL version 2.0
#include <SDL2/SDL_ttf.h>
extern DECLSPEC int SDLCALL TTF_Init(void);

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

// Game configuration
SDL_Color gameColor = {255,255,255,255};

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define SCREEN_FG 0xffffffff
#define SCREEN_BG 0x000000ff
#define SCORE_WIN 10
#define LOOP_TIME 20	// 50 frames par seconde

typedef struct ball_s {
	int dx, dy; /* movement vector */
} ball_move_t;

enum paddle_direction { PADDLE_DOWN, PADDLE_UP };

enum game_state { GAME_MENU, GAME_START, GAME_PLAY, GAME_OVER, GAME_QUIT };

// Program globals
SDL_Rect ball;
ball_move_t ball_move; 
SDL_Rect paddle[2];
int score[] = {0,0};
enum game_state state = GAME_MENU;

// Display configuration
SDL_Window* window = NULL;	
SDL_Renderer *renderer;
static SDL_Surface *screen;
SDL_Texture *screen_texture;
TTF_Font* fontBig;
TTF_Font* fontNormal;

//initialise starting position and sizes of game elemements
static void init_game() 
{
	ball.x = screen->w / 2;
	ball.y = screen->h / 2;
	ball.w = 10;
	ball.h = 10;
	ball_move.dy = 1;
	ball_move.dx = 1;
	
	paddle[0].x = 20;
	paddle[0].y = screen->h / 2 - 50 ;
	paddle[0].w = 10;
	paddle[0].h = 50;

	paddle[1].x = screen->w - 20 - 10;
	paddle[1].y = screen->h / 2 - 50;
	paddle[1].w = 10;
	paddle[1].h = 50;
}

// return the winner number
//return 0 if no one has reached a score of 10 yet
int check_score() 
{
	for(int i = 0; i < 2; i++) {
		if ( score[i] == SCORE_WIN ) {
			return(i+1);
		}
	}
	return 0;
}

static void move_ball() 
{
	/* Move the ball by its motion vector. */
	ball.x += ball_move.dx;
	ball.y += ball_move.dy;
	
	// Balle sortie laterale
	if (ball.x < 0) {		
		score[1] += 1;
		state  = GAME_START;
	}
	if (ball.x > screen->w - 10) { 
		score[0] += 1;
		state  = GAME_START;
	}

	if (state != GAME_START) {
		// Rebond sur partie haute ou basse
		if (ball.y < 0 || ball.y > screen->h - 10) {
			ball_move.dy = -ball_move.dy;
		}

		//check for collision with the paddle
		for (int i = 0; i < 2; i++) {

			//collision detected	
			if ( SDL_HasIntersection(&ball, &paddle[i]) ) {

				// Accelerer la balle			
				ball_move.dx +=  (ball_move.dx < 0) ? -1: 1;
				//change ball direction
				ball_move.dx = -ball_move.dx;
				
				//change ball angle based on where on the paddle it hit
				int hit_pos = (paddle[i].y + paddle[i].h) - ball.y;
				ball_move.dy = 4 - (int)(hit_pos / 7);
			}
		}
	}
}

static void move_paddle_ai() 
{
	int paddle_center = paddle[0].y + 25;
	int ball_speed;
	int paddle_goto;

	ball_speed = abs(ball_move.dy);
	ball_speed = (ball_speed==0) ? 5: ball_speed;
	
	if (ball_move.dx > 0) {

		//ball moving right	-> return to center position
		if (paddle_center < (screen->h/2) ) {
			paddle_goto = paddle[0].y + ball_speed;
		} else {
			paddle_goto = paddle[0].y - ball_speed;
		}
	
	} else {

		//ball moving left -> folllow ball
		if (ball.y > paddle_center) { 
			paddle_goto = paddle[0].y + ball_speed;
		} else { 
			paddle_goto = paddle[0].y - ball_speed;
		}
	}

	// Pour eviter le clignotement
	if ( abs(paddle_center-paddle_goto) > 5 ) {
		paddle[0].y = paddle_goto;
	}
}

static void move_paddle(int paddleNum, int paddleDirection) 
{
	if (paddleDirection == PADDLE_DOWN) {
		// if the down arrow is pressed move paddle down
		paddle[paddleNum].y += 5;		

		if(paddle[paddleNum].y >= screen->h - paddle[paddleNum].h) {
			paddle[paddleNum].y = screen->h - paddle[paddleNum].h;
		}
	} else if (paddleDirection == PADDLE_UP) {
		// if the up arrow is pressed move paddle up
		paddle[paddleNum].y -= 5;

		if(paddle[paddleNum].y <= 0) {
			paddle[paddleNum].y = 0;
		}
	}
}

void drawTextCenter( int posX, int posY, char* message, TTF_Font* police )
{
	SDL_Surface *text_surface;
	SDL_Rect src;
	SDL_Rect dest;
	int centerX, centerY;

	if (!(text_surface=TTF_RenderText_Blended( police, message, gameColor ) ) ) {
		printf("[TTF_RenderText_Blended] > %s", SDL_GetError());
	} else {
		src.x = 0;
		src.y = 0;
		src.w = text_surface->w;
		src.h = text_surface->h;

		centerX = posX - (src.w / 2);
		centerY = posY - (src.h / 2);

		dest.x = centerX;
		dest.y = centerY;
		dest.w = src.w;
		dest.h = src.h;

		SDL_BlitSurface( text_surface, &src, screen, &dest );
	}

	SDL_FreeSurface(text_surface);
}

static void draw_game_over() 
{
	char messageWinner[20] = "";
	int posX, posY;

	switch ( check_score() ) {
		case 1:			
			strcpy( messageWinner, "Machine wins");
			break;
		case 2:
			strcpy( messageWinner, "Player 2 wins");
			break;
	}

	posX = screen->w/2;
	posY = screen->h/2;

	drawTextCenter( posX, posY, messageWinner, fontBig );
	drawTextCenter( posX, posY+90, "Game over", fontNormal );
}

static void draw_menu() 
{
	int titleX, titleY;

	titleX = screen->w/2;
	titleY = screen->h/2;

	drawTextCenter( titleX, titleY, "Pong", fontBig );
	drawTextCenter( titleX, titleY+50, "<space> to play game", fontNormal );
	drawTextCenter( titleX, titleY+80, "<esc> to quit game", fontNormal );
}

static void draw_score()
{
	char str_score[] = "0";

	str_score[0] = '0' + score[0];
	drawTextCenter( (screen->w/2) - 55, 20, str_score, fontBig );

	str_score[0] = '0' + score[1];
	drawTextCenter( (screen->w/2) + 55, 20, str_score, fontBig );
}

static void draw_net() 
{
	SDL_Rect net;
	
	net.x = screen->w / 2;
	net.y = 0;
	net.w = 5;
	net.h = 15;

	for(int posY = 20; posY < screen->h-30; posY += 30) {
		net.y = posY;
		
		if (SDL_FillRect(screen, &net, SCREEN_FG) != 0) { 
			printf("fill rectangle failed in func draw_net()");
		}
	}
}

static void draw_ball() 
{
	if (SDL_FillRect(screen , &ball, SCREEN_FG) !=0) {
		printf("fill rectangle failed in func draw_ball()");
	}
}

static void draw_paddle() 
{
	for (int i = 0; i < 2; i++) {
		if (SDL_FillRect(screen, &paddle[i], SCREEN_FG) !=0){
			printf("fill rectangle failed in func draw_paddle()");
		}
	}
}

int init( bool isFullscreen ) 
{
	Uint32 windowFlags = 0;
	int width, height;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return 1;
	} 
	
	//Create window	and renderer
	windowFlags |= isFullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP: 0;
	SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, windowFlags, &window, &renderer);
	if (window == NULL) { 
		printf("SDL_CreateWindowAndRenderer() SDL_Error: %s\n", SDL_GetError());
		return 1;
	}
	SDL_GetWindowSize(window, &width, &height);

	//create the screen surface where all the elemnts will be drawn onto (ball, paddles, net etc)
	screen = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_RGBA32);
	if (screen == NULL) {
		printf("Could not create the screen surfce! SDL_Error: %s\n", SDL_GetError());
		return 1;
	}

	//create the screen texture to render the screen surface to the actual display
	screen_texture = SDL_CreateTextureFromSurface(renderer, screen);
	if (screen_texture == NULL) {
		printf("Could not create the screen_texture! SDL_Error: %s\n", SDL_GetError());
		return 1;
	}

	if (TTF_Init() < 0) {
    	printf( "Error ttf_init: %s\n", SDL_GetError() );
		return 1;
	}

	fontBig = TTF_OpenFont("ArcadeFuture.otf", 72);
	if(!fontBig) {
		printf("TTF_OpenFont: %s\n", SDL_GetError());
		return 1;
	}

	fontNormal = TTF_OpenFont("ArcadeFuture.otf", 24);
	if(!fontNormal) {
		printf("TTF_OpenFont: %s\n", SDL_GetError());
		return 1;
	}

	return 0;
}

int main (int argc, char *args[]) 
{
	bool isFullScreen = false;
	int sleep_time = 0;
	Uint32 next_game_tick = SDL_GetTicks();
	SDL_Event evenements; 	

	// Recherche parametre -f pour fullscreen
	for (int i = 0; i < argc; i++) {
		if ( strcmp(args[i], "-f") == 0 ) {
			isFullScreen = true;
		}
	}

	//SDL Window setup
	if (init( isFullScreen ) == 1) {
		return EXIT_FAILURE;
	}
	
	//render loop
	while(state != GAME_QUIT) {

		// Fermeture de la fenetre
		while ( (state != GAME_QUIT) && SDL_PollEvent(&evenements) ) {
			if ((evenements.type==SDL_WINDOWEVENT) && (evenements.window.event == SDL_WINDOWEVENT_CLOSE))
				state = GAME_QUIT;
		}

		// Traiter les événements clavier
		const Uint8 *keystate = SDL_GetKeyboardState(NULL);
		
		if (keystate[SDL_SCANCODE_ESCAPE]) {
			state = GAME_QUIT;
		}
		switch(state) {
			case GAME_MENU:
				if (keystate[SDL_SCANCODE_SPACE])
					state = GAME_START;
				break;
			case GAME_PLAY:
				if (keystate[SDL_SCANCODE_DOWN]) {
					move_paddle(1, PADDLE_DOWN);
				}
				if (keystate[SDL_SCANCODE_UP]) {
					move_paddle(1, PADDLE_UP);
				}
				if (keystate[SDL_SCANCODE_A]) {
					move_paddle(1, PADDLE_DOWN);
				}
				if (keystate[SDL_SCANCODE_Q]) {
					move_paddle(1, PADDLE_UP);
				}
				break;
			case GAME_OVER:
				if (keystate[SDL_SCANCODE_SPACE]) {
					state = GAME_START;
				}
				break;
		}

		//draw background
		SDL_RenderClear(renderer);
		SDL_FillRect(screen, NULL, SCREEN_BG);

		switch(state) {
			case GAME_MENU:
				draw_menu();
				break;

			case GAME_START:
				init_game();
				state = GAME_PLAY;
				break;

			case GAME_PLAY:
				//check score
				if (check_score() != 0) {
					state = GAME_OVER;
				} else {
					//paddle ai movement
					move_paddle_ai();

					//* Move the balls for the next frame. 
					move_ball();					
					if ( state != GAME_START ) {
						draw_net();
						draw_paddle();
						draw_ball();
						draw_score();
					}
				}
				break;

			case GAME_OVER:
				draw_game_over();
				//reset scores
				score[0] = 0;
				score[1] = 0;
				break;

			case GAME_QUIT:
				break;
		}

		//draw to the display
		SDL_UpdateTexture(screen_texture, NULL, screen->pixels, screen->w * sizeof (Uint32));
		SDL_RenderCopy(renderer, screen_texture, NULL, NULL);
		SDL_RenderPresent(renderer);

		//time it takes to render  frame in milliseconds
		next_game_tick += LOOP_TIME;
		sleep_time = next_game_tick - SDL_GetTicks();
		if ( sleep_time > 0 ) {
			SDL_Delay(sleep_time);
		}

	}

	SDL_FreeSurface(screen);
	//free renderer and all textures used with it
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	TTF_CloseFont(fontBig);
	TTF_CloseFont(fontNormal);

	//Quit SDL subsystems 
	TTF_Quit();	
	SDL_Quit(); 
	 
	return EXIT_SUCCESS;
}


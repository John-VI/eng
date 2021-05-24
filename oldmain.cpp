#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_gamecontroller.h>
#include <stdbool.h>
#include <SDL2/SDL_ttf.h>
//#include <string>
#include <cmath>
//#include <vector>
//#define SPDLOG_FMT_EXTERNAL
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#define SCRWIDTH 640
#define SCRHEIGH 480

struct pad {
  bool buttons[SDL_CONTROLLER_BUTTON_MAX];
  int axis[SDL_CONTROLLER_AXIS_MAX];
};

struct gamepads {
  unsigned int padc;
  SDL_GameController **pads;
};

auto logger = spdlog::basic_logger_mt("basic_logger", "log.txt");

SDL_Window *initsdl() {
  SDL_Window *window = NULL;
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    logger->critical("SDL failed: {}\n", SDL_GetError());
  } else {
    window = SDL_CreateWindow("Ayy", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			      SCRWIDTH, SCRHEIGH, SDL_WINDOW_SHOWN);
    if (window == NULL) {
      logger->critical("Window failed: {}\n", SDL_GetError());
    }
  }
  return window;
}

SDL_Renderer *initrenderer(SDL_Window *win) {
  SDL_Renderer *rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
  if (rend == NULL)
    logger->critical("Could not create renderer! {}", SDL_GetError());
  else {
    SDL_SetRenderDrawColor(rend, 0xFF, 0x00, 0x00, 0xFF);

    int imgflags = IMG_INIT_PNG;
    if (!(IMG_Init(imgflags) & imgflags)) {
      logger->critical("SDL_image failed! {}", IMG_GetError());
    }
  }
  return rend;
}

struct gamepads initpads() {
  // This one is basically stolen from
  // https://sausage-factory.games/dev-blog/easier-game-controller-input-in-sdl/
  if (SDL_WasInit(SDL_INIT_GAMECONTROLLER) != 1)
    SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER);
  int sticks = SDL_NumJoysticks();
  unsigned int padc = 0;
  SDL_GameController **pads = NULL;
  struct gamepads padstruct;

  for (unsigned int i=0;i<sticks;i++)
    if (SDL_IsGameController(i))
      padc++;

  if (padc) {
    // Reallocing for dead controller may save RAM, but I decided the CPU cycles
    // Were worth more.
    pads = (SDL_GameController **) malloc(padc * sizeof(SDL_GameController *));
    for (unsigned int i=0;i<padc;i++) {
      SDL_GameController *pad = SDL_GameControllerOpen(i);
      if (SDL_GameControllerGetAttached(pad) == 1) {
	*(pads + (i * sizeof(SDL_GameController *))) = pad;
      } else {
	logger->warn("Dead controller: {}", SDL_GetError());
	*(pads + (i * sizeof(SDL_GameController *))) = NULL;
      }
    }
  }
  padstruct.padc = padc;
  padstruct.pads = pads;
  return padstruct;
}

SDL_Surface *loadmedia(char filename[]) {
  SDL_Surface *img = NULL;
  if ((img = IMG_Load(filename)) == NULL)
    logger->warn("Failed to load media {}: {}", filename, SDL_GetError());
  return img;
}

SDL_Texture *loadtex(SDL_Renderer *ren, char filename[]) {
  SDL_Texture *tex = NULL;
  SDL_Surface *surf = loadmedia(filename);

  tex = SDL_CreateTextureFromSurface(ren, surf);
  if (tex == NULL) 
    logger->warn("Couldn't load texture: {}", SDL_GetError());

  SDL_FreeSurface(surf);
  return tex;
}

SDL_Texture *texttexture(SDL_Renderer *ren, TTF_Font *font, char *str,
			 SDL_Color color) {
  SDL_Surface *surf = TTF_RenderText_Solid(font, str, color);
  return SDL_CreateTextureFromSurface(ren, surf);
}

int main(int argc, char *argv[]) {
  logger->set_level(spdlog::level::debug);
  logger->flush_on(spdlog::level::debug);
  logger->info("Program starting...");
  
  SDL_Surface *screen = NULL;
  SDL_Window *win = NULL;
  SDL_Renderer *ren = NULL;
  SDL_Texture *img = NULL;
  int quit = 0;
  SDL_Event e;
  TTF_Font *font = NULL;
  SDL_Color white = {255, 255, 255};
  SDL_Rect msgrect = {0, 0, 0, 0};
  int w, h;
  
  struct gamepads pads;
  if ((win = initsdl()) != NULL) {
    if ((ren = initrenderer(win)) != NULL) {
      //if (TTF_Init()) {
      if(TTF_Init() != -1) {
	pads = initpads();
	logger->debug("Number of found controllers: {}", pads.padc);
	for (int i=0;i<pads.padc;i++) {
	  if (*(pads.pads + (i * sizeof(SDL_GameController *))))
	    logger->debug("{}",
			  SDL_GameControllerName(*(pads.pads + (i * sizeof(SDL_GameController *)))));
	  else
	    logger->debug("NULL");
	}
	//screen = SDL_GetWindowSurface(win);
	font = TTF_OpenFont("VGA9.ttf", 16);
	if (font == NULL)
	  logger->warn("Couldn't load font: {}", SDL_GetError());
	img = loadtex(ren, "Hey.jpg");
	SDL_SetTextureColorMod(img, 60, 60, 60);
	SDL_Texture *text = texttexture(ren, font, "Welcome to sticktest 0!", white);
	SDL_QueryTexture(text, NULL, NULL, &w, &h);
	msgrect.w = w;
	msgrect.h = h;
	while (quit != 1) {
	  while (SDL_PollEvent(&e) != 0) {
	    if (e.type == SDL_QUIT)
	      quit = 1;
	  }
	  // SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0xFF, 0, 0));
	  // SDL_BlitSurface(img, NULL, screen, NULL);
	  // SDL_UpdateWindowSurface(win);
	  SDL_RenderClear(ren);
	  SDL_RenderCopy(ren, img, NULL, NULL);
	  SDL_RenderCopy(ren, text, NULL, &msgrect);
	  SDL_RenderPresent(ren);
	}
      } else {
	logger->critical("Failed to init TTF! {}", TTF_GetError());
	printf("%s\n", TTF_GetError());
	return 1;
      }
    } else {
      return 1;
    }
  } else {
    return 1;
  }
  for (quit=0;quit<pads.padc;quit++)
    SDL_GameControllerClose(*(pads.pads + (quit * sizeof(SDL_GameController *))));
  free(pads.pads);
  SDL_DestroyTexture(img);
  img = NULL;
  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(win);
  IMG_Quit();
  SDL_Quit();
  return 0;
}

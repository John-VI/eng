#include <stdio.h>
#include <stdbool.h>
#include <string>
#include <cmath>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_gamecontroller.h>
#include <SDL2/SDL_ttf.h>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

#include "eng.h"

#define SCRWIDTH 640
#define SCRHEIGH 480

auto logger = spdlog::basic_logger_mt("main_logger", "animationtestlog.txt");

void initlogger() {
  logger->set_level(spdlog::level::debug);
  logger->flush_on(spdlog::level::debug);
  logger->info("Program starting...");
}

int main(int argc, char *argv[]) {
  initlogger();

  int quit = 0;
  SDL_Event e;
  // TTF_Font *font = NULL;
  SDL_Color white = { 255, 255, 255 };
  renwindow *win = NULL;
  animationsheet *spritesheet = NULL;
  font *VGA = NULL;
  // ttftext *statustext = NULL;
  std::vector<struct anirow *> *data = new std::vector<struct anirow *>;
  std::vector<struct anirow *> *fdat = new std::vector<struct anirow *>;
  data->push_back(new struct anirow(33, 33, 6));
  data->push_back(new struct anirow(91, 97, 4));
  fdat->push_back(new struct anirow(9, 16, 96));

  if (!initsdl()) {
    win = new renwindow("Animation Test", SCRWIDTH, SCRHEIGH, 0, 0, 0);
    spritesheet = new animationsheet(win->getren(), "Spritesheet.png", data);
    // font = TTF_OpenFont("VGA9.ttf", 16);
    // if (font == NULL)
    //   logger->warn("Couldn't load font: {}", SDL_GetError());
    // statustext = new ttftext(win->getren(), font, &white);
    VGA = new font(win->getren(), "font.gif", fdat);
    
    spritesheet->setspeed(1000);
    spritesheet->setset(0);

    Uint64 minimumfps = 1000/6;
    Uint64 lastfticks = SDL_GetTicks();
    
    while (quit != 1) {
      while (SDL_PollEvent(&e) != 0) {
	if (e.type == SDL_QUIT)
	  quit = 1;
      }

      Uint64 now = SDL_GetTicks();
      if (lastfticks < now) {
	Uint64 tdelta = now - lastfticks;
	if (tdelta > minimumfps)
	  tdelta = minimumfps;
	
	const Uint8 *keystate = SDL_GetKeyboardState(NULL);
	char motion = 0;
	if (keystate[SDL_SCANCODE_UP])
	  motion |= 0b10000000;
	if (keystate[SDL_SCANCODE_DOWN])
	  motion |= 0b01000000;

	win->renclear();
	spritesheet->render(0, 0);
	anirow *cani = spritesheet->currentdata();
	SDL_Rect destquad = { (SCRWIDTH - cani->width * 2) / 2,
			      200, cani->width * 2, cani->height * 2 };

	//logger->debug("0 {}", TTF_GetError());
	VGA->rendertext(100, 100,
			("frame " + std::to_string(spritesheet->cframe()) +
			 " animation " + std::to_string(spritesheet->cset()) +
			 " playing " + std::to_string(spritesheet->animated()) +
			 " speed " + std::to_string(spritesheet->getspeed())).c_str());
	VGA->rendertext(00, 300, "Poopsie F\x7f");
	//logger->debug("1 {}", TTF_GetError());
	spritesheet->renderframe(&destquad);
	// statustext->render((SCRWIDTH - statustext->getwidth()) / 2, 200 + cani->height * 2 + 5);
	lastfticks = now;
	win->renupdate();
      } else
	SDL_Delay(1);
      
      //logger->debug("{} {}", (SCRWIDTH - statustext->getwidth()) / 2, 200 + cani->height * 2 + 5);
    }
  }
  // TTF_CloseFont(font);
  delete win;
  delete spritesheet;
  delete VGA;
  // delete statustext;
  return 0;
}

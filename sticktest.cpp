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

enum KeyPresses { KEYPESSDEFAULT,
		  KEYPRESSUP,
		  KEYPRESSDOWN,
		  KEYPRESSLEFT,
		  KEYPRESSRIGHT,
		  KEYPRESSTOTAL
};

auto logger = spdlog::basic_logger_mt("basic_logger", "log.txt");

void initlogger() {
  logger->set_level(spdlog::level::debug);
  logger->flush_on(spdlog::level::debug);
  logger->info("Program starting...");
}

int main(int argc, char *argv[]) {
  initlogger();

  int quit = 0;
  SDL_Event e;
  TTF_Font *font = NULL;
  SDL_Color white = { 255, 255, 255 };
  SDL_Color orange = { 0xFD, 0x6A, 0x02 };
  Renwindow *win = NULL;
  Texture *texter = NULL;
  Texture *frog = NULL;
  Texture *unselect = NULL;
  Texture *yeselect = NULL;
  struct menuopts *cmenu = NULL;
  std::vector<SDL_GameController *> *pads = NULL;
  char motion = 0;
  char oldmot = 0;
  Uint8 r = 60;
  Uint8 g = 60;
  Uint8 b = 60;
  //std::vector<Texture *> textlist;
  
  if (!initsdl()) {
    pads = initpads();
    logger->debug("Number of found controllers: {}", pads->size());
    if (pads->size())
      for (int i=0;i<pads->size();i++)
	logger->debug("{}", SDL_GameControllerName(pads->at(i)));
    
    win = new Renwindow("Ayy", SCRWIDTH, SCRHEIGH);
    frog = new Img(win->getren(), "Hey.jpg");
    
    font = TTF_OpenFont("VGA9.ttf", 16);
    if (font == NULL)
      logger->warn("Couldn't load font: {}", SDL_GetError());
    texter = new Text(win->getren(), font, &white, "Welcome to sticktest 0!");
    unselect = new Text(win->getren(), font, &white, "[ ]");
    yeselect = new Text(win->getren(), font, &orange, "[*]");

    cmenu = new struct menuopts;
    cmenu->options = new std::vector<menuopt *>;
    for (int i=0;i<pads->size();i++) {
      // logger->debug("entering loop");
      struct menuopt *opt = new struct menuopt;
      opt->texture = new Text(win->getren(), font, &white,
				 SDL_GameControllerName(pads->at(i)));
      cmenu->options->push_back(opt);
    }
    struct menuopt *opt = new struct menuopt;
    opt->texture = new Text(win->getren(), font, &white, "DUMMY");
			       
    cmenu->options->push_back(opt);
    cmenu->options->at(0)->selected = true;
    cmenu->options->at(0)->chosen = true;
    cmenu->selected = 0;
    cmenu->chosen = 0;
    
    // logger->debug("{}", texter->getheight() + 2);
    
    while (quit != 1) {
      while (SDL_PollEvent(&e) != 0) {
	if (e.type == SDL_QUIT)
	  quit = 1;
	}
      
      motion = 0;
      const Uint8 *keystate = SDL_GetKeyboardState(NULL);
      if (keystate[SDL_SCANCODE_UP])
	motion |= 0b10000000;
      if (keystate[SDL_SCANCODE_DOWN])
	motion |= 0b01000000;
      if (keystate[SDL_SCANCODE_LEFT])
	motion |= 0b0010000;
      if (keystate[SDL_SCANCODE_RIGHT])
	motion |= 0b0001000;
      if (keystate[SDL_SCANCODE_Q])
	r += 1;
      if (keystate[SDL_SCANCODE_A])
	r -= 1;
      if (keystate[SDL_SCANCODE_W])
	g += 1;
      if (keystate[SDL_SCANCODE_S])
	g -= 1;
      if (keystate[SDL_SCANCODE_E])
	b += 1;
      if (keystate[SDL_SCANCODE_D])
	b -= 1;
      if (keystate[SDL_SCANCODE_ESCAPE])
	quit = 1;
      frog->setcolor(r, g, b);
      logger->debug("Colors: {}", r);
      
      win->renclear();
      frog->render(0, 0);
      texter->render(0, 0);
      
      int hoff = texter->getheight();
      if ((motion & 0b11000000) == 0b10000000 && motion != oldmot && cmenu->selected > 0)
	cmenu->selected--;
      else if ((motion & 0b11000000) == 0b01000000 && motion != oldmot &&
	       cmenu->selected < cmenu->options->size() - 1)
	cmenu->selected++;
      oldmot = motion;
      // logger->debug("{}", cmenu->selected);
      for (int i=0;i<cmenu->options->size();i++) { 
	// logger->debug("i: {}", i);
	cmenu->options->at(i)->texture->render(0, hoff);
	if (cmenu->selected == i)
	  yeselect->render(cmenu->options->at(i)->texture->getwidth(), hoff);
	else
	  unselect->render(cmenu->options->at(i)->texture->getwidth(), hoff);
	hoff += cmenu->options->at(i)->texture->getheight();
      }
      win->renupdate();
    }
  }
  if (pads->size()) {
    for (int i=0;i<pads->size();i++)
      SDL_GameControllerClose(pads->at(i));
    // for (int i=0;i<textlist.size();i++)
    //   delete textlist[i];
  }
  for (int i=0;i<cmenu->options->size();i++) {
    delete cmenu->options->at(i)->texture;
    delete cmenu->options->at(i);
  }
  delete cmenu->options;
  delete cmenu;
  delete unselect;
  delete yeselect;
  delete pads;
  delete texter;
  delete frog;
  delete win;
  TTF_CloseFont(font);
  closesdl();
  return 0;
}

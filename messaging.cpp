#include "messaging.h"

#include <SDL2/SDL.h>

#include "eng.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

template <class T>
void pushstack(T *stack[], T *newentry, int size) {
  T *temp1;
  T *temp2 = newentry;
  for (int i=0;i<size;i++) {
    temp1 = stack[i];
    // logger->debug("1");
    stack[i] = temp2;
    // logger->debug("2");
    temp2 = temp1;
    // logger->debug("3");
  }
  delete temp1;
}

template <class T>
void invert(T *sequence, unsigned int size) {
  T temp;
  unsigned int i = 0;
  size--;
  while (i < size) {
    // logger->debug("size: {}, i: {}", size, i);
    temp = sequence[i];
    sequence[i] = sequence[size];
    sequence[size] = temp;
    i++;
    size--;
  }
}

// message::~message() {
//   delete msgtext;
// }

messager::messager(SDL_Renderer *renderer, std::shared_ptr<spdlog::logger> log, unsigned int size, 
	 font *fontface, SDL_Color *colorpalette, MSGLEVEL mesglevel) {
  ren = renderer;
  logger = log;
  queue = new struct message*[queuesize];
  
  for (int i = 0; i < size; i++) {
    queue[i] = new struct message;
    queue[i]->msgtext = NULL;
    queue[i]->level = DDEBUG;
  }
  
  queuesize = size;
  face = fontface;
  palette = colorpalette;
  index[DDEBUG] = DGRAY;
  index[DINFO] = LGRAY;
  index[DWARN] = YELLOW;
  index[DERR] = ORANGE;
  index[DCRITICAL] = RED;
  index[DFATAL] = DRED;
  index[MESSAGE] = WHITE;
  index[WARN] = YELLOW;
  index[DANGER] = ORANGE;
  index[CRITICAL] = RED;
  index[FATAL] = DRED;
  level = mesglevel;
}

void messager::send(const char *message, MSGLEVEL level, COLOR altcolor) {
  struct message *newmessage = new struct message;
  newmessage->msgtext = message;
  newmessage->level = level;
  newmessage->altcolor = altcolor;
  pushstack<struct message>(queue, newmessage, queuesize);
}

void messager::rendercappedmessages(unsigned int from, SDL_Rect *box) {
  int maxlines = box->h / face->getspriteheight(0);
  int maxwidth = box->w / face->getspritewidth(0);
  SDL_Color *col = NULL;

  for (int i = 0; i < maxlines && i < queuesize; i++) {
    // if (queue[i]->altcolor == DEFAULT)
    //   col = &palette[index[queue[i]->level]];
    // else
    //   col = &palette[queue[i]->altcolor];
    // Right now I don't feel like setting up a palette so, default color
    
    face->rendercappedtext(box->x, box->y + box->h - face->getspriteheight(0) - i *
			   face->getspriteheight(0), maxwidth,
			   queue[i]->msgtext);
  }
}

messager::~messager() {
  for (int i = 0; i < queuesize; i++)
    delete queue[i];
  delete[] queue;
}

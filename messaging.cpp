#include "messaging.h"

#include <SDL2/SDL.h>

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

messager::messager(std::shared_ptr<spdlog::logger> log, uint16_t flag, unsigned int size) {
  flags = flag;
  logger = log;
  queue = new std::string[size]();
  queuesize = size;
}

#ifndef SPRITE_HPP
#define SPRITE_HPP

#include "SDL/SDL.h"

class Sprite {
public:
  int x, y, w, h;
  int speed ;
  SDL_Surface* image;
  Sprite(int x_pos, 
         int y_pos, 
         int width, 
         int height, 
         int speed_s);
} ;

#endif


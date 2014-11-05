#include "sprite.hpp"

Sprite::Sprite(int x_pos, 
               int y_pos, 
               int width, 
               int height, 
               int speed_s)
{
  x = x_pos;
  y = y_pos;
  w = width;
  h = height;
  speed = speed_s;
  image = NULL;
}


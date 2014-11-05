#ifndef GAME_HPP
#define GAME_HPP

#include <list>
#include <string>
#include "SDL/SDL.h"
#include "sprite.hpp"

class Game {
public:
  Game();
  ~Game();
  void start();
private:
  void stop() ;
  void draw();
  void fillRect( SDL_Surface* surface, SDL_Rect* rc, int r, int g, int b );
  void fpsChanged( int fps );
  void onQuit();
  void onKeyDown( SDL_Event* event );
  void onKeyUp( SDL_Event* event );
  void run();
  void update();
  void updateEnemies();
  void updateBackground();
  void updateBullets();
  bool collide(Sprite sprite);
  bool collide_sprites(Sprite obj_1, Sprite obj_2);
  SDL_Surface* load_image(std::string filename);
  Sprite createEnemy();
  Sprite createBullet();
  
  int keys[ SDLK_LAST ] ;
  int frameSkip ;
  int running ;
  int score;
  int enemyInterval;
  bool canShoot;
  SDL_Surface* display ;
  Sprite hero ;
  Sprite background;
  std::list<Sprite> enemyList;
  std::list<Sprite> bulletList;
};

#endif


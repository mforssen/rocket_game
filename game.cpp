#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <list>
#include <string>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "sprite.hpp"
#include "game.hpp"
#include "game_enum.cpp"

Game::Game()
:frameSkip(0), running(0), score(0), enemyInterval(ENEMY_INTERVAL),
				  canShoot(true), display(NULL),
                  hero(0,0, HERO_WIDTH, HERO_HEIGHT, HERO_SPEED),
                  background(0,0, DISPLAY_WIDTH, DISPLAY_HEIGHT, 1) {
  for ( int i=0; i<SDLK_LAST; ++i ) {
    this->keys[ i ] = 0 ;
  }
}

Game::~Game() {
  this->stop();
}

void Game::start() {

  srand (time(NULL));

  int flags = SDL_HWSURFACE|SDL_DOUBLEBUF|SDL_ANYFORMAT;
  int imgFlags = IMG_INIT_JPG | IMG_INIT_PNG;
  int initted=IMG_Init(imgFlags);
  if( (initted & imgFlags) != imgFlags) {
    printf("could not init SDL_Image\n");
  }
  IMG_Init(imgFlags);

  /* initialize SDL */
  if ( SDL_Init(SDL_INIT_EVERYTHING) ) {
    return ;
  }

  /* set video surface */
  this->display = SDL_SetVideoMode( DISPLAY_WIDTH, DISPLAY_HEIGHT, 0, flags );

  if ( display == NULL ) {
    return ;
  }

  SDL_RWops* hero_rwops = SDL_RWFromFile("data/rocket_new_comet.png", "rb");
  hero.image = IMG_LoadPNG_RW(hero_rwops);

  SDL_RWops* background_rwops = SDL_RWFromFile("data/bg.png", "rb");
  background.image = IMG_LoadPNG_RW(background_rwops);

  /* Set caption */
  SDL_WM_SetCaption( "SDL Base C++", NULL );

  this->running = 1 ;
  run();
}

void Game::stop() {
  if ( display != NULL ) {
    SDL_FreeSurface( display );
  }

  if (hero.image != NULL) {
    SDL_FreeSurface(hero.image);
  }

  for (std::list<Sprite>::iterator i = enemyList.begin();
                                   i != enemyList.end(); ++i) {
    SDL_FreeSurface(i->image);
  }

  for (std::list<Sprite>::iterator i = bulletList.begin();
                                   i != bulletList.end(); ++i) {
    SDL_FreeSurface(i->image);
  }

  SDL_Quit() ;
}

void Game::draw() {

  if ( (background.x + DISPLAY_WIDTH) == 0 ) {
    background.x = 0;
  }

  SDL_Rect backgroundRect;

  backgroundRect.x = background.x;
  backgroundRect.y = background.y;
  backgroundRect.w = DISPLAY_WIDTH;
  backgroundRect.h = DISPLAY_HEIGHT;

  SDL_Rect backgroundRect_2;

  backgroundRect_2.x = background.x + DISPLAY_WIDTH;
  backgroundRect_2.y = backgroundRect.y;
  backgroundRect_2.w = DISPLAY_WIDTH;
  backgroundRect_2.h = DISPLAY_HEIGHT;

  SDL_BlitSurface(background.image, NULL, display, &backgroundRect);
  SDL_BlitSurface(background.image, NULL, display, &backgroundRect_2);

  //Bullets
  for (std::list<Sprite>::iterator i = bulletList.begin();
                                   i != bulletList.end(); ++i) {
    SDL_Rect bulletRect;
    Sprite bullet = *i;

    bulletRect.x = bullet.x;
    bulletRect.y = bullet.y;
    bulletRect.w = BULLET_WIDTH;
    bulletRect.h = BULLET_HEIGHT;

    SDL_BlitSurface(bullet.image, NULL, display, &bulletRect);
  }


  SDL_Rect heroRect ;

  /* Render hero */
  heroRect.x = hero.x ;
  heroRect.y = hero.y ;
  heroRect.w = HERO_WIDTH ;
  heroRect.h = HERO_HEIGHT ;
  //fillRect( display, &heroRect, 255, 0, 0 );

  SDL_BlitSurface(hero.image, NULL, display, &heroRect);

  for (std::list<Sprite>::iterator i = enemyList.begin();
                                   i != enemyList.end(); ++i) {
    SDL_Rect enemyRect;
    Sprite enemy = *i;

    enemyRect.x = enemy.x;
    enemyRect.y = enemy.y;
    enemyRect.w = ENEMY_WIDTH;
    enemyRect.h = ENEMY_HEIGHT;

    SDL_BlitSurface(enemy.image, NULL, display, &enemyRect);
  }

  SDL_Flip( display );
}

void Game::fillRect( SDL_Surface* surface, SDL_Rect* rc, int r, int g, int b ) {
  SDL_FillRect( surface, rc, SDL_MapRGB(surface->format, r, g, b) );
}

void Game::fpsChanged( int fps ) {
  char szFps[ 128 ] ;
  sprintf( szFps, "%s: %d FPS", "SDL Base C++ - Use Arrow Keys to Move", fps );
  SDL_WM_SetCaption( szFps, NULL );
}

void Game::onQuit() {
  running = 0 ;
}

void Game::run() {

  int past = SDL_GetTicks();
  int now = past, pastFps = past ;

  int past_enemy = SDL_GetTicks();
  int now_enemy = past_enemy;

  int past_shoot = SDL_GetTicks();
  int now_shoot = past_shoot;

  int fps = 0, framesSkipped = 0 ;
  SDL_Event event ;
  while ( running ) {
    int timeElapsed = 0 ;
    int timeElapsed_enemy = 0;
    int timeElapsed_shoot = 0;

    if (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:    onQuit();            break;
        case SDL_KEYDOWN: onKeyDown( &event ); break ;
        case SDL_KEYUP:   onKeyUp( &event );   break ;
        case SDL_MOUSEBUTTONDOWN: {
          break ;
        }
        case SDL_MOUSEBUTTONUP: {
          break ;
        }
        case SDL_MOUSEMOTION: {
          break ;
        }
      }
    }
    /* update/draw */
    timeElapsed = (now=SDL_GetTicks()) - past ;
    if ( timeElapsed >= UPDATE_INTERVAL  ) {
      past = now ;
      update();
      updateBullets();
      updateEnemies();
      updateBackground();
      if ( framesSkipped++ >= frameSkip ) {
        draw();
        ++fps ;
        framesSkipped = 0 ;
      }
    }

    timeElapsed_enemy = (now_enemy = SDL_GetTicks()) - past_enemy;
    if (timeElapsed_enemy >= ( (rand() % enemyInterval) + enemyInterval ) ) {
      past_enemy = now_enemy;
      Sprite enemy = createEnemy();
      enemyList.push_back(enemy);
    }

    timeElapsed_shoot = (now_shoot = SDL_GetTicks()) - past_shoot;
    if ( (timeElapsed_shoot >= BULLET_INTERVAL) || canShoot) {
      past_shoot = now_shoot;
      canShoot = true;
    }

    /* fps */
    if ( now - pastFps >= 1000 ) {
      pastFps = now ;
      fpsChanged( fps );
      fps = 0 ;
    }

    /* sleep? */
    SDL_Delay( 1 );
  }
}

bool Game::collide(Sprite sprite) {
  for (std::list<Sprite>::iterator i = enemyList.begin();
                                   i != enemyList.end(); ++i) {
    Sprite enemy = *i;

    //Test if we bumped into any enemy.
    if ( (sprite.x + sprite.w >= enemy.x) &&
         (sprite.x <= enemy.x + ENEMY_WIDTH) &&
         (sprite.y + sprite.h >= enemy.y) &&
         (sprite.y <= enemy.y + ENEMY_HEIGHT) ) {

       return true;
    }
  }
  return false;
}

void Game::updateEnemies() {
  for (std::list<Sprite>::iterator i = enemyList.begin();
                                   i != enemyList.end(); ++i) {

    i->x -= i->speed;

    if (i->x + ENEMY_WIDTH < 0) {
      SDL_FreeSurface(i->image);
      i = enemyList.erase(i);
    }
  }
}

void Game::updateBackground() {
  background.x -= 1;
}

bool Game::collide_sprites(Sprite obj_1, Sprite obj_2) {

  if ( (obj_1.x + obj_1.w >= obj_2.x) &&
    (obj_1.x <= obj_2.x + obj_2.w) &&
    (obj_1.y + obj_1.h >= obj_2.y) &&
    (obj_1.y <= obj_2.y + obj_2.h) ) {
      return true;
    }
  return false;
}

void Game::updateBullets() {

  for (std::list<Sprite>::iterator i = bulletList.begin();
                                   i != bulletList.end(); ++i) {
    i->x += i->speed;

    if ( i->x > DISPLAY_WIDTH ) {
      i = bulletList.erase(i);
    } else {
      for (std::list<Sprite>::iterator j = enemyList.begin();
                                   j != enemyList.end(); ++j) {
        if (collide_sprites(*i, *j)) {
          SDL_FreeSurface(i->image);
          SDL_FreeSurface(j->image);

          j = enemyList.erase(j);
          i = bulletList.erase(i);

          score++;

          //if ( (score % 20) == 0) {
            enemyInterval--;
            //printf("enemy interval = %d\n", enemyInterval);
         // }

          break;
        }
      }
    }
  }
}

void Game::update() {
  if ( collide(hero) ) {
    if ( (hero.x != 0) || (hero.y!=0) ) {
      printf("Score: %d\n", score);
    }

    score = 0;
    enemyInterval = ENEMY_INTERVAL;

    hero.x = 0;
    hero.y = 0;
  } else {
    // Left
    if ( keys[SDLK_LEFT] ) {
      if ( hero.x > 0 ) {
        hero.x -= HERO_SPEED ;
      } else {
        hero.x = 0;
      }

    // Right
    } else if ( keys[SDLK_RIGHT] ) {
      if ( hero.x < DISPLAY_WIDTH - HERO_WIDTH ) {
        hero.x += HERO_SPEED ;
      } else {
        hero.x = DISPLAY_WIDTH - HERO_WIDTH;
      }

    // Up
    } else if ( keys[SDLK_UP] ) {
      if ( hero.y > 0 ) {
        hero.y -= HERO_SPEED ;
      } else {
        hero.y = 0;
      }

    // Down
    } else if ( keys[SDLK_DOWN] ) {
      if ( hero.y < DISPLAY_HEIGHT - HERO_HEIGHT ) {
        hero.y += HERO_SPEED ;
      } else {
        hero.y = DISPLAY_HEIGHT - HERO_HEIGHT;
      }

    // Shoot
    }

    if (keys[SDLK_SPACE]) {

      if (canShoot) {
        Sprite bullet = createBullet();
        bulletList.push_back(bullet);
        canShoot = false;
      }
    }
  }
}

void Game::onKeyDown( SDL_Event* evt ) {
  keys[ evt->key.keysym.sym ] = 1 ;
}

void Game::onKeyUp( SDL_Event* evt ) {
  keys[ evt->key.keysym.sym ] = 0 ;
}

SDL_Surface* Game::load_image(std::string filename) {
  SDL_Surface* loadedImage = NULL;
  SDL_Surface* optimizedImage = NULL;
  loadedImage = IMG_Load( filename.c_str() );

  if (loadedImage != NULL) {
    optimizedImage = SDL_DisplayFormat( loadedImage );
    SDL_FreeSurface( loadedImage );
  }

  return optimizedImage;
}

Sprite Game::createEnemy() {

  int x = DISPLAY_WIDTH + 1;
  int y = rand() % (DISPLAY_HEIGHT - ENEMY_WIDTH);
  int speed = (rand() % 5) + 2;

  Sprite enemy = Sprite(x, y, ENEMY_WIDTH, ENEMY_HEIGHT, speed);

  // Set image for the enemy
  SDL_RWops* enemy_rwops = SDL_RWFromFile("data/asteroid_new.png", "rb");
  enemy.image = IMG_LoadPNG_RW(enemy_rwops);

  return enemy;
}

Sprite Game::createBullet() {
  int x = hero.x + (hero.w / 2);
  int y = hero.y + (hero.h / 2);

  Sprite bullet = Sprite(x, y,
          BULLET_WIDTH, BULLET_HEIGHT, BULLET_SPEED);

  SDL_RWops* bullet_rwops = SDL_RWFromFile("data/bullet.png", "rb");
  bullet.image = IMG_LoadPNG_RW(bullet_rwops);

  return bullet;
}




#include "game.h"
#include <iostream>

using namespace std;

cGame::cGame() : m_Fps(50), m_Eps(50), m_TextureManager() {
  // initialize SDL video
  if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER ) < 0 ) {
    cout << "Unable to init SDL: " << SDL_GetError() << endl;
    exit(1);
  }

  // SDL defaults stderr and stdout to text files, reset that:
  //freopen( "CON", "w", stdout );
  //freopen( "CON", "w", stderr );

  // make sure SDL cleans up before exit
  atexit(SDL_Quit);

  SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 0 );
  // create a new window
  const SDL_VideoInfo* info = SDL_GetVideoInfo();
  m_pScreen = SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, info->vfmt->BitsPerPixel, SDL_OPENGL);
  if ( !m_pScreen ) {
    cout << "Unable to set video: " << SDL_GetError() << endl;
    exit(1);
  }

  //m_Fps = new cEps(50);
  //m_Eps = new cEps(50);

  //m_TextureManager = new cTextureManager();

  // opengl init:
  glEnable(GL_TEXTURE_2D);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);   // This Will Clear The Background Color To Black
  //glClearDepth(1.0);        // Enables Clearing Of The Depth Buffer
  glDepthFunc(GL_LESS);       // The Type Of Depth Test To Do
  glDisable(GL_DEPTH_TEST); // disable depth test to enable blend below
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glShadeModel(GL_SMOOTH);      // Enables Smooth Color Shading
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();       // Reset The Projection Matrix
  glOrtho(0.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 0.0f, 0.0f, 1.0f);
  glMatrixMode(GL_MODELVIEW);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

cGame::~cGame() {
  //destructor
  SDL_Quit();
}

void cGame::Physicsloop() {
  int loops = 0;
  while( SDL_GetTicks() >= m_iNextGameTick && loops < MAX_FRAMESKIP) {
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
      // check for messages
      switch (event.type)
      {
      case SDL_QUIT:
        m_bGameIsRunning = false;
        break;
      case SDL_KEYDOWN:
        m_bKeysHeld[event.key.keysym.sym] = true;
        KeyDown(event.key.keysym.sym);
        break;
      case SDL_KEYUP:
        m_bKeysHeld[event.key.keysym.sym] = false;
        KeyUp(event.key.keysym.sym);
        break;
      case SDL_MOUSEMOTION:
        //std::cout << (int)event.button.button << std::endl;
        MouseMoved(
          event.motion.x,
          event.motion.y,
          event.motion.xrel,
          event.motion.yrel);
        break;
      case SDL_MOUSEBUTTONUP:
        //std::cout << (int)event.button.button << std::endl;
        MouseButtonUp(
          event.button.button,
          event.motion.x,
          event.motion.y,
          event.motion.xrel,
          event.motion.yrel);
        break;
      case SDL_MOUSEBUTTONDOWN:
        //std::cout << (int)event.button.button << std::endl;
        MouseButtonDown(
          event.button.button,
          event.motion.x,
          event.motion.y,
          event.motion.xrel,
          event.motion.yrel);
        break;
      }
    }

    m_iNextGameTick += SKIP_TICKS;
    loops++;
    m_Eps.event();
    Physics(); // call this in child class
    cout << "P: " << m_Eps.persecond() << endl;
  }
  if (loops == MAX_FRAMESKIP) {
    // computer is too slow to run game
    std::cout << "computer too slow" << std::endl;
    //return 1;
  }
}

void cGame::Drawingloop() {
  m_Fps.event();

  //SDL_FillRect(m_pScreen, 0, SDL_MapRGB(m_pScreen->format, 0, 0, 0)); // black background
  //cout << "  glClear: " << SDL_GetTicks() << endl;
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   // Clear The Screen And The Depth Buffer
  //cout << "  glClear Done: "  << SDL_GetTicks() << endl;
  //cout << "  glLoadIdentity: " << SDL_GetTicks() << endl;
  glLoadIdentity();
  //cout << "  glLoadIdentity Done: " << SDL_GetTicks() << endl;

  float interpolation = float( SDL_GetTicks() + SKIP_TICKS - m_iNextGameTick ) / float( SKIP_TICKS );
  //cout << "  myGame.Draw: " << SDL_GetTicks() << endl;
  Draw(interpolation); // call this in child class
  //cout << "  myGame.Draw Done: " << SDL_GetTicks() << endl;
  //cout << "D: " << m_Fps.persecond() << endl;

  //SDL_Flip(m_pScreen);
  //cout << "  SDL_GL_SwapBuffers: " << SDL_GetTicks() << endl;
  //glFinish();
  //SDL_GL_SwapBuffers();
  //glFinish();
  //cout << "  SDL_GL_SwapBuffers Done: " << SDL_GetTicks() << endl;
}

int cGame::Go() {
  Init(); // child class setup

  m_iNextGameTick = SDL_GetTicks();
  m_bGameIsRunning = true;
  while( m_bGameIsRunning ) {
    //cout << "Start Game Loop: " << endl;
    //cout << "Game Physics Loop: " << SDL_GetTicks() << endl;
    Physicsloop();
    //cout << "Game Physics Loop Done: " << SDL_GetTicks() << endl;
    //cout << "Game Drawing Loop: " << SDL_GetTicks() << endl;
    Drawingloop();
    //cout << "Game Drawing Loop Done: " << SDL_GetTicks() << endl << endl;
    //cout << "ERRORS: " << glGetError() << endl;
  }
  End(); // child class cleanup
  return 0;
}

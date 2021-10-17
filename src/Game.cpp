/**
 * Implementation file for Game class.
 * @file Game.cpp
 * @author Maxence
 * @version 1.0
*/

#include <random>

#include <SDL2/SDL.h>

#include "Game.hpp"

Game::Game(int windowWidth, int windowHeight, int frameRate):   window(nullptr), m_renderer(nullptr),
                                                                quit(false), ready(false),
                                                                gameWindowWidth(windowWidth), gameWindowHeight(windowHeight),
                                                                gameFrameRate(frameRate), timePerFrame(1000 / frameRate),
                                                                lastTime(0), timeElapsed(0), timeToWait(0),
                                                                mt(rd()), dist(0, 1000)
                                                                {}

void Game::setup() {
    setupSDL();
    ready = true;
    SDL_Log("The game is ready");
}

void Game::start() {
    if(ready) {
        mainLoop();
    } else {
        log("Setup the game before starting it");
    }
}

void Game::stop() {
    quit = true;
}

void Game::destroy() {
    destroySDL();
}

void Game::mainLoop() {
    while(!quit) {

        lastTime = SDL_GetTicks();

        recordInput();

        update();

        SDL_RenderPresent(m_renderer);

        timeElapsed = SDL_GetTicks() - lastTime; // In ms
        timeToWait = timePerFrame - timeElapsed;

        SDL_Log("wait time : %d", timeToWait);

        if (timeToWait > 0) {
            SDL_Delay(timeToWait);
        }

    }
}

void Game::setupSDL()
{

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    log("Error during initalization");
  }

  window = SDL_CreateWindow("Cactus Dodger", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, gameWindowWidth, gameWindowHeight, SDL_WINDOW_SHOWN);
  if(window == nullptr) {
      log("Error during window creation");
  }

  m_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
  if(m_renderer == nullptr) {
      log("Error during renderer creation");
  }
}

void Game::destroySDL() {
    if (SDL_RenderClear(m_renderer) != 0) {
        log("Error during renderer clearing");
    }
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

/* This function is called at every frame */
void Game::update() {}

void Game::handleInput(const SDL_Event &event) {
}

void Game::recordInput() {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            quit = true;
        } else {
           handleInput(event); 
        }
    }
}

void Game::drawTexture(SDL_Texture* texture, const SDL_Rect* srcRect, const SDL_Rect* destRect, const double angle, const SDL_Point* center, const SDL_RendererFlip flip) {
    if(SDL_RenderCopyEx(m_renderer, texture, srcRect, destRect, angle, center, flip) != 0) {
        log("Can't render the texture");
    };
}

void Game::log(const char errorString[]) {
    SDL_Log("%s : %s\n", errorString, SDL_GetError());
}
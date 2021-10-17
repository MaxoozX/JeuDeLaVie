#include <cmath>

#include <iostream>
#include <vector>
#include <utility>

#include <SDL2/SDL.h>

#include "Game.hpp"

const std::vector< std::pair<int, int> > neighborsOffsets = { {-1, -1}, {-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1} };

class LifeGame : public Game {

    private:
        int gridWidth;
        int gridHeight;
        bool evolve;
        bool drawing;
        bool penIsBlack;
        std::vector< std::vector<bool> > grid;
        void handleInput(const SDL_Event&);
        void computeLife();
    public:
        LifeGame(int = 100, int = 100, int = 30, int = 10, int = 10);
        void update();
        void set(int = 0, int = 0, bool = false);
};

LifeGame::LifeGame(int width, int height, int framerate, int _gridWidth, int _gridHeight): Game(width, height, framerate), gridWidth(_gridWidth), gridHeight(_gridHeight), grid(_gridHeight, std::vector<bool>(_gridWidth, false)), evolve(false), drawing(false), penIsBlack(false) {
    setup();
}

void LifeGame::update() {
    SDL_Log(evolve ? "Evolving" : "Paused");
    if(evolve) {
        computeLife();
    }
    double ratioX = gameWindowWidth / gridWidth;
    double ratioY = gameWindowHeight / gridHeight;
    for (int row = 0; row < gridHeight; row ++) {
        for (int column = 0; column < gridWidth; column++) {
            SDL_Rect rect {
                .x = column * ratioX,
                .y = row * ratioY,
                .w = ratioX,
                .h = ratioY
            };
            if(!grid[row][column]) {
                SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 1);
            } else {
                SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 1);
            }
            SDL_RenderFillRect(m_renderer, &rect);
        }
    }

}

void LifeGame::set(int row, int column, bool value) {
    grid[row][column] = value;
}

void LifeGame::handleInput(const SDL_Event& event) {
    switch(event.type) {

        case SDL_MOUSEBUTTONDOWN:
            {
                
                int x = event.button.x/(gameWindowHeight / gridHeight);
                int y = event.button.y/(gameWindowWidth / gridWidth);
                grid[y][x] = !grid[y][x];
                penIsBlack = !grid[x][y];
                drawing = true;
                break;
            }

        case SDL_MOUSEBUTTONUP:
            {
                drawing = false;
                break;
            }

        case SDL_MOUSEMOTION:
            {
                if(drawing) {
                    int x = event.motion.x/(gameWindowHeight / gridHeight);
                    int y = event.motion.y/(gameWindowWidth / gridWidth);
                    grid[y][x] = penIsBlack;
                }
            }

        case SDL_KEYDOWN:
            {
                if (event.key.repeat == 0) {
                    switch (event.key.keysym.sym) {
                        case SDLK_SPACE:
                            evolve = not evolve;
                            break;

                        default:
                            break;
                    }
                }
            }
        
        default:
            break;
    }
}

void LifeGame::computeLife() {
    std::vector< std::vector<bool> > copyGrid(grid);

    for (int row = 0; row < gridHeight; row ++) {
        for (int column = 0; column < gridWidth; column++) { 
            int neighborsCount = 0;
            for(auto offset: neighborsOffsets) {
                // Make sure this is ok
                int actualY = row + offset.first;
                int actualX = column + offset.second;
                if(actualX >= 0 && actualX < gridWidth && actualY >= 0 && actualY < gridHeight) {
                    if(copyGrid[actualY][actualX]) {
                        neighborsCount++;
                    }
                }
            }
            switch(neighborsCount) {
                case 3:
                    grid[row][column] = true;
                    break;
                case 2:
                    break;
                default:
                    grid[row][column] = false;
                    break;
            }
        }
    }

}


int main(int argc, char** argv) {

    LifeGame game(1000, 1000, 10, 500, 500);
    game.set(20, 20, true);
    game.set(21, 20, true);
    game.set(22, 20, true);
    game.set(20, 19, true);
    game.set(21, 18, true);
    game.start();

    return 0;
}
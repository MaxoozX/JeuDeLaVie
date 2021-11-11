#include <cmath>

#include <iostream>
#include <vector>
#include <utility>
#include <unordered_set>

#include <SDL2/SDL.h>

#include "Game.hpp"

const std::vector< std::pair<int, int> > neighborsOffsets = { {-1, -1}, {-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1} };

class LifeGame : public Game {

    private:
        int gridWidth;
        int gridHeight;
        int ratioX;
        int ratioY;
        bool evolve;
        bool drawing;
        bool penIsBlack;
        std::vector<char> grid;
        std::unordered_set<int> toCompute;
        void handleInput(const SDL_Event&);
        void computeLife();
        void updateDisplay();
    public:
        LifeGame(int = 100, int = 100, int = 30, int = 10, int = 10);
        void update();
        void set(int = 0, int = 0, bool = false);
};

LifeGame::LifeGame(int width, int height, int framerate, int _gridWidth, int _gridHeight): Game(width, height, framerate), gridWidth(_gridWidth), gridHeight(_gridHeight), grid(_gridWidth * _gridHeight, 0), evolve(false), drawing(false), penIsBlack(false), ratioY(height / _gridHeight), ratioX((width / _gridWidth)) {
    setup();

    SDL_Rect rect {
      .x = 0,
      .y = 0,
      .w = ratioX-1,
      .h = ratioY-1
    };
    
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 1);
    SDL_RenderClear(m_renderer);
    SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 1);
    
    char* cellPointer = grid.data();
    
    for (int row = 0; row < gridHeight; row++, rect.x=0, rect.y+=ratioY) {
        
        for (int column = 0; column < gridWidth; column++, rect.x+=ratioX, cellPointer++) {
            SDL_RenderFillRect(m_renderer, &rect);
        }
    }

}

void LifeGame::update() {
  
    SDL_Log(evolve ? "Evolving" : "Paused");
    
    if(evolve) {
        computeLife();
    }
    
    // updateDisplay();

}

// Useless now
void LifeGame::updateDisplay() {
  
  SDL_Rect rect {
    .x = 0,
    .y = 0,
    .w = ratioX-1,
    .h = ratioY-1
  };
  
  SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 1);
  SDL_RenderClear(m_renderer);
  
  char* cellPointer = grid.data();
  
  for (int row = 0; row < gridHeight; row++, rect.x=0, rect.y+=ratioY) {
      
      for (int column = 0; column < gridWidth; column++, rect.x+=ratioX, cellPointer++) {
          
          if(*cellPointer & 1) {
              SDL_SetRenderDrawColor(m_renderer, 0, 0, 255, 1);
          } else {
              SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 1);
          }
          SDL_RenderFillRect(m_renderer, &rect);
      }
  }
  
}

void LifeGame::set(int row, int column, bool value) {
  int valueToAdd = 0;
  int coordinate = row * gridWidth + column;
  char* cellPointer = grid.data() + coordinate;
  toCompute.insert(coordinate);

  SDL_Rect rect {
    .x = column * ratioX,
    .y = row * ratioY,
    .w = ratioX-1,
    .h = ratioY-1
  };
  
  if(value) {
    *cellPointer |= 1; // Set the low bit to 1
    valueToAdd = 2;
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 255, 1);
  } else {
    *cellPointer &= 0xFE; // Set the low bit to 0
    valueToAdd = -2;
    SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 1);
  }
  SDL_RenderFillRect(m_renderer, &rect);
  
  // Top
  if(row > 0) {
    
    char* rowPointer = cellPointer - gridWidth;
    int index = coordinate - gridWidth;
    
    // Middle
    *rowPointer += valueToAdd;
    toCompute.insert(index);
    
    // Left
    if(column > 0) {
      *(rowPointer - 1) += valueToAdd;
      toCompute.insert(index - 1);
    }
    
    // Right
    if(column + 1 < gridWidth) {
      *(rowPointer + 1) += valueToAdd;
      toCompute.insert(index + 1);
    }
    
  }
  
  if (row + 1 < gridHeight) {
    
    char* rowPointer = cellPointer + gridWidth;
    int index = coordinate + gridWidth;
    
    // Middle
    *rowPointer += valueToAdd;
    toCompute.insert(index);
    
    // Left
    if(column > 0) {
      *(rowPointer - 1) += valueToAdd;
      toCompute.insert(index - 1);
    }
    
    // Right
    if(column + 1 < gridWidth) {
      *(rowPointer + 1) += valueToAdd;
      toCompute.insert(index + 1);
    }

  }
  
  // Left
  if(column > 0) {
    *(cellPointer - 1) += valueToAdd;
    toCompute.insert(coordinate - 1);
  }
  
  // Right
  if(column + 1 < gridWidth) {
    *(cellPointer + 1) += valueToAdd;
    toCompute.insert(coordinate + 1);
  }
  
}

void LifeGame::handleInput(const SDL_Event& event) {
    switch(event.type) {

      case SDL_MOUSEBUTTONDOWN:
          {
            int x = event.button.x/ratioY;
            int y = event.button.y/ratioX;
            if(grid[y*gridWidth+x] & 1) {
              penIsBlack = false;
              set(y, x, false);
            } else {
              penIsBlack = true;
              set(y, x, true);
            }
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
              int x = event.motion.x/ratioX;
              int y = event.motion.y/ratioY;
              if(penIsBlack) {
                set(y, x, true);
              } else {
                set(y, x, false);
              }
            }
          }

      case SDL_KEYDOWN:
          {
            if (event.key.repeat == 0) {
              switch (event.key.keysym.sym) {
                case SDLK_SPACE:
                  evolve = !evolve;
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
  
    std::vector<char> copyGrid(grid); // Here the copy is probably extremely expensive
    char* copyGridPointer = copyGrid.data();
    
    int neighborsCount = 0;
    
    for(int row = 0; row < gridHeight; row++) {
      
      for(int column = 0; column < gridWidth; column++) {
        
        while((*copyGridPointer) == 0) {
          copyGridPointer++;
          if(++column >= gridWidth) goto rowDone;
        }
        
        neighborsCount = (*copyGridPointer) >> 1;
        
        if((*copyGridPointer) & 1) {
          if(neighborsCount != 2 && neighborsCount != 3) {
            set(row, column, false);
          }
        } else {
          if(neighborsCount == 3) {
            set(row, column, true);
          }
        }
        
        copyGridPointer++;
        
      }
      
      rowDone: continue; // Label to end multiple loops with goto
    }

    // SDL_Log("Length of toCompute : %d", toCompute.size());
    
    // for(auto itr = toCompute.begin(); itr != toCompute.end(); itr++) {
      
    //   // SDL_Log("copyGridPointer %p, offset : %d", copyGridPointer, *itr);
    //   neighborsCount = copyGridPointer[*itr] >> 1;
      
    //   if(copyGridPointer[*itr] & 1) {
    //     if(neighborsCount != 2 && neighborsCount != 3) {
    //       set((*itr)/gridWidth, (*itr)%gridWidth, false);
    //     }
    //   } else {
    //     if(neighborsCount == 3) {
    //       set((*itr)/gridWidth, (*itr)%gridWidth, true);
    //     }
    //   }
    // }

    // // Don't forget to remove the cells that are no longer useful to compute
    // for(auto itr = toCompute.begin(); itr != toCompute.end();) {
    //   if(grid[*itr] == 0) {
    //     itr = toCompute.erase(itr);
    //   } else {
    //     itr++;
    //   }
    // }
    
    // Not compute ALL the cells, just the ones in the set

}


int main(int argc, char** argv) {

    LifeGame game(500, 500, 30, 500, 500);
    game.set(20, 20, true);
    game.set(21, 20, true);
    game.set(22, 20, true);
    game.set(20, 19, true);
    game.set(21, 18, true);
    game.start();

    return 0;
}

/*
Optimisation :
  - Change a 2d array into a 1d array
  - Change bool to char
  - Change the for loop to get the neighbors into a simple hard-written check
  
  
  ? - Add the number of neighbors into the bit
  
  At this point with 500x500 at 20 FPS => wait time ~5ms
  
  - Optimization to skip empty cells (cells at 0)
  
  At this point with 500x500 at 20 FPS => wait time ~10ms
  
  - Optimizing the display loop to get a wait time of about 500x500 20FPS ~12/15ms
  
  ## Display at the same time we compute to loop one less time ?? Maybe but that's not necessary for now
  
  ## That's good, but let's keep track of bits we need to change
    We can use a std::set or std::unordered_set to prevent duplicate elements, but this may not be fast enough
    With set we can get : iteration = O(n), insertion = O(1)
    !!! Actually not that fast, almost not fast at all
    Why clear the renderer each time




    There are 3 bottlenecks:
      - Copying the whole array once every frame : Can't find a solution
      - Iterating through the pixels once every frame : Solved, just change the pixel when changing the array value
      - Iterating through the whole array to compute : I can use set, but I already iterate one time, a second is not that costful
*/
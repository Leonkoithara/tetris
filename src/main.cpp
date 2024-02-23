#include <iostream>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

bool running;
int score = 0;

void process_events();
std::vector<SDL_Rect> get_random_shape();
void render_shape_at_pos(SDL_Renderer *renderer, std::vector<SDL_Rect> shape, int x, int y);

int main()
{
    running = true;
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        std::cout << "SDL Initialization failed" << std::endl;
        exit(1);
    }

    if(TTF_Init() < 0)
    {
        std::cout << "Couldn't initialize TTF lib: " << TTF_GetError() << std::endl;
        exit(1);
    }

    SDL_Window *win = SDL_CreateWindow("Tetris", 10, 10, 480, 960, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(win, -1, 0);
    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0, &DM);
    float width = DM.w;
    float height = DM.h;
    
    while (running)
	{
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderDrawRect(renderer, NULL);
        SDL_RenderPresent(renderer);
        process_events();
    }
    
    return 0;
}

void process_events()
{
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type)
	{
        case SDL_QUIT:
            running = false;
            break;
        default:
            break;
    }
}

std::vector<SDL_Rect> get_random_shape()
{
    std::vector<SDL_Rect> I_block = {
        { 0, 0, 1, 4 }
    };
    std::vector<SDL_Rect> L_block = {
        { 0, 0, 1, 1 },
        { 0, 1, 3, 1 }
    };
    std::vector<SDL_Rect> J_block = {
        { 0, 1, 3, 1 },
        { 2, 0, 1, 1 }
    };
    std::vector<SDL_Rect> O_block = {
        { 0, 0, 2, 2 }
    };
    std::vector<SDL_Rect> S_block = {
        { 0, 1, 2, 1 },
        { 1, 0, 2, 1 }
    };
    std::vector<SDL_Rect> T_block = {
        { 0, 1, 3, 1 },
        { 1, 0, 1, 1 }
    };
    std::vector<SDL_Rect> Z_block = {
        { 0, 0, 2, 1 },
        { 1, 1, 2, 1 }
    };

    return Z_block;
}

void render_shape_at_pos(SDL_Renderer *renderer, std::vector<SDL_Rect> shape, int x, int y)
{
    int block_size = 48;
    for (SDL_Rect rectangle : shape)
	{
        rectangle.x += x;
        rectangle.y += y;
        rectangle.x *= block_size;
        rectangle.y *= block_size;
        rectangle.w *= block_size;
        rectangle.h *= block_size;
        SDL_RenderFillRect(renderer, &rectangle);
    }
}

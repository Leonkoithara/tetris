#include "SDL_render.h"
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>


bool running;
bool falling = true;
int block_size = 48;
int score = 0;
int screen_x = 480;
int screen_y = 960;
std::vector<std::vector<SDL_Rect>> shapes;
std::vector<std::pair<int, int>> shape_positions;
std::vector<SDL_Rect> falling_shape;

void process_events();
std::vector<SDL_Rect> get_random_shape();
void render_shape_at_pos(SDL_Renderer *renderer, std::vector<SDL_Rect> shape,
                         int x, int y);
void update_falling_blocks();
void spawn_new();

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

    SDL_Window *win = SDL_CreateWindow("Tetris", 10, 10, screen_x, screen_y, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(win, -1, 0);
    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0, &DM);
    float width = DM.w;
    float height = DM.h;
    
    auto old_time = std::chrono::high_resolution_clock::now();
    int block_speed = 1; // falls 1 block every 1 second

    falling_shape = get_random_shape();
    shapes.push_back(falling_shape);
    shape_positions.push_back(std::pair<int, int>(0, 0));

    while (running)
	{
        auto new_time = std::chrono::high_resolution_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::seconds>(new_time - old_time);
        if (diff.count() >= block_speed)
        {
            update_falling_blocks();
            old_time = new_time;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderDrawRect(renderer, NULL);

        for (int i=0; i<shapes.size(); i++)
		{
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            for (SDL_Rect rectangle : shapes[i])
			{
                rectangle.x += shape_positions[i].first;
                rectangle.y += shape_positions[i].second;
                rectangle.x *= block_size;
                rectangle.y *= block_size;
                rectangle.w *= block_size;
                rectangle.h *= block_size;

                SDL_RenderFillRect(renderer, &rectangle);
            }
        }
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
    std::vector<std::vector<SDL_Rect>> blocks = {
        {
            { 0, 0, 1, 4 }
        },
        {
            { 0, 0, 1, 1 },
            { 0, 1, 3, 1 }
        },
        {
            { 2, 0, 1, 1 },
            { 0, 1, 3, 1 }
        },
        {
            { 0, 0, 2, 1 },
            { 0, 1, 2, 1 }
        },
        {
            { 1, 0, 2, 1 },
            { 0, 1, 2, 1 }
        },
        {
            { 1, 0, 1, 1 },
            { 0, 1, 3, 1 }
        },
        {
            { 0, 0, 2, 1 },
            { 1, 1, 2, 1 }
        }
    };

    int rand_num = rand()%7;

    return blocks[rand_num];
}

void render_shape_at_pos(SDL_Renderer *renderer, std::vector<SDL_Rect> shape, int x, int y)
{
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

void update_falling_blocks()
{
    int xb = falling_shape.back().x + shape_positions.back().first;
    int wb = falling_shape.back().w;
    int yb = falling_shape.back().y + shape_positions.back().second;
    std::cout << xb << " " << wb << " " << yb << std::endl;;
    for (int i=0; i<shape_positions.size(); i++)
	{
        int x = shapes[i].front().x + shape_positions[i].first;
        int w = shapes[i].front().w;
        int y = shapes[i].front().y + shape_positions[i].second;
        if ((y == yb+1 && (xb <= x+w && xb+wb >= x)) || yb == 19)
		{
            std::cout << x << " " << w << " " << y << std::endl;
            falling = false;
            break;
        }
    }
    if (falling)
        shape_positions.back().second++;
    else
        spawn_new();
}

void spawn_new()
{
    falling = true;
    falling_shape = get_random_shape();
    shapes.push_back(falling_shape);
    shape_positions.push_back(std::pair<int, int>(0, 0));
}

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>


bool running;
bool falling = true;
int block_size = 48;
int falling_shape_index;
int rotation = 0;
int score = 0;
int screen_x = 480;
int screen_y = 960;
std::vector<std::vector<SDL_Rect>> shapes;
std::vector<std::pair<int, int>> shape_positions;
std::vector<std::vector<SDL_Rect>> spawn_blocks = {
    {
        { 0, 0, 4, 1 }        // I Block
    },
    {
        { 0, 0, 1, 1 },
        { 0, 1, 1, 1 },
        { 0, 2, 1, 1 },
        { 0, 3, 1, 1 }
    },
    {
        { 0, 0, 1, 1 },       // L Block
        { 0, 1, 3, 1 }
    },
    {
        { 0, 0, 2, 1 },
        { 0, 1, 1, 1 },
        { 0, 2, 1, 1 }
    },
    {
        { 0, 0, 3, 1 },
        { 2, 1, 1, 1 }
    },
    {
        { 1, 0, 1, 1 },
        { 1, 1, 1, 1 },
        { 1, 2, 2, 1 }
    },
    {
        { 2, 0, 1, 1 },       // J Block
        { 0, 1, 3, 1 }
    },
    {
        { 0, 0, 1, 1 },
        { 0, 1, 1, 1 },
        { 0, 2, 2, 1 }
    },
    {
        { 0, 0, 3, 1 },
        { 0, 1, 1, 1 }
    },
    {
        { 0, 0, 2, 1 },
        { 1, 1, 1, 1 },
        { 1, 2, 1, 1 }
    },
    {
        { 0, 0, 2, 1 },       // O Block
        { 0, 1, 2, 1 }
    },
    {
        { 1, 0, 2, 1 },       // S Block
        { 0, 1, 2, 1 }
    },
    {
        { 0, 0, 1, 1 },
        { 0, 1, 2, 1 }
    },
    {
        { 1, 0, 1, 1 },       // T block
        { 0, 1, 3, 1 }
    },
    {
        { 0, 0, 1, 1 },
        { 0, 1, 2, 1 },
        { 0, 2, 1, 1 }
    },
    {
        { 0, 0, 3, 1 },
        { 1, 1, 1, 1 }
    },
    {
        { 1, 0, 1, 1 },
        { 0, 1, 2, 1 },
        { 1, 2, 1, 1 }
    },
    {
        { 0, 0, 2, 1 },       // Z Block
        { 1, 1, 2, 1 }
    },
    {
        { 1, 0, 1, 1 },
        { 0, 1, 2, 1 },
        { 0, 2, 1, 1 }
    }
};

void process_events();
void process_keystoke(unsigned int key, unsigned int mod);
int get_random_shape();
void render_shape_at_pos(SDL_Renderer *renderer, std::vector<SDL_Rect> shape,
                         int x, int y);
void rotate(bool direction);
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
    int block_speed = 500; // falls 1 block every 500 milliseconds

    spawn_new();

    while (running)
	{
        auto new_time = std::chrono::high_resolution_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(new_time - old_time);
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
        case SDL_KEYDOWN:
            process_keystoke(event.key.keysym.sym, event.key.keysym.mod);
            break;
        case SDL_QUIT:
            running = false;
            break;
        default:
            break;
    }
}

void process_keystoke(unsigned int key, unsigned int mod)
{
    int max_x = 0, min_x = 10;
    std::vector<SDL_Rect> falling_shape = spawn_blocks[falling_shape_index];
    for (SDL_Rect block : falling_shape)
    {
        if (max_x < block.x+block.w)
            max_x = block.x+block.w;
        if (min_x > block.x)
            min_x = block.x;
    }
    switch (key)
	{
        case 'w':
            rotate(true);
            break;
        case 'a':
            if (shape_positions.back().first+min_x > 0)
                shape_positions.back().first--;
            break;
        case 's':
            rotate(false);
            break;
        case 'd':
            if (shape_positions.back().first+max_x < 10)
                shape_positions.back().first++;
            break;
        default:
            std::cout << "Unknown" << std::endl;
            break;
    }
}

int get_random_shape()
{
    int rand_num = rand()%7;

    return rand_num;
}

void rotate(bool direction)
{
    std::vector<int> shape_indices = { 0, 2, 6, 10, 11, 13, 17, 19 };
    std::vector<int> max_shape_orientations = { 2, 4, 4, 1, 2, 4, 2 };

    int current_shape;
    for (int i=0; i<shape_indices.size(); i++)
	{
        if (falling_shape_index < shape_indices[i])
        {
            current_shape = i-1;
            int max_orientations = max_shape_orientations[current_shape];
            if (direction)
			{
                if (falling_shape_index+1 < shape_indices[current_shape+1])
                    falling_shape_index++;
                else
                    falling_shape_index = shape_indices[current_shape];
            }
            else
            {
                if (falling_shape_index-1 >= shape_indices[current_shape])
                    falling_shape_index--;
                else
                    falling_shape_index = shape_indices[current_shape+1]-1;
            }
            shapes.pop_back();
            shapes.push_back(spawn_blocks[falling_shape_index]);
            break;
        }
    }
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
    std::vector<SDL_Rect> falling_shape = spawn_blocks[falling_shape_index];
    for (SDL_Rect falling_blocks : falling_shape)
	{
        int xb = shape_positions.back().first+falling_blocks.x;
        int yb = shape_positions.back().second+falling_blocks.y;
        int wb = falling_blocks.w;
        if (yb == 19)
        {
            falling = false;
            break;
        }
        for (int i=0; i<shapes.size()-1; i++)
		{
            std::vector<SDL_Rect> shape = shapes[i];
            for (SDL_Rect block : shape)
			{
                int xt = shape_positions[i].first+block.x;
                int yt = shape_positions[i].second+block.y;
                int wt = block.w;

                if (yt == yb+1 && (xb < xt+wt && xb+wb > xt))
				{
                    falling = false;
                    break;
                }
            }
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
    falling_shape_index = get_random_shape();
    shapes.push_back(spawn_blocks[falling_shape_index]);
    shape_positions.push_back(std::pair<int, int>(3, 0));
}

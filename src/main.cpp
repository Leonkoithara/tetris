#include <chrono>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>


bool running;
bool falling = true;
int block_size = 48;
int block_speed = 250;
int falling_shape_index;
int score = 0;
int screen_x = 480;
int screen_y = 960;
int filled_blocks[20];
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
        { 0, 1, 2, 1 },
        { 1, 2, 1, 1 }
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

void move(bool direction);
void process_events();
void process_keystoke(unsigned int key, unsigned int mod);
int get_random_shape();
bool orientation_possible(int orientation_index);
void render_shape_at_pos(SDL_Renderer *renderer, std::vector<SDL_Rect> shape,
                         int x, int y);
void rotate(bool direction);
void update_falling_blocks();
void spawn_new();

int main()
{
    running = true;
    srand(time(NULL));

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

    falling = true;
    falling_shape_index = get_random_shape();
    shapes.push_back(spawn_blocks[falling_shape_index]);
    shape_positions.push_back(std::pair<int, int>(3, 0));

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
    switch (key)
	{
        case 'w':
            rotate(true);
            break;
        case 'a':
            move(false);
            break;
        case 's':
            rotate(false);
            break;
        case 'd':
            move(true);
            break;
        default:
            std::cout << "Unknown" << std::endl;
            break;
    }
}

void move(bool direction)
{
    std::vector<SDL_Rect> falling_shape = spawn_blocks[falling_shape_index];
    bool valid_move = true;

    for (SDL_Rect falling_blocks : falling_shape)
	{
        int xb = shape_positions.back().first+falling_blocks.x;
        int yb = shape_positions.back().second+falling_blocks.y;
        int wb = falling_blocks.w;
        if (xb+wb == 10 && direction)
        {
            valid_move = false;
            break;
        }
        if (xb == 0 && !direction)
        {
            valid_move = false;
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

                if (yt == yb)
				{
                    if (xb+wb == xt && direction)
                    {
                        valid_move = false;
                        break;
                    }
                    if (xt+wt == xb && !direction)
                    {
                        valid_move = false;
                        break;
                    }
                }
            }
        }
    }

    if (valid_move)
        shape_positions.back().first += direction ? 1 : -1;
}

int get_random_shape()
{
    int rand_num = rand()%7;
    std::vector<int> shape_indices = { 0, 2, 6, 10, 11, 13, 17, 19 };
    std::vector<int> max_shape_orientations = { 2, 4, 4, 1, 2, 4, 2 };
    int orientation = rand()%max_shape_orientations[rand_num];

    return shape_indices[rand_num]+orientation;
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
                {
                    if (orientation_possible(falling_shape_index+1))
                        falling_shape_index++;
                }
                else
                {
                    if (orientation_possible(shape_indices[current_shape]))
                        falling_shape_index = shape_indices[current_shape];
                }
            }
            else
            {
                if (falling_shape_index-1 >= shape_indices[current_shape])
                {
                    if (orientation_possible(falling_shape_index-1))
                        falling_shape_index--;
                }
                else
                {
                    if (orientation_possible(shape_indices[current_shape+1]-1))
                        falling_shape_index = shape_indices[current_shape+1]-1;
                }
            }
            shapes.pop_back();
            shapes.push_back(spawn_blocks[falling_shape_index]);
            break;
        }
    }
}

bool orientation_possible(int orientation_index)
{
    std::vector<SDL_Rect> falling_shape = spawn_blocks[orientation_index];
    bool possible = true;

    for (SDL_Rect falling_blocks : falling_shape)
	{
        int xb = shape_positions.back().first+falling_blocks.x;
        int yb = shape_positions.back().second+falling_blocks.y;
        int wb = falling_blocks.w;

        if (xb+wb > 10)
        {
            possible = false;
            break;
        }
        if (xb < 0)
        {
            possible = false;
            break;
        }
        if (yb > 20)
		{
            possible = false;
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

                if (yt == yb)
				{
                    if (xb+wb >= xt)
                    {
                        possible = false;
                        break;
                    }
                    if (xt+wt >= xb)
                    {
                        possible = false;
                        break;
                    }
                }
            }
        }
    }

    return possible;
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
    if (shape_positions.back().second <= 0)
	{
        std::cout << "Game Over! You scored: " << score << std::endl;
        running = false;
        return;
    }

    std::vector<SDL_Rect> falling_shape = spawn_blocks[falling_shape_index];
    for (SDL_Rect falling_blocks : falling_shape)
	{
        int xb = shape_positions.back().first+falling_blocks.x;
        int yb = shape_positions.back().second+falling_blocks.y;
        int wb = falling_blocks.w;
        filled_blocks[yb] += wb;
    }
    for (int i=19; i>0; i--)
	{
        if (filled_blocks[i] == 10)
		{
            score += 10;
            block_speed -= 10;
            for (int j=0; j<shapes.size(); j++)
			{
                std::vector<SDL_Rect> shape = shapes[j];
                for (int k=shape.size()-1; k>=0; k--)
				{
                    if (shape_positions[j].second+shape[k].y == i)
					{
                        if (shape.size() == 1)
						{
                            shapes.erase(shapes.begin()+j);
                            shape_positions.erase(shape_positions.begin()+j);
                            j--;
                        }
                        else
                        {
                            shapes[j].erase(shapes[j].begin()+k);
                        }
                    }
                }
            }
        }
    }
    std::vector<std::vector<SDL_Rect>> tmp = shapes;
    for (int i=19; i>0; i--)
	{
        if (filled_blocks[i] == 10)
		{
            for (int j=0; j<shapes.size(); j++)
			{
                std::vector<SDL_Rect> shape = shapes[j];
                for (int k=shape.size()-1; k>=0; k--)
				{
                    if (shape_positions[j].second+shape[k].y < i)
                        tmp[j][k].y++;
                }
            }
        }
    }
    for (int i=0; i<shapes.size(); i++)
	{
        for (int j=0; j<shapes[i].size(); j++)
		{
            shapes[i][j] = tmp[i][j];
        }
    }
    for (int i=19; i>0; i--)
	{
        if (filled_blocks[i] == 10)
		{
            for (int l=i; l>0; l--)
                filled_blocks[l] = filled_blocks[l-1];
            filled_blocks[0] = 0;
        }
    }
    falling = true;
    falling_shape_index = get_random_shape();
    shapes.push_back(spawn_blocks[falling_shape_index]);
    shape_positions.push_back(std::pair<int, int>(3, 0));
}

#include <iostream>
#include <ctime>
#include <cstdlib>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <vector>
#include <string>
#include <sstream>

using namespace std;

bool again = true;
int num_line = 3;

string sprite_path;
const string screen_title   = "Puzzle 4x4";

const int sprite_cell_width = 125;
const int sprite_cell_height = 125;
const int window_cell_width = 150;
const int window_cell_height = 150;

const int font_size = 25;

////////////////////////kieu du lieu moi ///////////////////////////////

enum sprite_type
{
    sprite_1,
    sprite_2,
    sprite_3,
    sprite_4,
    sprite_5,
    sprite_6,
    sprite_7,
    sprite_8,
    sprite_9,
    sprite_10,
    sprite_11,
    sprite_12,
    sprite_13,
    sprite_14,
    sprite_15,
    sprite_0,
    sprite_all
};

struct cellPos
{
   int row;
   int col;
};

typedef vector< vector<int> > celltable;

struct Game
{
   celltable cells;
   cellPos first_click;
   cellPos last_click;
   int turn_move;
};

struct graphic
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture *spriteTexture;
    TTF_Font* font;
    vector<SDL_Rect> spriteRects;
};

////////////////////////////// Ham //////////////////////////////////

bool initGraphic(graphic &g);

void close(graphic &g); // huy cac khoi tao tu sdl

SDL_Texture* createTexture(SDL_Renderer *renderer, const string &path); // load anh tu path va tra ve texture

void initSpriteRects(vector<SDL_Rect> &rects); // luu vi tri cac hinh trong anh vao rects

void initGame(Game &game); // khoi tao game va cac o xep ngau nhien

void drawGame(Game &game, graphic &g);  // hien thi game len cua so

SDL_Rect getSpriteRect (const Game &game, const cellPos &pos, const vector<SDL_Rect> &spriteRects); // lay hinh se ve cho o o vi tri pos

void updateGame(Game &game, SDL_Event &event); // cap nhat game khi co thao tac

bool check_win(Game &game);  // return true neu win///

void draw_text(graphic &g, Game &game, string text, int x, int y);

void draw_illustration(graphic &g);

void drawline(graphic &g);

void drawline_2(graphic &g);

string randomSprite(); //chon ngau nhien anh de choi

void autoWin(Game &game, SDL_Event &event);

//////////////////////ham main
///////////////////////////////////

int main(int argc, char* argv[])
{
    srand(time(NULL));

    while (again)
    {
        sprite_path = randomSprite();

        graphic g;
        if (!initGraphic(g))
           {
              close(g);
              return 1;
           }

        drawline(g);

        Game game;
        initGame(game);

        bool running = true;
        while (running)
            {
                drawGame(game, g);

                SDL_Event event;
                while (SDL_PollEvent(&event))
                      {
                          if (event.type == SDL_QUIT)
                             {
                                 running = false;
                                 again = false;
                                 break;
                             }

                          if (check_win(game) and event.type == SDL_KEYDOWN and event.key.keysym.sym == SDLK_RETURN)
                          {
                              running = false;
                              game.turn_move = 0;
                              break;
                          }

                          if (event.type == SDL_KEYDOWN and event.key.keysym.sym == SDLK_LCTRL) autoWin(game, event);
                          else updateGame(game, event);

                      }
            }
        close(g);
    }

    return 0;
}

/////////////////////////////////////////////////////////////////////

string randomSprite()
{
    int temp = 1 + rand() % 4;

    switch (temp)
    {
        case 1: return "sprite/doremon.png";
        case 2: return "sprite/hiroomi.png";
        case 3: return "sprite/pikachu.png";
        case 4: return "sprite/board_game.png";

        default: break;
    }
}

bool initGraphic(graphic &g)
{
    g.window = NULL;
    g.renderer = NULL;
    g.spriteTexture = NULL;
    g.font = NULL;

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        cout << "error SDL";
        return false;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        cout << "error SDL_Image";
        return false;
    }

    if (TTF_Init() < 0)
    {
        cout << "error SDL_ttf";
        return false;
    }

    g.window = SDL_CreateWindow(screen_title.c_str(),
                                SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED,
                                6 * window_cell_width,
                                4 * window_cell_height,
                                SDL_WINDOW_SHOWN);

    if (g.window == NULL)
    {
        cout << "error open window";
        return false;
    }

    g.renderer = SDL_CreateRenderer(g.window, -1, SDL_RENDERER_ACCELERATED);

    if (g.renderer == NULL)
    {
        cout << "error create renderer";
        return false;
    }

    g.spriteTexture = createTexture(g.renderer, sprite_path);

    if (g.spriteTexture == NULL)
    {
        cout << "unable to create texture";
        return false;
    }

    initSpriteRects(g.spriteRects);

    g.font = TTF_OpenFont("font/VeraMoBd.ttf", font_size);

    if (g.font == NULL)
    {
        cout << "error font";
        return false;
    }

    return true;
}

void close(graphic &g)
{
    SDL_DestroyWindow(g.window);
    SDL_DestroyRenderer(g.renderer);
    SDL_DestroyTexture(g.spriteTexture);

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

SDL_Texture* createTexture(SDL_Renderer *renderer, const string &path)
{
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (surface == NULL)
    {
        cout << "error load image";
        return NULL;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    return texture;
}

void initSpriteRects(vector<SDL_Rect> &rects)
{
    for (int i = 0; i < 16; i ++)
    {
        sprite_type type = (sprite_type) i;
        SDL_Rect rect = {0, 0, sprite_cell_width, sprite_cell_height};

        if (type >= sprite_1 and type <= sprite_4)
        {
            rect.x = i *sprite_cell_width;
            rect.y = 0;
        }
        else switch (type)
        {
            case sprite_5:
                {
                    rect.x = 0;
                    rect.y = sprite_cell_height;
                    break;
                }
            case sprite_6:
                {
                    rect.x = 1*sprite_cell_width;
                    rect.y = sprite_cell_height;
                    break;
                }
            case sprite_7:
                {
                    rect.x = 2*sprite_cell_width;
                    rect.y = sprite_cell_height;
                    break;
                }
            case sprite_8:
                {
                    rect.x = 3*sprite_cell_width;
                    rect.y = sprite_cell_height;
                    break;
                }
            case sprite_9:
                {
                    rect.x = 0;
                    rect.y = 2*sprite_cell_height;
                    break;
                }
            case sprite_10:
                {
                    rect.x = 1*sprite_cell_width;
                    rect.y = 2*sprite_cell_height;
                    break;
                }
            case sprite_11:
                {
                    rect.x = 2*sprite_cell_width;
                    rect.y = 2*sprite_cell_height;
                    break;
                }
            case sprite_12:
                {
                    rect.x = 3*sprite_cell_width;
                    rect.y = 2*sprite_cell_height;
                    break;
                }
            case sprite_13:
                {
                    rect.x = 0;
                    rect.y = 3*sprite_cell_height;
                    break;
                }
            case sprite_14:
                {
                    rect.x = 1*sprite_cell_width;
                    rect.y = 3*sprite_cell_height;
                    break;
                }
            case sprite_15:
                {
                    rect.x = 2*sprite_cell_width;
                    rect.y = 3*sprite_cell_height;
                    break;
                }
            case sprite_0:
                {
                    rect.x = 3*sprite_cell_width;
                    rect.y = 3*sprite_cell_height;
                }
            default: break;
        }
        rects.push_back(rect);
    }

    SDL_Rect rect {0,0,500,500};
    rects.push_back(rect);

}

void initGame(Game &game)
{
    game.turn_move = 0;
    game.cells = celltable(4, vector<int> (4));

    bool choose[17];
    for (int i = 0; i < 17; i ++) choose[i] = false;

    for (int i = 0; i < 4; i ++ )
    {
        int j = 0;
        while (j < 4)
        {
            int tmp = rand() % (16);
            if (choose[tmp] == false)
            {
                game.cells[i][j] = tmp;
                choose[tmp] = true;
                j ++;
            }
        }
    }

}

void drawGame(Game &game, graphic &g)
{
    SDL_RenderClear(g.renderer); // xoa man hinh
    drawline_2(g);

    SDL_SetRenderDrawColor(g.renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

    for (int i = 0; i < 4; i ++)
    {
        for (int j = 0; j < 4; j ++)
        {
            SDL_Rect destRect ={j * window_cell_width, i * window_cell_height, window_cell_width, window_cell_height};

            cellPos pos = {i, j};

            SDL_Rect srcRect = getSpriteRect(game, pos, g.spriteRects);

            SDL_RenderCopy(g.renderer, g.spriteTexture, &srcRect, &destRect);
        }
    }

    draw_illustration(g);

    draw_text(g, game, "Turn Moves: ", 4.45 * window_cell_width, 2 * window_cell_height);

    if (check_win(game))
    {
        draw_text(g, game, "YOU WIN!" , 4.5 * window_cell_width, 2.5 * window_cell_height);
        draw_text(g, game, "Press Enter To Play Again" , 4.4 * window_cell_width, 2.9 * window_cell_height);
    }

    SDL_RenderPresent(g.renderer);

}

SDL_Rect getSpriteRect(const Game &game,const cellPos &pos, const vector<SDL_Rect> &spriteRects)
{
    int i = game.cells[pos.row][pos.col];
    switch (i)
    {
        case 0 : return spriteRects[sprite_0];
        default: return spriteRects[i - 1];
    }
}

void updateGame(Game &game, SDL_Event &event)
{
    if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        SDL_MouseButtonEvent mouse = event.button;

        int row = mouse.y / window_cell_height;
        int col = mouse.x / window_cell_width;

        game.first_click = (cellPos) {row, col};
    }

    if (event.type == SDL_MOUSEBUTTONUP)
    {
        SDL_MouseButtonEvent mouse = event.button;

        int row = mouse.y / window_cell_height;
        int col = mouse.x / window_cell_width;

        game.last_click = (cellPos) {row, col};

        int x1 = game.first_click.row;
        int x2 = game.first_click.col;
        int x3 = game.last_click.row;
        int x4 = game.last_click.col;

        if (game.cells[x3][x4] == 0 and (x1 == x3 or x2 == x4) and (abs(x1 - x3) == 1 or abs(x2 - x4) == 1))
        {
            swap(game.cells[x1][x2], game.cells[x3][x4]);
            game.turn_move ++;
        }
    }
}

void draw_text(graphic &g, Game &game, string text, int x, int y)
{
    SDL_Color color = {243, 156, 0};
    SDL_Rect src;
    SDL_Rect des;

    des.x = x;
    des.y = y;

    if (text == "Turn Moves: ")
    {
        string turn_text;
        ostringstream convert;
        convert << game.turn_move;
        turn_text = convert.str();
        text = text + turn_text;
    }

    SDL_Surface* surface = TTF_RenderText_Solid(g.font,text.c_str(),color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(g.renderer, surface);
    SDL_FreeSurface(surface);

    TTF_SizeText(g.font,text.c_str(), &src.w, &src.h);

    src.x = 0;
    src.y = 0;

    des.w = src.w;
    des.h = src.h;

    if (text == "YOU WIN!")
    {
        des.w *= 1.8;
        des.h *= 1.8;
    }

    if (text == "Press Enter To Play Again")
    {
        des.w /= 1.6;
        des.h /= 1.6;
    }

    SDL_RenderCopy(g.renderer, texture, &src, &des);
    SDL_DestroyTexture(texture);
}

void draw_illustration(graphic &g)
{
    int x = 4.5 * window_cell_width;
    int w = 1.5 * window_cell_width;
    int h = 1.5 * window_cell_height;

    SDL_Rect destRect = {x, 0, w, h};

    SDL_Rect srcRect = g.spriteRects[sprite_all];

    SDL_RenderCopy(g.renderer, g.spriteTexture, &srcRect, &destRect);
}

bool check_win(Game &game)
{
    int number = 1;
    for (int i = 0; i < 4; i ++)
    {
        for (int j = 0; j < 4; j ++)
        {
            if (game.cells[i][j] == number)
            {
                number ++;
                if (number == 4*4) return true;
            }
            else return false;
        }
    }
}

void drawline(graphic &g)
{
        SDL_SetRenderDrawColor(g.renderer, 255, 55, 255, SDL_ALPHA_OPAQUE);
        for (int i =0; i < 4 * window_cell_height; i ++)
        {
            if (i >= 1.5 * window_cell_height)
            {
                for (int j = 0; j < 5 * num_line; j += 5)
                {
                    SDL_RenderDrawLine(g.renderer,
                                       i + 50 + 2.5 * window_cell_height,
                                       1.5 * window_cell_height + 25 + j,
                                       i + 50 + 2.5 * window_cell_height,
                                       1.5 * window_cell_height + 25 + j);
                }
            }

            for (int j = 0; j < 5 * num_line; j += 5)
            {
                SDL_RenderDrawLine(g.renderer,
                                   4 * window_cell_height + 40 + j,
                                   i,
                                   4 * window_cell_height + 40 + j,
                                   i + 2);
            }

            SDL_RenderPresent(g.renderer);
            SDL_Delay(3);
       }
}

void drawline_2(graphic &g)
{
            SDL_SetRenderDrawColor(g.renderer, 255, 55, 255, SDL_ALPHA_OPAQUE);
            for (int j = 0; j < 5 * num_line; j += 5)
                {
                    SDL_RenderDrawLine(g.renderer,
                                       50 + 4 * window_cell_height,
                                       1.5 * window_cell_height + 25 + j,
                                       50 + 6 * window_cell_height,
                                       1.5 * window_cell_height + 25 + j);
                }

            for (int j = 0; j < 5 * num_line; j += 5)
            {
                SDL_RenderDrawLine(g.renderer,
                                   4 * window_cell_height + 40 + j,
                                   0,
                                   4 * window_cell_height + 40 + j,
                                   4 * window_cell_height);
            }
}

void autoWin(Game &game, SDL_Event &event)
{
    int tmp = 1;

    for (int i = 0; i < 4; i ++)
    {
        for (int j = 0; j < 4; j ++)
        {
            if (i == 3 and j == 3) game.cells[i][j] = 0;
            else game.cells[i][j] = tmp;
            tmp ++;
        }
    }
}





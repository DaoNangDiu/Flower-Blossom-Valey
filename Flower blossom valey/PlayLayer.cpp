#include "PlayLayer.h"
#include "Elements.h"
#include <bits/stdc++.h>
#include "GameUtils.h"
#include "code.h"
#include "BaseObject.h"
#include "Function.h"

const int Esize = 112;
PlayLayer::PlayLayer( SDL_Renderer *newrender)
{
    renderer= newrender;
}
PlayLayer::~PlayLayer()
{}
void PlayLayer::createMatrix(int width, int height)
{
    matrix = new int*[width];
    for ( int i=0 ; i < width ; i++ )
        matrix[i] = new int[height];
    square = new int*[width];
    for ( int i = 0 ; i < width ; i++ )
        square[i] = new int[height];
}

bool PlayLayer::init()
{
    nhapfile();
    createMatrix(MATRIX_WIDTH, MATRIX_HEIGHT);
    for ( int i = 0 ; i < MATRIX_WIDTH ; i++ )
        for ( int j = 0 ; j < MATRIX_HEIGHT ; j++ )
        {
            f >> matrix[j][i];
            square[j][i]= matrix[j][i];
//            matrix[i][j] = rand()%5;
        }
    return true;
}


int PlayLayer::exec()
{
    auto oldTick = SDL_GetTicks();
    for ( auto done = false ; !done; )
    {
        SDL_RenderClear(renderer);
        drawMatrix();
        SDL_Event e;
        if ( SDL_PollEvent(&e) )
        {
            switch (e.type)
            {
            case SDL_MOUSEBUTTONUP:
            {
                auto x = (e.button.x-430) / (Esize);
                auto y = (e.button.y-10) / (Esize);
                if (selectedX >= 0 && selectedY >= 0 &&
                        ((abs(x - selectedX) == 1 && y == selectedY) || (abs(y - selectedY) == 1 && x == selectedX))&&matrix[selectedX][selectedY] >=0)
                {
                    std::swap(matrix[x][y], matrix[selectedX][selectedY]);
                    bee2(matrix, x, y, selectedX,selectedY);
                    beeplain(matrix,x,y,selectedX,selectedY);
                    stripes2(matrix,x,y,selectedX,selectedY);
                    bee(matrix,x,y);
                    bee(matrix,selectedX,selectedY);
                    stripes(matrix,selectedX,selectedY);
                    stripes(matrix,x,y);
                    plain(matrix,x,y,score);
                    plain(matrix,selectedX,selectedY,score);
                    selectedX = -1;
                    selectedY = -1;
                    moves--;
                }
                else
                {
                    selectedX = (e.button.x-430) / (Esize);
                    selectedY = (e.button.y-10) / (Esize);
                }
            }
            break;
            case SDL_QUIT:
                done = true;
                break;
            }
        }
        auto currentTick = SDL_GetTicks();
        for (auto t = oldTick; t < currentTick; ++t)
            if (!tick())
                return 1;
        oldTick = currentTick;
        if ( moves < 18 )
        draw();
        else
        {
            if ( score <= target )

                {
                    BaseObject fn;
                    fn.LoadImg("bg.png",renderer);
                    SDL_Rect fn1{0,0,1760,990};
                    fn.Render(0,0,renderer,&fn1);
                }
        }
        SDL_RenderPresent(renderer);
    }
    return 0;
}

bool PlayLayer::tick()
{
    if (ticks++ % 10 != 0)
        return true;
    if (createAndDropElement())
        return true;
    for ( int i = 0 ; i  < MATRIX_WIDTH ; i++ )
        for ( int j = 0 ; j < MATRIX_HEIGHT ; j++ )
        {
            bee(matrix,i,j);
            stripes(matrix,i,j);
            plain(matrix,i,j,score);
        }
    return true;
}


bool PlayLayer::createAndDropElement()
{
    // Create and drop
    bool hasHoles = false;
    for (auto y = MATRIX_HEIGHT - 2; y >= 0; --y)
    {
        for (auto x = 0; x < MATRIX_WIDTH; ++x)
        {
            if (matrix[x][y] >= 0 && matrix[x][y + 1] == -1)
            {
                for (auto yy = y; yy >= 0; --yy)
                {
                    if ( matrix[x][yy+1] != -10 && matrix[x][yy] != -10 )
                    {matrix[x][yy + 1] = matrix[x][yy];
                matrix[x][0] = -1;
                hasHoles = true;}
                }
            }
        }
    }
    for (auto x = 0; x < MATRIX_WIDTH; ++x)
        for ( auto y = 0 ; y < MATRIX_HEIGHT ; ++y )
        if (matrix[x][y] == -1)
            matrix[x][y] = rand() % 5;
    return hasHoles;

}

void PlayLayer::draw()
{

    for (int x = 0; x < MATRIX_WIDTH; ++x)
        for (int y = 0; y < MATRIX_HEIGHT; ++y)
        {
            if (matrix[x][y] >= 0)
            {
                if (x != selectedX || y != selectedY )
                {
                    BaseObject ss;
                    ss.LoadImg(elementNormal[matrix[x][y]],renderer);
                                SDL_Rect ss1={0,0,112,112};

                    ss.Render(Esize*x+430,Esize*y+10,renderer, &ss1);
                }
                else
                {
                    BaseObject ss;
                    SDL_Rect ss1={0,0,112,112};

                    ss.LoadImg(elementNormal[matrix[x][y]],renderer);
                    ss.Render(Esize*x+430,Esize*y+10, renderer,&ss1);
                    BaseObject border;
                    border.LoadImg("img/Other/border.png",renderer);
                    border.Render(Esize*x+430,Esize*y+10, renderer,&ss1);

                }
            }
        }
    TTF_Font *font = NULL;
    font = TTF_OpenFont("font/Cute Dino.ttf",64);
    BaseObject text;
    string t = std::to_string(score)  ;
    SDL_Color textColor = {206, 126, 0};
    text.LoadFromRenderedText(t,font,textColor,renderer);
    SDL_Rect ss2 = {0,0,10,10};
    text.Render(10, 650,renderer,&ss2);
    BaseObject Move;
    string s = std::to_string(moves);
    Move.LoadFromRenderedText(s,font,textColor,renderer);
    Move.Render(10,370,renderer,&ss2);
}

void PlayLayer::drawMatrix()
{
    BaseObject bg;
    SDL_Rect bg_rect;
    bg_rect={0,0,1880,1053};
    bg.LoadImg("img/background/bg2.png",renderer);
    bg.Render(0,0,renderer,&bg_rect);
    for (int x = 0; x < MATRIX_WIDTH; ++x)
        for (int y = 0; y < MATRIX_HEIGHT; ++y)
        {
            BaseObject ss;
            SDL_Rect ss1={0,0,112,112};
            if ( (x+y) %2 == 0)
            ss.LoadImg("img/Other/tile64_dark.png",renderer);
            else
            ss.LoadImg("img/Other/tile64_light.png",renderer);
                    if (square[x][y] >=0){
            ss.Render((Esize)*(x)+430,(Esize)*(y)+10,renderer,&ss1);}
        }
}

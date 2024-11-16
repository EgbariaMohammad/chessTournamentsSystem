#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "../includes/Game.h"

struct Game_t
{
    int player1ID;
    int player2ID;
    int winnerID;
    int playTime;
};

Game GameCreate(int player1ID, int player2ID)
{
    Game newGame = malloc(sizeof(*newGame));
    if(!newGame)
        return NULL;

    newGame->player1ID = player1ID;
    newGame->player2ID = player2ID;
    newGame->winnerID = -1;
    newGame->playTime = 0;
    return newGame;
}

void* GameCopy(void* g)
{
    Game game = (Game) g;
    if(!game) return NULL;
    Game newGame = GameCreate(game->player1ID, game->player2ID);
    if(!newGame) return NULL;
    return newGame;
}

void GameDestroy(void* g)
{
    Game game = (Game) g;
    if(!game) return;
    free(game);
}

void GameSetWinner(Game game, int winner) { game->winnerID = winner; }
void GameSetPlayTime(Game game , int playTime)  { game->playTime += playTime; }
int GameGetPlayTime(Game game)    { return game->playTime; }
int GameGetPlayer1ID(Game game)   { return game->player1ID; }
int GameGetPlayer2ID(Game game)   { return game->player2ID; }
int GameGetWinner(Game game)      { return game->winnerID; }




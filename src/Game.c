#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "../includes/Game.h"

/** Note:
 * Here I chose to implement the Game without including Player as a structural member
 * but instead saved the ID of the players in the game, because, if i would have saved players
 * as an object, the question the rises is, who should be responsible for deleting the player?
 * chessSystem or the Game ?
 */
struct Game_t
{
    int player1ID;
    int player2ID;
    int winnerID;
    int playTime;
};

Game GameCreate(int player1ID, int player2ID, int winnerID, int playTime)
{
    Game newGame = malloc(sizeof(*newGame));
    if(!newGame)
        return NULL;

    newGame->player1ID = player1ID;
    newGame->player2ID = player2ID;
    newGame->winnerID = winnerID;
    newGame->playTime = playTime;
    return newGame;
}

void* GameCopy(void* g)
{
    Game game = (Game) g;
    if(!game) return NULL;
    Game newGame = GameCreate(game->player1ID, game->player2ID, game->winnerID, game->playTime);
    if(!newGame) return NULL;
    return newGame;
}

void GameDestroy(void* g)
{
    Game game = (Game) g;
    if(!game) return;
    free(game);
}

bool GameIsPlayerInGame(Game game, int playerID)
{
    if(!game) return false;
    return (game->player1ID == playerID || game->player2ID == playerID);
}

int GameGetPlayTime(Game game)    { return game->playTime;  }
int GameGetPlayer1ID(Game game)   { return game->player1ID; }
int GameGetPlayer2ID(Game game)   { return game->player2ID; }
int GameGetWinner(Game game)      { return game->winnerID;  }




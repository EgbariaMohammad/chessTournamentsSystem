#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <math.h>

// #include "../utilities.h"
#include "../lib/Map.h"
#include "../includes/Player.h"
#include "../includes/Game.h"
#include "../includes/Tournament.h"
#include "../includes/chessSystem.h"

struct chess_system_t
{
    Map tournaments;
    Map players;
    int gamesNumber;
};

ChessSystem chessCreate()
{
    ChessSystem newSystem = malloc(sizeof(*newSystem));
    if(!newSystem)
        return NULL;

    newSystem->tournaments = mapCreate(TournamentCopy, copyIntkey, TournamentDestroy, freeIntKey, Intkeycompare);
    if(!newSystem->tournaments)
    {
        free(newSystem);
        return NULL;
    }

    newSystem->players = mapCreate(PlayerCopy, copyIntkey, PlayerDestroy, freeIntKey, Intkeycompare);
    if(!newSystem->players)
    {
        mapDestroy(newSystem -> tournaments);
        free(newSystem);
        return NULL;
    }

    newSystem->gamesNumber = 0;
    return newSystem;
}

void chessDestroy(ChessSystem chess)
{
    if(!chess)
        return;
    mapDestroy(chess->tournaments);
    mapDestroy(chess->players);
    free(chess);
}

ChessResult chessAddTournament(ChessSystem chess, int tournamentID, int maxGamesPerPlayer, const char* tournamentLocation)
{
    if(!chess || !tournamentLocation)                       return CHESS_NULL_ARGUMENT;
    else if(tournamentID <= 0)                              return CHESS_INVALID_ID;
    else if(mapContains(chess->tournaments, &tournamentID)) return CHESS_TOURNAMENT_ALREADY_EXISTS;
    else if(validName(tournamentLocation) == false)         return CHESS_INVALID_LOCATION;
    else if(maxGamesPerPlayer <= 0)                         return CHESS_INVALID_MAX_GAMES;

    Tournament newTournament = tournamentCreate(tournamentID, maxGamesPerPlayer, tournamentLocation);
    if(!newTournament)
    {
        chessDestroy(chess);
        return CHESS_OUT_OF_MEMORY;
    }

    if(mapPut(chess->tournaments, &tournamentID, newTournament) == MAP_OUT_OF_MEMORY)
    {
        tournamentDestroy(newTournament);
        chessDestroy(chess);
        return CHESS_OUT_OF_MEMORY;
    }
    tournamentDestroy(newTournament);
    return CHESS_SUCCESS;
}

ChessResult chessRemoveTournament(ChessSystem chess, int tournamentID)
{
    if(!chess)                       return CHESS_NULL_ARGUMENT;
    else if(tournamentID <= 0 )      return CHESS_INVALID_ID;
    else if(mapContains(chess->tournaments, &tournamentID) == false) 
        return CHESS_TOURNAMENT_NOT_EXIST;

    Tournament toDelete = mapGet(chess->tournaments, &tournamentID);
    Map gamesList = tournamentGetGamesList(toDelete);
    MAP_FOREACH(const char*, currentGameKey, gamesList)
    {
        Game game = mapGet(gamesList, (MapKeyElement) currentGameKey);
        int firstPlayerID = GameGetPlayer1ID(game);
        int secondPlayerID = GameGetPlayer2ID(game);
        Player player1 = mapGet(chess->players, &firstPlayerID);
        Player player2 = mapGet(chess->players, &secondPlayerID);
        INFO status = REMOVE;
        updatePlayersInfo(player1, player2, GameGetWinner(game), GameGetPlayTime(game), status);
        freeChar((MapKeyElement) currentGameKey);
    }
    mapRemove(chess->tournaments, &tournamentID);
    return CHESS_SUCCESS;
}
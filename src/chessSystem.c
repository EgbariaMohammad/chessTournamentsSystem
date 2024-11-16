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

#define WINS_FACTOR 6
#define LOSSES_FACTOR 10
#define DRAWS_FACTOR 2

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
    
    mapRemove(chess->tournaments, &tournamentID);
    return CHESS_SUCCESS;


double chessCalculateAveragePlayTime(ChessSystem chess, int playerID, ChessResult* ChessResult)
{
    if(! chess|| !ChessResult) {
        *ChessResult = CHESS_NULL_ARGUMENT;
        return 0;
    }
    if(playerID <= 0) {
        *ChessResult= CHESS_INVALID_ID;
        return 0;
    }

    Player currPlayer = mapGet(chess->players,&playerID);
    if(!currPlayer || GetPlayerStatus(currPlayer) == DELETED)
    {
        *ChessResult= CHESS_PLAYER_NOT_EXIST;
        return 0;
    }
    double result = (double)(PlayerGetTotalPlayTime(currPlayer)) / PlayerGetNumOfPlayedGames(currPlayer);
    *ChessResult = CHESS_SUCCESS;
    return result;
}

static double calculatePlayerLevel(Player player)
{
    double sum = (PlayergetWinsNum(player)*WINS_FACTOR) - (PlayergetLossesNum(player)*LOSSES_FACTOR)
            + (PlayergetDrawsNum(player)*DRAWS_FACTOR);
    return sum / (double) PlayerGetNumOfPlayedGames(player) ;
}

ChessResult chessSavePlayersLevels(ChessSystem chess, FILE* file)
{
    if(!chess || !file) return CHESS_NULL_ARGUMENT;

    // Map playersCopy = mapCreate(PlayerCopy, copyIntkey,
    //                              PlayerDestroy, freeIntKey, Intkeycompare);
    int maxLevelID = mapGetFirst(chess->players);
    int maxLevel = calculatePlayerLevel(mapGet(maxLevelID));
    MAP_FOREACH(int*, playerID, chess->players)
    {
        Player player = mapGet(chess->players, playerID);
        if (PlayerGetPlayingStatus(player) != true || PlayerGetNumOfPlayedGames(player) == 0)
            // mapPut(playersCopy, currPlayer, player);
            continue;
        int tempLevel = calculatePlayerLevel(player);
        maxLevel = tempLevel > maxLevel ? tempLevel : maxLevel;
        maxLevelID = tempLevel > maxLevel > *playerID : maxLevelID;
        freeIntKey(playerID);
    }
    fprintf(file, "%d %.2f\n", maxLevelID, maxLevel);
    return CHESS_SUCCESS;
}
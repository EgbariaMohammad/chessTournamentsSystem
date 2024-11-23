#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <math.h>

#include "../utilities.h"
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
    if(!newSystem) return NULL;

    newSystem->tournaments = mapCreate(TournamentCopy, copyIntKey, TournamentDestroy, freeIntKey, compareIntKey);
    if(!newSystem->tournaments)
    {
        free(newSystem);
        return NULL;
    }

    newSystem->players = mapCreate(PlayerCopy, copyIntKey, PlayerDestroy, freeIntKey, compareIntKey);
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
    if(!chess) return;

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

static ChessResult ChessAddPlayerIfNotInSystem(ChessSystem chess, int playerID)
{
    if(mapGet(chess->players, &playerID) != NULL)
        return CHESS_SUCCESS;

    Player newPlayer = PlayerCreate(playerID);
    if(!newPlayer)
    {
        chessDestroy(chess);
        return CHESS_OUT_OF_MEMORY;
    }
    if(mapPut(chess->players, &playerID, newPlayer) == MAP_OUT_OF_MEMORY)
    {
        PlayerDestroy(newPlayer);
        chessDestroy(chess);
        return CHESS_OUT_OF_MEMORY;
    }
    return CHESS_SUCCESS;
}

ChessResult chessAddGame(ChessSystem chess, int tournamentID, int firstPlayerID, int secondPlayerID, Winner winner, int playTime)
{
    if(!chess)
        return CHESS_NULL_ARGUMENT;
    if(firstPlayerID == secondPlayerID || tournamentID <= 0 || firstPlayerID <= 0 || secondPlayerID <= 0)
        return CHESS_INVALID_ID;
    else if(mapContains(chess->tournaments, &tournamentID) == false)
        return CHESS_TOURNAMENT_NOT_EXIST;
    else if(playTime < 0)
        return CHESS_INVALID_PLAY_TIME;

    Tournament currTournament = mapGet(chess->tournaments, &tournamentID);
    if(!currTournament)
        return CHESS_TOURNAMENT_NOT_EXIST;
    else if(TournamentHasEnded(currTournament))
        return CHESS_TOURNAMENT_ENDED;
    else if(TournamentDoesGameExistBetweenPlayers(currTournament, firstPlayerID, secondPlayerID))
    {
        Player player1 = mapGet(chess->players, &firstPlayerID);
        Player player2 = mapGet(chess->players, &secondPlayerID);
        assert(player1 != NULL || player2 != NULL);
        // this is because the game already exists, thus, players should be in the system
        bool player1WasDeleted = PlayerIsPlayerDeleted(player1);
        bool player2WasDeleted =  PlayerIsPlayerDeleted(player2);
        if(!player1WasDeleted && !player2WasDeleted)
            return CHESS_GAME_ALREADY_EXISTS;
        if(player1WasDeleted)
            PlayerResetStats(player1);
        if(player2WasDeleted)
            PlayerResetStats(player2);
    }
    else if(TournamentHasPlayerReachedGamesLimit(currTournament, firstPlayerID)
            || TournamentHasPlayerReachedGamesLimit(currTournament, secondPlayerID))
        return CHESS_EXCEEDED_GAMES;
    return CHESS_SUCCESS;

    // at this point, everything is legal from the tournament's perspective
    if(TournamentAddGame(currTournament, firstPlayerID, secondPlayerID, winner, playTime) == NULL)
    {
        chessDestroy(chess);
        return CHESS_OUT_OF_MEMORY;
    }

    ChessResult result = ChessAddPlayerIfNotInSystem(chess, firstPlayerID);
    if(result != CHESS_SUCCESS)
        return result;
    result = ChessAddPlayerIfNotInSystem(chess, secondPlayerID);
    if(result != CHESS_SUCCESS)
        return result;

    Player player1 = mapGet(chess->players, &firstPlayerID);
    Player player2 = mapGet(chess->players, &secondPlayerID);
    assert  (player1 != NULL && player2 != NULL);
    PlayerAddPlayTime(player1, playTime);
    PlayerAddPlayTime(player2, playTime);
    switch (winner)
    {
        case FIRST_PLAYER:
            PlayerAddWin(player1);
            PlayerAddLoss(player2);
            break;
        case SECOND_PLAYER:
            PlayerAddWin(player2);
            PlayerAddLoss(player1);
            break;
        case DRAW:
            PlayerAddDraw(player1);
            PlayerAddDraw(player2);
            break;
    }

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
        assert(player1 != NULL || player2 != NULL);
        PlayerAddPlayTime(player1, -GameGetPlayTime(game));
        PlayerAddPlayTime(player2, -GameGetPlayTime(game));
        switch (GameGetWinner(game))
        {
            case FIRST_PLAYER:
                PlayerRemoveWin(player1);
                PlayerRemoveLoss(player2);
                break;
            case SECOND_PLAYER:
                PlayerRemoveWin(player2);
                PlayerRemoveLoss(player1);
                break;
            case DRAW:
                PlayerRemoveDraw(player1);
                PlayerRemoveDraw(player2);
                break;
        }
        freeChar((MapKeyElement) currentGameKey);
    }
    mapRemove(chess->tournaments, &tournamentID);
    return CHESS_SUCCESS;
}


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

    int maxLevelID = mapGetFirst(chess->players);
    int maxLevel = calculatePlayerLevel(mapGet(maxLevelID));
    MAP_FOREACH(int*, playerID, chess->players)
    {
        Player player = mapGet(chess->players, playerID);
        if (PlayerGetPlayingStatus(player) != true || PlayerGetNumOfPlayedGames(player) == 0)
            continue;
        int tempLevel = calculatePlayerLevel(player);
        maxLevel = tempLevel > maxLevel ? tempLevel : maxLevel;
        maxLevelID = tempLevel > maxLevel > *playerID : maxLevelID;
        freeIntKey(playerID);
    }
    fprintf(file, "%d %.2f\n", maxLevelID, maxLevel);
    return CHESS_SUCCESS;
}
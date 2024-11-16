#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <math.h>
#include "../include/chessSystem.h"
#include "../lib/Map.h"
#include "../include/Players.h"
#include "../include/Games.h"
#include "../include/Tournament.h"
#include "../utilities.h"


typedef enum {
    ADD,
    REMOVE
} INFO;



ChessSystem chessCreate()
{
    ChessSystem newSystem = malloc(sizeof(*newSystem));
    if(!newSystem)
        return NULL;

    newSystem->tournaments = mapCreate(copyTournamentData, copyIntkey, freeTournamentData, freeIntKey, Intkeycompare);
    if(!newSystem->tournaments)
    {
        free(newSystem);
        return NULL;
    }

    newSystem->players = mapCreate(copyPlayerData, copyIntkey, freePlayerData, freeIntKey, Intkeycompare);
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

// ADD TOURNAMENT FUNCTIONS
static bool validName(const char* tournament_location)
{
    int length = (int) strlen(tournament_location);
    for (int i = 0; i < length; i++)
    {
        if(tournament_location[i] > 'z' || tournament_location[i] < 'a')
        {
            if(tournament_location[i] != ' ')
            {
                return false;
            }
        }
    }
    return true;
}

ChessResult chessAddTournament(ChessSystem chess, int tournamentID, int maxGamesPerPlayer, const char* tournamentLocation)
{
    if(!chess || !tournamentLocation)
        return CHESS_NULL_ARGUMENT;

    if(tournamentID <= 0)
        return CHESS_INVALID_ID;

    if(mapContains(chess->tournaments, &tournamentID))
        return CHESS_TOURNAMENT_ALREADY_EXISTS;

    if(validName(tournamentLocation) == false)
        return CHESS_INVALID_LOCATION;

    if(maxGamesPerPlayer <= 0)
        return CHESS_INVALID_MAX_GAMES;

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

static void updatePlayersInfo(Player player1, Player player2, Winner  winner, int playTime, INFO status)
{
    int update = 1;
    if(status == REMOVE)
        update = -1;

    assert(player1 != NULL && player2 != NULL);

    PlayersetNumOfGames(player1, update);
    PlayersetTotalTime(player1, update * playTime);
    PlayersetNumOfGames(player2, update);
    PlayersetTotalTime(player2, update * playTime);

    if(winner == DRAW)
    {
        PlayersetDrawsNum(player1, update);
        PlayersetDrawsNum(player2, update);
    }
    else if(winner == FIRST_PLAYER)
    {
        PlayersetWinsNum(player1, update);
        PlayersetLossesNum(player2, update);
    }
    else
    {
       PlayersetLossesNum(player1, update);
       PlayersetWinsNum(player2, update);
    }
}

// CHESS ADD GAME FUNCTIONS
static int numLength(int num)
{
    int i=0;
    while(num > 0)
    {
        i++;
        num /= 10;
    }
    return i;
}

static void fillStrKey(char* str, int num_to_add, int length)
{

    for(int i = length-1; i>=0; i--)
    {
        str[i] = '0' + num_to_add % 10;
        num_to_add /= 10;
    }
}

static char* makeKeyAnStr(int player1_id, int player2_id)
{
    int length1 = numLength(player1_id);
    int length2 = numLength(player2_id);
    int length = length1 + length2;

    char* keymade = malloc(sizeof(char)*(length+2));
    if(keymade == NULL)
    {
        return NULL;
    }
    keymade[length1] = '|';
    keymade[length+1] = 0;
    fillStrKey(keymade, player1_id, length1);
    fillStrKey(keymade + length1 + 1, player2_id, length2);
    return keymade;
}



static int checkNumOfParticaption(Map games, Player player)
{
    if(GetPlayerStatus(player) == DELETED)
        return 0;
    int player_id = PlayergetPlayerID(player);
    int counter = 0;
    MAP_FOREACH(char*, curr_game, games)
    {
        Game game = mapGet(games, curr_game);
        if(GameGetPlayer1ID(game) == player_id || GameGetPlayer2ID(game) == player_id)
        {
            counter++;
        }
        freeChar(curr_game);
    }
    return counter;
}

static void clearPlayer(Player player)
{
    int wins_to_reset = -1*PlayergetWinsNum(player);
    PlayersetWinsNum(player, wins_to_reset);

    int draws_to_reset = -1*PlayergetDrawsNum(player);
    PlayersetDrawsNum(player, draws_to_reset);

    int loses_to_reset = -1*PlayergetLossesNum(player);
    PlayersetLossesNum(player, loses_to_reset);

    int time_to_reset = -1*PlayergetTotalTime(player);
    PlayersetTotalTime(player, time_to_reset);

    int games_to_reset = -1*PlayergetNumOfGames(player);
    PlayersetNumOfGames(player, games_to_reset);

    SetPlayerStatus(player, STILL_IN);
}


ChessResult chessAddGame(ChessSystem chess, int tournamentID, int firstPlayerID, int secondPlayerID, Winner winner, int playTime)
{
    if(!chess)
        return CHESS_NULL_ARGUMENT;

    if(firstPlayerID == secondPlayerID || tournamentID <= 0 || firstPlayerID <= 0 || secondPlayerID <= 0)
        return CHESS_INVALID_ID;

    if(mapContains(chess->tournaments, &tournamentID) == false)
        return CHESS_TOURNAMENT_NOT_EXIST;

    if(playTime <= 0)
        return CHESS_INVALID_PLAY_TIME;

    Tournament currTournament = mapGet(chess->tournaments, &tournamentID);
    Map gamesList = tournamentGetGamesList(currTournament);
    if(tournamentGetStatus(currTournament) == ENDED)
        return CHESS_TOURNAMENT_ENDED;

    char* key1 = makeKeyAnStr(firstPlayerID, secondPlayerID);
    char* key2 = makeKeyAnStr(secondPlayerID, firstPlayerID);
    bool Is_Player1_IN = mapContains(chess->players, &firstPlayerID);
    bool Is_Player2_IN = mapContains(chess->players, &secondPlayerID);

    bool player1_deleted = false;
    bool player2_deleted = false;

        if( Is_Player1_IN || Is_Player2_IN )
        {
            Player player1 = mapGet(chess->players, &firstPlayerID);
            Player player2 = mapGet(chess->players, &secondPlayerID);

            if( ( mapContains(gamesList, key1) || mapContains(gamesList, key2)) &&
               (Is_Player1_IN && GetPlayerStatus(player2) != DELETED) &&
               (Is_Player2_IN && GetPlayerStatus(player1) != DELETED)  )
            {
                free(key2);
                free(key1);
                return CHESS_GAME_ALREADY_EXISTS;
            }

            if( (Is_Player1_IN && checkNumOfParticaption(gamesList, player1)+1 > tournamentGetMaxGamesP(currTournament) )
               || (Is_Player2_IN && checkNumOfParticaption(gamesList, player2)+1 > tournamentGetMaxGamesP(currTournament)) )
            {
                free(key2);
                free(key1);
                return CHESS_EXCEEDED_GAMES;
            }


            if((player1 != NULL && GetPlayerStatus(player1) == DELETED) )
            {
                player1_deleted = true;
                clearPlayer(player1);
            }

            if((player2 != NULL && GetPlayerStatus(player2) == DELETED))
            {
                player2_deleted = true;
                clearPlayer(player2);
            }
        }

    int to_add = 1;
    if(Is_Player1_IN == false)
    {
        Player player1 = playerCreate(firstPlayerID);
        if(mapPut(chess->players, &firstPlayerID, player1) == MAP_OUT_OF_MEMORY)
        {
            free(key2);
            free(key1);
            chessDestroy(chess);
            return CHESS_OUT_OF_MEMORY;
        }

        playerDestroy(player1);
    }

    if(Is_Player2_IN == false)
    {
        Player player2 = playerCreate(secondPlayerID);
        if(mapPut(chess->players, &secondPlayerID, player2) == MAP_OUT_OF_MEMORY)
        {
            free(key2);
            free(key1);
            chessDestroy(chess);
            return CHESS_OUT_OF_MEMORY;
        }
        playerDestroy(player2);

    }

    Game new_game = GameCreate(firstPlayerID, secondPlayerID, winner, playTime);
    if(new_game == NULL)
    {
        free(key2);
        free(key1);
        chessDestroy(chess);
        return CHESS_OUT_OF_MEMORY;
    }

    Map games = tournamentGetGamesList(currTournament);
    if(mapPut(games, key1, new_game) == MAP_OUT_OF_MEMORY) // switch the data
    {
        free(key2);
        free(key1);
        GameDestroy(new_game);
        chessDestroy(chess);
        return CHESS_OUT_OF_MEMORY;
    }

    free(key2);
    free(key1);

    INFO status = ADD;
    updatePlayersInfo(mapGet(chess->players, &firstPlayerID), mapGet(chess->players, &secondPlayerID), winner, playTime, status);

    chess->gamesNumber += 1;
    tournamentSetNumOfGames(currTournament);
    tournamentSetTotalTime(currTournament, playTime);

    if(playTime > tournamentGetMaxTime(currTournament))
        tournamentSetMaxTime(currTournament, playTime);

    Player player1 = mapGet(chess->players, &firstPlayerID);
    Player player2 = mapGet(chess->players, &secondPlayerID);

    if( mapContains(playerGetTournamentsList(player1), &tournamentID) )
    {
        if(player1_deleted)
        {
            int* num_of_adds = mapGet(playerGetTournamentsList(player1), &tournamentID);
            *num_of_adds += 1;
            mapPut(playerGetTournamentsList(player1), &tournamentID, num_of_adds);
        }
    }
    else
    {
         mapPut(playerGetTournamentsList(player1), &tournamentID, &to_add);
    }

    if(mapContains(playerGetTournamentsList(player2), &tournamentID) )
    {
        if(player2_deleted)
        {
            int* num_of_adds= mapGet(playerGetTournamentsList(player2), &tournamentID);
            *num_of_adds += 1;
            mapPut(playerGetTournamentsList(player2), &tournamentID, num_of_adds);
        }
    }
    else
    {
         mapPut(playerGetTournamentsList(player2), &tournamentID, &to_add);
    }

    bool Is_Player1_IN_T = mapContains(tournamentGetPlayersList(currTournament), &firstPlayerID);
    bool Is_Player2_IN_T = mapContains(tournamentGetPlayersList(currTournament), &secondPlayerID);

    Map player_in_t = tournamentGetPlayersList(currTournament);

    if(Is_Player1_IN_T == false)
    {
        Player player1 = playerCreate(firstPlayerID);
        if(mapPut(player_in_t, &firstPlayerID, player1) == MAP_OUT_OF_MEMORY)
        {
            chessDestroy(chess);
            return CHESS_OUT_OF_MEMORY;
        }
        playerDestroy(player1);
    }

    if(Is_Player2_IN_T == false)
    {
        Player player2 = playerCreate(secondPlayerID);
        if(mapPut(player_in_t, &secondPlayerID, player2) == MAP_OUT_OF_MEMORY)
        {
            chessDestroy(chess);
            return CHESS_OUT_OF_MEMORY;
        }
        playerDestroy(player2);
    }

    status = ADD;
    updatePlayersInfo(mapGet(player_in_t, &firstPlayerID), mapGet(player_in_t, &secondPlayerID), winner, playTime, status);

    GameDestroy(new_game);
    return CHESS_SUCCESS;
}


// DELETE AN EXISITING TOURNAMENT FUNCTIOIN
ChessResult chessRemoveTournament(ChessSystem chess, int tournamentID)
{
    if(!chess)
        return CHESS_NULL_ARGUMENT;

    if(tournamentID <= 0 )
        return CHESS_INVALID_ID;

    if(mapContains(chess->tournaments, &tournamentID) == false)
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

static void changeGame(ChessSystem chess, Map games_list, int player_id, char* game_key)
{
    Game game = mapGet(games_list, game_key);
    Player player_to_delete = mapGet(chess->players, &player_id);

    int player1_id = GameGetPlayer1ID(game);
    int player2_id = GameGetPlayer2ID(game);
    int num = numLength(player1_id);

    if(game_key[0] == '0' || game_key[num+1] == '0')
        return;

    if(player1_id == player_id)
    {
        int p2_id = GameGetPlayer2ID(game);
        Player p2 = mapGet(chess->players, &p2_id);
        updatePlayersInfo(player_to_delete, p2, GameGetWinner(game), 0, REMOVE);
        GameSetWinner(game, SECOND_PLAYER);
        updatePlayersInfo(player_to_delete, p2, GameGetWinner(game), 0, ADD);
    }

    if(player2_id == player_id)
    {
        int p2_id = GameGetPlayer1ID(game);
        Player p2 = mapGet(chess->players, &p2_id);

        updatePlayersInfo(p2, player_to_delete, GameGetWinner(game), 0, REMOVE);
        GameSetWinner(game, FIRST_PLAYER);
        updatePlayersInfo(p2, player_to_delete, GameGetWinner(game),0, ADD);
    }
}

static bool isPlayerIngame(Game game, int player_id)
{
    return (GameGetPlayer1ID(game) == player_id || GameGetPlayer2ID(game) == player_id);
}


ChessResult chessRemovePlayer(ChessSystem chess, int player_id)
{
    if(!chess)
        return CHESS_NULL_ARGUMENT;

    if(player_id <= 0)
        return CHESS_INVALID_ID;

    Player player = mapGet(chess->players, &player_id);
    if(!player || GetPlayerStatus(player) == DELETED)
        return CHESS_PLAYER_NOT_EXIST;

    MAP_FOREACH(int*, tournaement_id , chess->tournaments)
    {
        Tournament tournament = mapGet(chess->tournaments, tournaement_id);
        if(tournamentGetStatus(tournament) == ENDED)
        {
            freeIntKey(tournaement_id);
            continue;
        }
        Map games = tournamentGetGamesList(tournament);
        MAP_FOREACH(char*, curr_game, games)
        {
            Game game_to_change = mapGet(games, (MapKeyElement) curr_game);
            if(isPlayerIngame(game_to_change, player_id) == false)
            {
                freeChar((MapKeyElement) curr_game);
                continue;
            }
            changeGame(chess, games, player_id, curr_game);
            // mapRemove(games, curr_game);
            freeChar((MapKeyElement) curr_game);
        }
        freeIntKey(tournaement_id);
    }

    SetPlayerStatus(player, DELETED);
    return CHESS_SUCCESS;
}

static int getMax(Map curr_players)
{
    int* firstkey = mapGetFirst(curr_players);
    double* max_level = mapGet(curr_players, firstkey);
    freeIntKey(firstkey);

    MAP_FOREACH(int*, curr_player_id, curr_players)
    {
        double* level = mapGet(curr_players, curr_player_id);
        if(*level > *max_level)
        {
            *max_level = *level;
        }
        freeIntKey(curr_player_id);
    }
    return *max_level;
}

static double calculatePoints(Player player)
{
    return (double)(PlayergetWinsNum(player)*2 + PlayergetDrawsNum(player));
}

ChessResult chessEndTournament(ChessSystem chess, int tournament_id)
{
    if(chess == NULL)
    {
        return  CHESS_NULL_ARGUMENT;
    }

    if(tournament_id <= 0)
    {
        return CHESS_INVALID_ID;
    }


    Tournament curr_tournament = mapGet(chess->tournaments, &tournament_id);
    if(curr_tournament == NULL)
    {
        return CHESS_TOURNAMENT_NOT_EXIST;
    }

    if(tournamentGetStatus(curr_tournament) == ENDED)
    {
        return CHESS_TOURNAMENT_ENDED;
    }

    Map games_list = tournamentGetGamesList(curr_tournament);
    if(mapGetSize(games_list) == 0)
    {
        return CHESS_NO_GAMES;
    }

    Map players_in_tournament = mapCreate(copyPlayerData, copyIntkey, freePlayerData, freeIntKey, Intkeycompare);

    MAP_FOREACH(char*, curr_game, games_list)
    {
        Game game = mapGet(games_list, (MapKeyElement) curr_game);
        int player1_id = GameGetPlayer1ID(game);
        int player2_id = GameGetPlayer2ID(game);
        Player player1 = playerCreate(player1_id);
        Player player2 = playerCreate(player2_id);
        if( !mapContains(players_in_tournament, &player1_id) )
        {
            mapPut(players_in_tournament, &player1_id, player1);
        }
        if( !mapContains(players_in_tournament, &player2_id) )
        {
            mapPut(players_in_tournament, &player2_id, player2);
        }
        playerDestroy(player1);
        playerDestroy(player2);
        freeChar(curr_game);
    }

    MAP_FOREACH( char*, curr_game, games_list)
    {
        Game game = mapGet(games_list, (MapKeyElement) curr_game);
        int player1_id = GameGetPlayer1ID(game);
        int player2_id = GameGetPlayer2ID(game);
        INFO status = ADD;
        updatePlayersInfo(mapGet(players_in_tournament, &player1_id),
             mapGet(players_in_tournament, &player2_id), GameGetWinner(game),
              GameGetPlayTime(game), status);
        freeChar(curr_game);
    }

    MAP_FOREACH(int*, player_id, chess->players)
    {
        Player player = mapGet(chess->players, player_id);
        if(GetPlayerStatus(player) == DELETED)
        {
            Player player_tournament=mapGet(players_in_tournament,player_id);
            SetPlayerStatus(player_tournament,DELETED);
        }
        freeIntKey(player_id);
    }

    Map players_points = mapCreate(copyDoubleData, copyIntkey, freeDoubleData, freeIntKey, Intkeycompare);

    MAP_FOREACH(int*, curr_player_id, players_in_tournament)
    {
        Player player = mapGet(players_in_tournament, curr_player_id);
        double points = calculatePoints(player);
        mapPut(players_points, curr_player_id, &points);
        freeIntKey( (MapKeyElement) curr_player_id);
    }

    double max_level = getMax(players_points);

    MAP_FOREACH(int*, curr_player_id, players_points)
    {
        double* curr_player_level = mapGet(players_points, curr_player_id);
        Player player = mapGet(players_in_tournament, curr_player_id);
        if( max_level > *curr_player_level && GetPlayerStatus(player) != DELETED)
        {
            // mapRemove(players_points, curr_player_id);
            SetPlayerStatus(player, DELETED);

        }
        freeIntKey(curr_player_id);
    }

    if(mapGetSize(players_points) <= 1)
    {
        assert(mapGetSize(players_points) != 0);
        int* id = mapGetFirst(players_points);
        tournamentSetWinnerID(curr_tournament, *id);
        freeIntKey(id);
        mapDestroy(players_points);
        mapDestroy(players_in_tournament);
        tournamentSetStatus(curr_tournament, ENDED);
        return CHESS_SUCCESS;
    }

    bool first_player = true;
    int loses = 0;
    int min_loses_id = 0;

    MAP_FOREACH(int*, curr_player_id, players_points)
    {
        Player player = mapGet(players_in_tournament, curr_player_id);
        int num_of_loses = PlayergetLossesNum(player);
        if(first_player == true && GetPlayerStatus(player) != DELETED)
        {
            loses = num_of_loses;
            first_player = false;
            min_loses_id = *curr_player_id;
            freeIntKey(curr_player_id);
            continue;
        }
        if(num_of_loses < loses && GetPlayerStatus(player) != DELETED)
        {
            loses = num_of_loses;
            mapRemove(players_points,  &min_loses_id);
            min_loses_id = *curr_player_id;
        }
        freeIntKey(curr_player_id);
    }

    if(mapGetSize(players_points) <= 1)
    {
        assert(mapGetSize(players_points) != 0);
        int* id = mapGetFirst(players_points);
        tournamentSetWinnerID(curr_tournament, *id);
        freeIntKey(id);
        mapDestroy(players_points);
        mapDestroy(players_in_tournament);
        tournamentSetStatus(curr_tournament, ENDED);
        return CHESS_SUCCESS;
    }

    first_player = true;
    int wins = 0;
    int min_wins_id = 0;

    MAP_FOREACH(int*, curr_player_id, players_points)
    {
        Player player = mapGet(players_in_tournament, curr_player_id);
        int num_of_wins = PlayergetWinsNum(player);
        if(first_player == true && GetPlayerStatus(player) != DELETED)
        {
            wins = num_of_wins;
            first_player = false;
            min_wins_id = *curr_player_id;
            freeIntKey(curr_player_id);
            continue;
        }
        if(num_of_wins > wins && GetPlayerStatus(player) != DELETED)
        {
            wins = num_of_wins;
            mapRemove(players_points,  &min_wins_id);
            min_wins_id = *curr_player_id;
        }
        freeIntKey(curr_player_id);
    }

    if(mapGetSize(players_points) <= 1)
    {
        assert(mapGetSize(players_points) != 0);
        int* id = mapGetFirst(players_points);
        tournamentSetWinnerID(curr_tournament, *id);
        freeIntKey(id);
        mapDestroy(players_points);
        mapDestroy(players_in_tournament);
        tournamentSetStatus(curr_tournament, ENDED);
        return CHESS_SUCCESS;
    }

    // IF WE GET HERE WE CAN'T FIND A WINNER

    int min_id = 0;
    first_player = true;
    MAP_FOREACH(int*, curr_player_id, players_points)
    {
        Player player = mapGet(players_in_tournament, curr_player_id);
        if(first_player == true && GetPlayerStatus(player) != DELETED)
        {
            min_id = *curr_player_id;
            first_player=false;
            freeIntKey(curr_player_id);
            continue;
        }
        if(*curr_player_id < min_id && GetPlayerStatus(player) != DELETED)
        {
            min_id = *curr_player_id;
        }
        freeIntKey(curr_player_id);
    }

    tournamentSetWinnerID(curr_tournament, min_id);
    tournamentSetStatus(curr_tournament, ENDED);
    mapDestroy(players_points);
    mapDestroy(players_in_tournament);
    return CHESS_SUCCESS;

}


double chessCalculateAveragePlayTime (ChessSystem chess, int player_id, ChessResult* chess_result)
{
    if(chess==NULL || chess_result == NULL)
    {
        *chess_result= CHESS_NULL_ARGUMENT;
        return 0;
    }
    if(player_id <= 0)
    {
        *chess_result= CHESS_INVALID_ID;
        return 0;
    }

    Player curr_player = mapGet(chess->players,&player_id);
    if(curr_player == NULL || GetPlayerStatus(curr_player) == DELETED)
    {
        *chess_result= CHESS_PLAYER_NOT_EXIST;
        return 0;
    }
    double result = (double)(PlayergetTotalTime(curr_player)) / PlayergetNumOfGames(curr_player);
    *chess_result = CHESS_SUCCESS;
    return result;
}


//  NEEDS TO BE CHANGED;
// SAVE PLAYERS LEVEL AND STATICS

static double calculateLevel(Player player)
{
    double sum =  (double) ( (PlayergetWinsNum(player)*6) -
                        (10*PlayergetLossesNum(player)) + (2* PlayergetDrawsNum(player)) );
    return sum / (double) PlayergetNumOfGames(player) ;
}

ChessResult chessSavePlayersLevels(ChessSystem chess, FILE* file)
{
    if(chess == NULL || file == NULL)
    {
        return CHESS_NULL_ARGUMENT;
    }

    Map players_copy = mapCreate(copyPlayerData, copyIntkey,
                                 freePlayerData, freeIntKey, Intkeycompare);
    MAP_FOREACH(int*, curr_player, chess->players)
    {
        Player player = mapGet(chess->players, curr_player);
        if ( GetPlayerStatus(player) == STILL_IN && PlayergetNumOfGames(player) != 0)
        {
            mapPut(players_copy,curr_player,player);
        }
        freeIntKey( (MapKeyElement) curr_player);
    }

    while(mapGetSize(players_copy)> 0)
    {
        int* firstkey = mapGetFirst(players_copy);
        Player player = mapGet(players_copy, firstkey);
        double level = calculateLevel(player);
        double max_level = level;
        int max_level_id = *firstkey;
        freeIntKey(firstkey);

        MAP_FOREACH(int*, curr_player, players_copy)
        {
            if(mapGetSize(players_copy) == 1)
            {
                freeIntKey(curr_player);
                break;
            }
            player = mapGet(players_copy, curr_player);
            level = calculateLevel(player);
            if(level > max_level )
            {
                max_level = level;
                max_level_id = *curr_player;
            }
            freeIntKey(curr_player);
        }
        fprintf(file, "%d %.2f\n", max_level_id, max_level);
        mapRemove(players_copy, &max_level_id);
    }
    mapDestroy(players_copy);
    return CHESS_SUCCESS;
}

// CALCULATE AND SAVE STATISTICS OF TOURNAMENTS THAT HAVE ENDED
ChessResult chessSaveTournamentStatistics (ChessSystem chess, char* path_file)
    {
    if(chess==NULL)
    {
        return CHESS_NULL_ARGUMENT;
    }

    FILE* file = fopen(path_file, "w");
    if(file == NULL)
    {
        return  CHESS_SAVE_FAILURE;
    }

    int count_ended=0;

    MAP_FOREACH(int*, curr_tournament, chess->tournaments)
    {
        Tournament tournament = mapGet(chess->tournaments, curr_tournament);
        double num_of_games = tournamentGetNumOfGames(tournament);
        double total_time = tournamentGetTotalTime(tournament);
        if(tournamentGetStatus(tournament) == ENDED)
        {
            count_ended++;
            fprintf(file,"%d\n", tournamentGetWinnerID(tournament));
            int max_time = tournamentGetMaxTime(tournament);
            double avg = total_time/num_of_games ;
            fprintf(file,"%d\n", max_time);
            fprintf(file,"%.2f\n", avg);
            fprintf(file,"%s\n",  tournamentGetlocation(tournament));
            fprintf(file,"%d\n", (int) num_of_games);
            int num_of_players_in_tournament = 0;
            MAP_FOREACH(int*, curr_player, chess->players)
            {
                Player player = mapGet(chess->players, curr_player);
                MAP_FOREACH(int*, curr_player_tournament, playerGetTournamentsList(player))
                {
                    if(*curr_player_tournament == *curr_tournament)
                    {
                         int* num_of_adds = mapGet(playerGetTournamentsList(player), curr_player_tournament);
                         num_of_players_in_tournament += *num_of_adds;
                    }
                    freeIntKey(curr_player_tournament);
                }
                freeIntKey(curr_player);
            }
            fprintf(file, "%d\n", num_of_players_in_tournament);
        }
        freeIntKey(curr_tournament);
    }
    if(count_ended == 0)
    {
        fclose(file);
        return CHESS_NO_TOURNAMENTS_ENDED;
    }
    fclose(file);
    return CHESS_SUCCESS;
}

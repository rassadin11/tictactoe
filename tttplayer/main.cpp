/**
 *  This is example of game, that you can use for debugging.
 */
#include <cstdlib>
#include <ctime>
#include <iostream>

#include "my_player.h"
#include "random_bot.h"
#include "game_engine.h"

class A {
    static int ma;
public:
    A();
};
 
int main() {
    srand(time(NULL));

    extern int x;
    extern int o;
    extern int mov;

    /// Creates observer to log every event to std::cout
    BasicObserver obs(std::cout);
    DebugObserver debugObs(std::cout);

    /// Create setting for game
    GameSettings settings = {
        .field_size = {
            .min = {.x = -19, .y = -19},
            .max = {.x = 20, .y = 20},
        },
        .max_moves = 0,
        .win_length = 5,
        .move_timelimit_ms = 0,
    };

    for (int i = 0; i < 10; i++) {
        /// Creates game engine with given settings
        GameEngine game(settings);

        /// Adds observer to the view of this game engine
        game.get_view().add_observer(obs);
        game.get_view().add_observer(debugObs);

        GoodPlayer player1("Artem");
        /// Adds it to play Os
        game.set_player(player1);
        /// Starts game until someone wins.
        /// Create second player
        GoodPlayer player2("Alena");
        /// Adds it to play Xs
        game.set_player(player2);
        /// Creates first player

        game.play_game();

        std::cout << "X: " << x << std::endl;
        std::cout << "O: " << o << std::endl;
        std::cout << "Moves: " << mov << std::endl;
    }

    return 0;
}

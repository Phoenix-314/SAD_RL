#include <iostream>

#include <string>
#include <chrono>
#include <functional>
#include <stdexcept>
#include <filesystem>
#include <optional>
#include <exception>

#include "state.h"
#include "ents.h"
#include "actions.h"
#include "fights.h"
#include "reversedIDs.h"
#include "util.h"
#include "transitions.h"
#include "render.h"
#include "factions.h"
#include "validActions.h"

#include "randomAction.h"

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <fstream>


std::string serializationFolderName = "../saveStates";

void temp() {
    std::ofstream ofs("stateLogger.txt");

    // create class instance
    const State state({},{});

    // save data to archive
    {
        boost::archive::text_oarchive oa(ofs);
        // write class instance to archive
        oa << state;
    	// archive and stream closed when destructors are called
    }

    // ... some time later restore the class instance to its orginal state
    State newstate ({}, {});
    {
        // create and open an archive for input
        std::ifstream ifs("stateLogger.txt");
        boost::archive::text_iarchive ia(ifs);
        // read class state from archive
        ia >> newstate;
        // archive and stream closed when destructors are called
    }
}

std::pair<State, int> loadFile() {
    if (std::filesystem::exists(serializationFolderName) && std::filesystem::is_directory(serializationFolderName) && std::filesystem::exists(serializationFolderName + "/state_0.txt")) {
        // Check if the first save file exists
        int i = 0;
        while (std::filesystem::exists(serializationFolderName + "/state_" + std::to_string(i) + ".txt")) { // Finds most recent saved state and loads it
            i += 1;
        }
        i -= 1;
        State state;
        {
            // create and open an archive for input
            std::ifstream ifs(serializationFolderName + "/state_" + std::to_string(i) + ".txt");
            boost::archive::text_iarchive ia(ifs);
            // read class state from archive
            ia >> state;
            // archive and stream closed when destructors are called
        }
        return std::make_pair(state, i + 1);
    } else {
        State s;
        return std::make_pair(s, 0);
    }
}
void saveFile(const State& state, int i) {
    if (!std::filesystem::exists(serializationFolderName)) {
        std::filesystem::create_directory(serializationFolderName);
    }
    std::ofstream ofs(serializationFolderName + "/state_" + std::to_string(i) + ".txt");
    {
        boost::archive::text_oarchive oa(ofs);
        // write class instance to archive
        oa << state;
        // archive and stream closed when destructors are called
    }
}

State runInputCycle(Render& renderer, const State& oldState, const State& ancientState, const std::function<int(State&)>& actionGenerator) {
    /*
    Updates pygame display and reads from pygame console input. (If actionGenerator is provided, empty console input will be filled with actions from the generator)
    Returns the new state after executing the action, or returns the old state if the input was invalid.
    empty for actionGenerator
    q to quit,
    rXXXXX to reroll allies with the given dice values (X = 0-5),
    v to view valid actions (numeric)
    a to view valid actions (text)
    s to view state
    saX to view ally X (1-5)
    seX to view enemy X (1-10)
    b to go back to the previous state (or forward, if b was just used)
    x<code> to execute arbitrary python code (for testing)
    DAxy to use ally x's die on target y (x = 1-5, y = 1-10, or - for untargeted)
    DExy, SAxy, SExy for similar actions. SA0/SE0 is burst
    RXXXXX to reroll allies with the given dice values (X = 0-1, 1 = reroll, 0 = keep)
    E to end turn
    C to continue (if the game is in a non-interactive state)
    */
    State state = oldState;
    std::optional<std::string> line = renderer.readConsoleLine(">>> ", &state);
    if (!line.has_value() || line.value().empty()) {
        try {

            int act = actionGenerator(state);
            transition(state, act);
            renderer.addValueToConsoleHistory(util::getActionStr(act));
            renderer.render(state);
            return state;
        }
        catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
            std::cout << "Some error has occurred. Returning oldstate" << std::endl;
            util::printState(state, true);
            return oldState;
        }
        return oldState;
    }
    try {
        std::string x = line.value();
        if (x[0] == 'q') {
            return State({}, {}); // Empty state indicates to quit
        } else if (x[0] == 'r') {
            if (x.length() != 6) {
                std::cout << "INVALID INPUT. Try again: " << x << std::endl;
                return oldState;
            }
            std::array<int, 5> rerollValues;
            for (int i=0;i<5;i++) { rerollValues[i] = int(x[i+1]); }
            rollAllies(state, rerollValues);
            factions::handleSDS(state);
            return state;
        } else if (x[0] == 'v') {
            for (const auto& v : validActions::validActions(state)) {
                std::cout << v << ", ";
            }
            std::cout << std::endl;
            return oldState;
        } else if (x[0] == 'a') {
            for (const auto& v : validActions::validActions(state)) {
                std::cout << util::getActionStr(v) << ", ";
            }
            std::cout << std::endl;
            return oldState;
        } else if (x[0] == 's') {
            if (x.substr(0, 2) == "sa") {
                util::printEnt(state, *state.players[int(x[2] - '0') - 1]);
            } else if (x.substr(0, 2) == "se") {
                util::printEnt(state, *state.enemies[int(x[2] - '0') - 1]);
            } else {
                util::printState(state);
            }
            return oldState;
        } else if (x[0] == 'b') {
            return ancientState;
        } else if (x[0] == 'x') {
            // exec(x[1:]) // Local testing program - arbitrary code execution is harder in cpp
        } else {
            int action = -1;
            std::pair<int, int> data = {0, 0};
            if (x.substr(0, 2) == "DA") {
                action = DICE_ALLY_ACTION;
                if (x[3] == '-') {
                    data = std::make_pair<int, int>(int(x[2] - '0') - 1, -1);
                } else {
                    data = std::make_pair<int, int>(int(x[2] - '0') - 1, int(x[3] - '0') - 1);
                }
            } else if (x.substr(0, 2) == "DE") {
                action = DICE_ENEMY_ACTION;
                data = std::make_pair<int, int>(int(x[2] - '0') - 1, int(x[3] - '0') - 1);
            } else if (x.substr(0, 2) == "SA") {
                action = SPELL_ALLY_ACTION;
                if (x[3] == '-') {
                    data = std::make_pair<int,int>(int(x[2]), -1);
                } else {
                    data = std::make_pair<int, int>(int(x[2] - '0'), int(x[3] - '0') - 1);
                }
            } else if (x.substr(0,2) == "SE") {
                action = SPELL_ENEMY_ACTION;
                data = std::make_pair<int, int>(int(x[2] - '0'), int(x[3] - '0') - 1);
            } else if (x.substr(0,1) == "R") {
                action = REROLL_ACTION;
                if (x.length() == 1) {
                    data = std::make_pair<int, int>(0, 0);
                } else {
                    if (x.length() != 6 && x.length() != 2) {
                        std::cout << "INVALID INPUT. Try again: " << x << std::endl;
                        return oldState;
                    }
                    if (x.length() == 2) {
                        data = std::make_pair<int, int>(31, 0);
                    } else {
                        std::array<bool, 5> boolarray;
                        for (int i=0;i<5;i++) { boolarray[i] = (x[i+1] == '1'); }
                        data = std::make_pair<int, int>(util::ba2int<5>(boolarray), 0);
                    }
                }
            } else if (x.substr(0,1) == "E") {
                if (state.rerolls == 2) { // Auto finishes turn for convenience, if one tries to end turn at the start of a turn
                    transition(state, actionsReversedIDs.at(std::make_pair(REROLL_ACTION, std::make_pair<int,int>(0, 0))));
                    transition(state, actionsReversedIDs.at(std::make_pair(REROLL_ACTION, std::make_pair<int,int>(0, 0))));
                }
                action = END_TURN_ACTION;
                data = std::make_pair<int, int>(0, 0);
            } else if (x.substr(0,1) == "C") {
                action = CONTINUE_ACTION;
                data = std::make_pair<int, int>(0, 0);
            } else {
                std::cout << "INVALID INPUT. Try again: " << x << std::endl;
                return oldState;
            }

            if (actionsReversedIDs.find(std::make_pair(action, data)) != actionsReversedIDs.end() && validActions::isValidAction(state, actionsReversedIDs.at(std::make_pair(action, data)))) {
                std::cout << "Executing: " << util::getActionStr(actionsReversedIDs.at(std::make_pair(action, data))) << std::endl;
                transition(state, actionsReversedIDs.at(std::make_pair(action, data)));
                renderer.render(state);
                return state;
            } else {
                std::cout << "Illegal Action: " << x << std::endl;
                return oldState;
            }
        }
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
        std::cout << "Some error has occurred. Returning oldstate" << std::endl;
        util::printState(state, true);
        return oldState;
    }
    return state;
}

void makeSerializationFolder() {
    if (!std::filesystem::exists(serializationFolderName)) {
        std::filesystem::create_directory(serializationFolderName);
    }
}

std::pair<State, int> loadState() {
    

    if (std::filesystem::exists(serializationFolderName) && std::filesystem::is_directory(serializationFolderName) && std::filesystem::exists(serializationFolderName + "/state_0.txt")) {
        return loadFile();
    } else {
        State s = genState({LUDUS, LEADER, VALKYRIE, MEDIC, ARTIFICER}, {THORN});
        int i = 0;
        transition(s, actionsReversedIDs.at(std::make_pair(REROLL_ACTION, std::make_pair<int,int>(0, 0))));
        transition(s, actionsReversedIDs.at(std::make_pair(REROLL_ACTION, std::make_pair<int,int>(0, 0))));
        s.mana = 10;

        s = initial();
        i = 0;
        return std::make_pair(s, i);
    }
}

int playGame(std::function<int(State&)> actionGenerator) {
    /*
    Allows user to play game with console input in a pygame window. Saves all non-error states. 
    If a state has been saved, that state will be loaded when restarting. Else, starts a new state
    If actionGenerator is provided, empty console input will be filled with actions from the generator, allowing for automated play.
    */
    Render renderer = Render({1920, 1080});
    
    std::pair<State, int> x = loadState();
    State state = x.first;
    int i = x.second;
    State lastState = State({}, {});
    State ancient = State({}, {});
    while (true) {
        ancient = lastState;
        lastState = state;
        state = runInputCycle(renderer, state, ancient, actionGenerator);
        if (state.players[0]->positionID == -1 && state.players[1]->positionID == -1 && state.players[2]->positionID == -1 && state.players[3]->positionID == -1 && state.players[4]->positionID == -1 && state.enemies.empty()) { // Empty state indicates to quit
            return -1;
        }

        saveFile(state, i);
        i += 1;

        if (state.stateType == StateType::WON || state.stateType == StateType::LOST) {
            std::cout << "Game Over. Result: " << stateTypesReversedIDs.at(state.stateType) << ", Level: " << state.level << std::endl;
            return (state.stateType == StateType::WON) ? 1 : 0;
        }
    }
}

int playManyGamesRandomly(std::function<int(State&)> actionGenerator, int numGames=10000) {
    /*
    Does not render anything.
    Plays numGames games with (mostly) random actions, and displays winrate, average level, and the highest level reached.
    */
    int wins = 0;
    int losses = 0;
    int levels = 0;
    int maxLevel = 0;
    const auto timer = std::chrono::high_resolution_clock::now();
    for (int i = 0; i <= numGames; i++) {
        // Progress bar with time estimate
        //if (i % std::max(numGames / 345, 1) == 0 || i == numGames - 1) {
        //    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - timer).count() / 1000.0;
        //    double estimatedTotal = elapsed / (i + 1) * numGames;
        //    double remaining = estimatedTotal - elapsed;
        //    std::cout << "Progress: " << i << "/" << numGames << " (" << (i / static_cast<double>(numGames) * 100) << "%), Time elapsed: " << elapsed << "s, Estimated remaining: " << remaining << "s                   " << "\r";
        //}
        if (i % 2000 == 0) {
            std::cout << "Levels: " << levels << ", maxLevel: " << maxLevel << std::endl;
        }

        State state = initial(); // Random first fight & heroes
        State oldState = State({},{}); // For debugging, to print the state before an error occurs
        int actionCount = 0;
        int act = -1;
        while (!isTerminal(state)) {
            actionCount += 1;
            try {
                // Infinite Loop Prevention, should never happen as the game causes instant loss after 30 turns
                if (actionCount > 10000) {
                    std::cout << "Action count exceeded 10000, breaking out of loop" << std::endl;
                    throw std::runtime_error("Action count exceeded 10000");
                }
                
                oldState = state;
                act = actionGenerator(state);

                transition(state, act);
            } catch (const std::exception& e) {
                // Debugging information
                std::cout << std::endl;
                std::cout << "Error: " << e.what() << std::endl;

                std::cout << "ActionError: " << act << " " << util::getActionStr(act) << std::endl;
                
                // auto x = validActions::findMadnessedHero(oldState);
                // int index = x.first;
                // Ent* possessedHero = x.second;
                // std::cout << "Temp info, Possessed: " << index << " " << possessedHero << std::endl;
                // std::cout << "Temp info, CanUseDice: " << validActions::diceUsable(oldState, index) << std::endl;
                // std::cout << "Temp info, checkBasicValidity: " << validActions::checkBasicValidity(oldState, index, ) << std::endl;


                std::cout << "Old State: " << std::endl;
                util::printState(oldState, true);

                saveFile(oldState, -99); // may not be in a valid state
                throw std::runtime_error("Error occurred during game simulation");
            }

        }
        // Stat Collection
        levels += state.level;
        maxLevel = std::max(maxLevel, state.level);
        if (state.stateType == StateType::WON) {
            wins += 1;
        } else {
            losses += 1;
        }
    }
    std::cout << std::endl;
    std::cout << "Wins: " << wins << ", Losses: " << losses << ", Winrate: " << (wins / static_cast<double>(wins + losses)) << ", Average Level: " << (levels / static_cast<double>(numGames)) << ", Max Level: " << maxLevel << std::endl;
    std::cout << "Time taken: " << (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - timer).count() / 1000.0) << "s" << std::endl;
	std::cout << "Average time per game: " << (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - timer).count()) / (double) numGames << "ms" << std::endl;
    return maxLevel;
}

int main(int argc, char *argv[]) {
    initializeLibraries();


    // playGame(randomAction);
    
    playManyGamesRandomly(randomAction, 20000);

    // temp(); // Test serialization and deserialization of State

    return 0;
}
/*
int main(int argc, char *argv[])
{
    initializeLibraries();

    State s = initial();
    util::printState(s);
    Render render({1920, 1080});
    render.render(s);
    render.waitForInput();
    
    std::cout << "Hello World!" << std::endl;
	std::cout << "Hello World!" << std::endl;
    int x = 4;
	int y = 5;
    std::array<bool, 57> keywordArray = {false};
    Side side(x, SideType::DAMAGE, keywordArray);

    Ent e(100, std::array<Side, 6>{side, side, side, side, side, side}, 1, -1);
    std::cout << "Ent HP: " << e.hp << std::endl;
    std::cout << "Ent Size: " << e.size << std::endl;

    State state(std::array<Ent, 5>{e, e, e, e, e}, std::vector<Ent>{e, e});
    state.players[0]->redirectTarget = state.players[1]; // Set redirect target for player 0 to player 1

    State stateCopy = state; // Test copy constructor
    
    std::cout << "State player 0 address: " << state.players[0] << std::endl;
    std::cout << "State player 0 redirect target address: " << state.players[0]->redirectTarget << std::endl;
    std::cout << "State player 1 address: " << state.players[1] << std::endl;

    std::cout << "StateCopy player 0 address: " << stateCopy.players[0] << std::endl;
    std::cout << "StateCopy player 0 redirect target address: " << stateCopy.players[0]->redirectTarget << std::endl;
    std::cout << "StateCopy player 1 address: " << stateCopy.players[1] << std::endl;


    std::cout << "State Turn: " << state.turn << std::endl;
    std::cout << "State Players: " << state.players.size() << std::endl;
    std::cout << "State Enemies: " << state.enemies.size() << std::endl;
    
    std::cout << util::findTopPlayer(state) << std::endl;
    std::cout << util::findTopPlayer(stateCopy) << std::endl;

    return 0;
}
*/
#include <iostream>

#include <string>
#include <chrono>
#include <functional>
#include <stdexcept>
#include <filesystem>
#include <optional>
#include <exception>
#include <set>
#include <unordered_map>


#include "state.h"
#include "ents.h"
#include "actions.h"
#include "fights.h"
#include "reversedIDs.h"
#include "util.h"
#include "transitions.h"
#include "factions.h"
#include "validActions.h"

#include "actionGenerator.h"
#include "randomActor.h"
#include "trialActor.h"
#include "heuristicActor.h"
#include "triristicActor.h"
#include "mcts.h"

#include "render.h"

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <fstream>
#include <boost/functional/hash.hpp>

std::string serializationFolderName = "../saveStates";

State loadStateFromFile(const std::string& filename) {
    State state;
    {
        std::ifstream ifs(filename);
        boost::archive::text_iarchive ia(ifs);
        ia >> state;
    }
    return state;
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

State runInputCycle(Render& renderer, const State& oldState, const State& ancientState, MCTS& actionGenerator) {
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
    DAxy to use ally x's die on target y (x = 1-5, y = 1-10, or - for untargeted)
    DExy, SAxy, SExy for similar actions. SA0/SE0 is burst
    RXXXXX to reroll allies with the given dice values (1 = reroll, 0 = keep)
    E to end turn
    C to continue (if the game is in a non-interactive state)
    x and y for debugging
    */
    State state = oldState;
    std::optional<std::string> line = renderer.readConsoleLine(">>> ", &state);
    if (!line.has_value() || line.value().empty()) {
        try {

            int act = actionGenerator.generateAction(state);
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
            for (int i=0;i<5;i++) { rerollValues[i] = int(x[i+1] - '0'); }
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
            if (x[1] == 'x') {
                std::cout << actionGenerator.root->children[int(x[2] - '0') * 100 + int(x[3] - '0') * 10 + int(x[4] - '0')]->toString(1, int(x[6] - '0')) << std::endl;
            } else {

                std::cout << actionGenerator.toString(int(x[1] - '0')) << std::endl;
            }
        } else if (x[0] == 'y') {
            std::unordered_map<State, int, boost::hash<State>> stateMap;
            int countVisits = 0;
            int countUniqueStates = 0;
            int countTotal = 0;
            for (const auto& [act1, randNodeUPtr1] : actionGenerator.root->children) {
                int countUniqueStatesBefore = (int) stateMap.size();
                int numNonUniqueStatesBefore = countTotal;
                for (const auto& [state1, decNodeUPtr1] : randNodeUPtr1->children) {
                    stateMap[state1]++;
                    countTotal++;
                    countVisits += decNodeUPtr1->visits;
                    for (const auto& [act2, randNodeUPtr2] : decNodeUPtr1->children) {
                        for (const auto& [state2, decNodeUPtr2] : randNodeUPtr2->children) {
                        }
                    }
                }
                std::cout << stateMap.size() - countUniqueStatesBefore << " unique states found for action " << util::getActionStr(act1) << " and ";
                std::cout << countTotal - numNonUniqueStatesBefore << " non-unique states found for action " << util::getActionStr(act1) << std::endl;
            }
            std::cout << "Unique states: " << stateMap.size() << ", Total states: " << countTotal << ", Total visits: " << countVisits << std::endl;

            int maxTreeDepth = 0;
            int totalTreeDepth = 0;
            int numLeaves = 0;
            std::function<void(DecisionNode*, int)> traverseTree = [&](DecisionNode* node, int depth) {
                if (node->children.empty()) {
                    maxTreeDepth = std::max(maxTreeDepth, depth);
                    totalTreeDepth += depth;
                    numLeaves++;
                } else {
                    for (const auto& [act, randNodeUPtr] : node->children) {
                        for (const auto& [state, decNodeUPtr] : randNodeUPtr->children) {
                            traverseTree(decNodeUPtr.get(), depth + 1);
                        }
                    }
                }
            };
            traverseTree(actionGenerator.root.get(), 0);
            std::cout << "Max tree depth: " << maxTreeDepth << ", Average tree depth: " << (numLeaves > 0 ? static_cast<double>(totalTreeDepth) / numLeaves : 0) << std::endl;
            return oldState;
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

#ifdef __EMSCRIPTEN__
Render renderer = Render({1920, 1080});
State state;
MCTS* actionGenerator;
bool clearConsole = true;
void init(MCTS* actionGen, State initialState) {
    state = initialState;
    actionGenerator = actionGen;
}
void runGameLoop() {
    std::optional<std::string> line = renderer.readConsoleLineNonBlocking(">>> ", &state, clearConsole);
    clearConsole = false;
    
    if (!line.has_value()) {
        return; // No input yet, continue the loop
    } else {
        clearConsole = true; // Console gets reset since user input has been detected
    }

    if (line.value().empty()) {
        try {
            int act = actionGenerator->generateAction(state);
            transition(state, act);
            renderer.addValueToConsoleHistory(util::getActionStr(act));
            renderer.render(state);
        }
        catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
            std::cout << "Some error has occurred. Returning oldstate" << std::endl;
            util::printState(state, true);
            return;
        }
    }
    try {
        std::string x = line.value();
        if (x[0] == 'q') {
            return; // Empty state indicates to quit
        } else if (x[0] == 'r') {
            if (x.length() != 6) {
                std::cout << "INVALID INPUT. Try again: " << x << std::endl;
            }
            std::array<int, 5> rerollValues;
            for (int i=0;i<5;i++) { rerollValues[i] = int(x[i+1] - '0'); }
            rollAllies(state, rerollValues);
            factions::handleSDS(state);
        } else if (x[0] == 'a') {
            for (const auto& v : validActions::validActions(state)) {
                std::cout << util::getActionStr(v) << ", ";
            }
            std::cout << std::endl;
        } else if (x[0] == 's') {
            if (x.substr(0, 2) == "sa") {
                util::printEnt(state, *state.players[int(x[2] - '0') - 1]);
            } else if (x.substr(0, 2) == "se") {
                util::printEnt(state, *state.enemies[int(x[2] - '0') - 1]);
            } else {
                util::printState(state);
            }
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
            }

            if (actionsReversedIDs.find(std::make_pair(action, data)) != actionsReversedIDs.end() && validActions::isValidAction(state, actionsReversedIDs.at(std::make_pair(action, data)))) {
                std::cout << "Executing: " << util::getActionStr(actionsReversedIDs.at(std::make_pair(action, data))) << std::endl;
                transition(state, actionsReversedIDs.at(std::make_pair(action, data)));
                renderer.render(state);
            } else {
                std::cout << "Illegal Action: " << x << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
        std::cout << "Some error has occurred. Returning oldstate" << std::endl;
        util::printState(state, true);
        return;
    }

    if (state.stateType == StateType::WON || state.stateType == StateType::LOST) {
        std::cout << "Game Over. Result: " << stateTypesReversedIDs.at(state.stateType) << ", Level: " << state.level << std::endl;
        return;
    }
}
int playGameBrowser(MCTS& actionGenerator, State* statePtr) {
    /*
    Allows user to play game with console input in a pygame window. Saves all non-error states. 
    If a state has been saved, that state will be loaded when restarting. Else, starts a new state
    If actionGenerator is provided, empty console input will be filled with actions from the generator, allowing for automated play.
    */
    std::cout << "Starting browser game" << std::endl;
    State s = initial();
    if (statePtr == nullptr) {
        statePtr = &s;
    }
    init(&actionGenerator, *statePtr);
    renderer.initialize();
    emscripten_set_main_loop(runGameLoop, 0, 1);
    return 0;
}
#endif

int playGame(MCTS& actionGenerator, State* statePtr=nullptr) {
    /*
    Allows user to play game with console input in a pygame window. Saves all non-error states. 
    If a state has been saved, that state will be loaded when restarting. Else, starts a new state
    If actionGenerator is provided, empty console input will be filled with actions from the generator, allowing for automated play.
    */
    Render renderer = Render({1920, 1080});
    renderer.initialize();
    
    std::pair<State, int> x;
    if (statePtr == nullptr) {
        x = loadState();
    } else {
        x = std::make_pair(*statePtr, 0);
    }
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

int playManyGamesRandomly(ActionGenerator& actionGenerator, int numGames=10000, int printProgress=1) {
    /*
    Does not render anything.
    Plays numGames games with (mostly) random actions, and displays winrate, average level, and the highest level reached.
    */
    long long actions = 0;
    int wins = 0;
    int losses = 0;
    int levels = 0;
    int maxLevel = 0;
    const auto timer = std::chrono::high_resolution_clock::now();
    for (int i = 1; i <= numGames; i++) {
        // Progress bar with time estimate
        if (printProgress == 1) {
            if (i % std::max(numGames / 345, 1) == 0 || i == numGames - 1) {
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - timer).count() / 1000.0;
                int tmpI = (i == 1) ? 1 : (i - 1); // Prevent division by zero
                double estimatedTotal = elapsed / tmpI * numGames;
                double remaining = estimatedTotal - elapsed;
                std::cout << "Progress: " << (i-1) << "/" << numGames << " (" << ((i-1) / static_cast<double>(numGames) * 100) << "%), Time elapsed: " << elapsed << "s, Estimated remaining: " << remaining << "s                   " << "\r";
            }
        } else if (printProgress >= 2) {
            if (i % std::max(numGames / 20, 1) == 0) {
                std::cout << "After " << i-1 << " Games-> " << "Levels: " << levels << ", actions: " << actions << std::endl;
            }
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
                if (printProgress >= 3) {
                    std::cout << "Action count: " << actionCount << ", Level: " << state.level << std::endl;
                }
                // saveFile(state, -101); // Save state before executing action, for debugging
                oldState = state;
                act = actionGenerator.generateAction(state);
                actions += 1;
                transition(state, act);
            } catch (const std::exception& e) {
                // Debugging information
                std::cout << std::endl;
                std::cout << "Error: " << e.what() << std::endl;

                std::cout << "ActionError: " << act << " " << util::getActionStr(act) << std::endl;

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

    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - timer).count() / 1000.0;
    std::cout << "Progress: " << numGames << "/" << numGames << " (" << (numGames / static_cast<double>(numGames) * 100) << "%), Time elapsed: " << elapsed << "s                                                                           " << std::endl;
	std::cout << "Action Generator: " << actionGenerator.toString() << std::endl;
    std::cout << "Wins: " << wins << ", Losses: " << losses << ", Winrate: " << (wins / static_cast<double>(wins + losses)) << ", Total Levels: " << levels << ", Average Level: " << (levels / static_cast<double>(numGames)) << ", Max Level: " << maxLevel << std::endl;
    std::cout << "Time taken: " << (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - timer).count() / 1000.0) << "s" << std::endl;
	std::cout << "Average time per game: " << (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - timer).count()) / (double) numGames << "ms" << std::endl;
    std::cout << "Exact action count: " << actions << std::endl;
    std::cout << "===========================================================================" << std::endl << std::endl;
    return maxLevel;
}

int main(int argc, char *argv[]) {
    std::cout << "Starting program..." << std::endl;
    initializeLibraries();

    RandomActor randomActor;
    HeuristicActor heuristicActor;
    TrialActor trialActor(100);
    TriristicActor triristicActor(100);
    MCTS mcts(50000, 0.05, 0.4, 0.35, 2); // nSims, K, alpha, beta, progressBar // 200000
    // mcts(5000, 1.414, -0.0, 0.5, 0); // with BASE MCTS select, not SPW

    // State state = initial();
    State state = genState({RANGER, CAPTAIN, PILGRIM, FATE, CHRONOS}, {QUARTZ, BASALT, QUARTZ}, {std::array<int, 5>{1, 3, 4, 0, 2}}, {std::vector<int>{3, 1, 2}}); // a hard fight
    // State state = genState({LUDUS, LEADER, VALKYRIE, MEDIC, ARTIFICER}, {WARCHIEF}, {std::array<int, 5>{0, 0, 0, 0, 0}});
    // state.players[1]->dead = true;
    // state.players[2]->dead = true;
    // state.players[3]->dead = true;
    // state.players[4]->dead = true;

    // state.players[0]->hp = 1;

    // mcts.generateAction(state);
    
    // std::cout << std::filesystem::exists(serializationFolderName + "/state_-101.txt") << std::endl;
    // State state = loadStateFromFile(serializationFolderName + "/state_-101.txt");
    // util::printState(state, true);
    // trialActor.generateAction(state);
    // playGame(randomActor, &state);
    // return 0;
    
    // return 0;

    int numGames = 100;
    std::cout << "Playing " << numGames << " games..." << std::endl;
    //  rand();
    #ifdef __EMSCRIPTEN__
    playGameBrowser(mcts, &state);
    #else
	// State state = initial();
	//int v = mcts.generateAction(state);
    //std::cout << v << std::endl;
    playGame(mcts, &state);
    // State init = initial();
    // playGame(heuristicActor, &init);
    // playManyGamesRandomly(mcts, numGames, 1);
    // playManyGamesRandomly(triristicActor, numGames, 1);
    // playManyGamesRandomly(heuristicActor, numGames, 1);
    // playManyGamesRandomly(trialActor, numGames, 1);
    // playManyGamesRandomly(randomActor, numGames, 1);
    #endif


    return 0;
}

#include "randomActor.h"

#include <random>
#include <array>

#include "actions.h"
#include "validActions.h"
#include "reversedIDs.h"
#include "keywordIDs.h"
#include "util.h"


int RandomActor::generateAction(State& state) {
    std::vector<int> validActionsList = validActions::validActionsFast(state);

    int act = -1;
    while (true) {
        act = validActionsList[rand() % validActionsList.size()];

        // Picking slightly better than random play
        if (act == actionsReversedIDs.at(std::make_pair(END_TURN_ACTION, std::make_pair(0, 0))) && validActionsList.size() > 1) {
            continue;
        }
        if (act == 80 || act == 81 || act == 82 || act == 83 || act == 84) { // Bursting defensively (SA01-SA05) is almost always a waste of mana for random actions
            continue;
        }
        if (state.rerolls > 0 && state.stateType == StateType::BATTLE) { // Reroll if blank, else dont.
            auto rr = std::array<bool, 5>{false};

            for (int i = 0; i < 5; i++) {
                if (!validActions::diceUsable(state, i) && !(state.players[i]->dead || state.players[i]->currentSide.keywords[KeywordID::STICKY])) {
                    rr[i] = true;
                } else {
                    rr[i] = false;
                }
            }
            int targAct = actionsReversedIDs.at(std::make_pair(REROLL_ACTION, std::make_pair(util::ba2int<5>(rr), 0)));
            if (std::find(validActionsList.begin(), validActionsList.end(), targAct) != validActionsList.end()) {
                act = targAct;
            }
        }
        break;
    }
    return act;
}
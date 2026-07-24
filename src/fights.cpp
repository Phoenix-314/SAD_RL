#include "fights.h"
#include <random>
#include <stdexcept>
#include <string>

#include <iostream>

std::vector<int> Fights::generateFight(int lvl) {
    const auto& fights = MONSTER_LISTS[lvl];
    double randomValue = static_cast<double>(rand()) / RAND_MAX;

    double cumulativeProbability = 0.0;
    for (size_t i = 0; i < fights.size(); ++i) {
        cumulativeProbability += MONSTER_PROBS[lvl][i];
        if (randomValue <= cumulativeProbability) {
            return fights[i];
        }
    }
    return fights.back(); // Fallback in case of rounding errors. Apparently 1.0 is not <= 1.0 sometimes, mainly since monster probs sum to near 1, while randomValue can be exactly 1.0
    // throw std::runtime_error("Failed to generate a fight for level " + std::to_string(lvl) + " with random value " + std::to_string(randomValue));
    // return std::vector<int>();
}
#pragma once
#include <string>
#include <vector>
#include "../world/Level.hpp"

struct RankingEntry {
    std::string playerName;
    std::string difficulty;
    std::string timeFormatted;
    int score;
};

class Ranking {
public:
    static void saveScore(const GameState& state);
    static std::vector<RankingEntry> loadScores();
};

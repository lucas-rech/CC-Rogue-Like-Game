#include "Ranking.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>

void Ranking::saveScore(const GameState& state) {
    std::ofstream file("ranking.txt", std::ios::app);
    if (file.is_open()) {
        file << state.playerName << "," 
             << getDifficultyName(state.difficulty) << ","
             << formatCampaignTime(state.campaignElapsedTime) << ","
             << state.finalScore << "\n";
        file.close();
    } else {
        std::cerr << "Erro ao abrir ranking.txt para salvar." << std::endl;
    }
}

std::vector<RankingEntry> Ranking::loadScores() {
    std::vector<RankingEntry> entries;
    std::ifstream file("ranking.txt");
    std::string line;

    if (file.is_open()) {
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string name, diff, timeStr, scoreStr;
            
            if (std::getline(ss, name, ',') &&
                std::getline(ss, diff, ',') &&
                std::getline(ss, timeStr, ',') &&
                std::getline(ss, scoreStr, ',')) {
                
                RankingEntry entry;
                entry.playerName = name;
                entry.difficulty = diff;
                entry.timeFormatted = timeStr;
                try {
                    entry.score = std::stoi(scoreStr);
                } catch (...) {
                    entry.score = 0;
                }
                entries.push_back(entry);
            }
        }
        file.close();
    }

    // Sort descending by score
    std::sort(entries.begin(), entries.end(), [](const RankingEntry& a, const RankingEntry& b) {
        return a.score > b.score;
    });

    return entries;
}

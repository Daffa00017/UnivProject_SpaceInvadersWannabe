#pragma once
#include <string>

void PostScoreAsync(const std::string& serverBase,
    const std::string& player, int score);

std::string LoadServerBase(); // optional helper to read Saves/server.txt

// Leaderboard.h
#pragma once
#include <string>
#include <vector>

struct LBEntry {
    std::string name;                    // username (unique key)
    int    score = 0;              // best score
    int    level = 1;              // best level reached
    double timeSec = 0.0;            // timestamp of best (for tie-break)

    // extra profile snapshot for API / resume
    std::string              selectedShipId = "Classic";
    std::vector<std::string> unlockedShips = { "Classic" };
    double                   lastUpdatedSec = 0.0;
};

class Leaderboard {
public:
    bool Load();                 // reads Saves/leaderboard.json (creates it if missing)
    bool Save() const;           // writes Saves/leaderboard.json

    // Insert or update per-username best score + merge unlocks
    void UpsertUser(const std::string& username,
        int score, int level, double timeSec,
        const std::string& selectedShipId,
        const std::vector<std::string>& unlockedShips,
        double lastUpdatedSec);

    std::vector<LBEntry> Top(int n) const; // top n by score desc, time asc

private:
    std::vector<LBEntry> entries;

    // tiny JSON helpers (manual, schema-specific)
    static std::string JsonEscape(const std::string& s);
    static bool        JsonGetString(const std::string& src, const std::string& key, std::string& out);
    static bool        JsonGetInt(const std::string& src, const std::string& key, int& out);
    static bool        JsonGetDouble(const std::string& src, const std::string& key, double& out);
    static bool        JsonGetStringArray(const std::string& src, const std::string& key, std::vector<std::string>& out);

    // platform helper to ensure folder exists
    static void EnsureSavesDir();

    static constexpr const char* kPath() { return "Saves/leaderboard.json"; }
};

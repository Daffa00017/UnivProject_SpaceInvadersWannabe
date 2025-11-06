// Leaderboard.h
#pragma once
#include <string>
#include <vector>

struct LBEntry {
    std::string name;            // username (unique key)
    // NEW: plain-text password (dev-only)
    std::string password;        // "" if unknown/not set

    int    score = 0;
    int    level = 1;
    double timeSec = 0.0;

    std::string              selectedShipId = "Classic";
    std::vector<std::string> unlockedShips = { "Classic" };
    double                   lastUpdatedSec = 0.0;
};

class Leaderboard {
public:
    bool Load();
    bool Save() const;

    // Existing signature (unchanged)
    void UpsertUser(const std::string& username,
        int score, int level, double timeSec,
        const std::string& selectedShipId,
        const std::vector<std::string>& unlockedShips,
        double lastUpdatedSec);

    // NEW: optional password (plain text). Pass "" to keep existing.
    void UpsertUser(const std::string& username,
        int score, int level, double timeSec,
        const std::string& selectedShipId,
        const std::vector<std::string>& unlockedShips,
        double lastUpdatedSec,
        const std::string& password);
    // Find existing user (nullptr if not found)
    const LBEntry* FindUser(const std::string& username) const;


    std::vector<LBEntry> Top(int n) const;

private:
    std::vector<LBEntry> entries;

    static std::string JsonEscape(const std::string& s);
    static bool        JsonGetString(const std::string& src, const std::string& key, std::string& out);
    static bool        JsonGetInt(const std::string& src, const std::string& key, int& out);
    static bool        JsonGetDouble(const std::string& src, const std::string& key, double& out);
    static bool        JsonGetStringArray(const std::string& src, const std::string& key, std::vector<std::string>& out);

    static void EnsureSavesDir();
    static constexpr const char* kPath() { return "Saves/leaderboard.json"; }
};

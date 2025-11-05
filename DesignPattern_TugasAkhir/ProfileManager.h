// ProfileManager.h
#pragma once
#include <string>
#include <vector>

class ProfileManager {
public:
    struct Profile {
        std::string username;                 
        int  highscore = 0;
        int  maxLevel = 1;
        std::vector<std::string> unlockedShips{ "Classic" }; 
        std::string selectedShipId = "Classic";
        double lastUpdatedSec = 0.0;
    };

    // lifecycle
    bool LoadFromDisk();            
    bool SaveToDisk() const;        
    bool Login(const std::string& username);

    bool HasCurrent() const { return loaded; }
    const Profile& CurrentProfile() const { return current; }
    Profile& CurrentProfile() { return current; }

    void SnapshotFromGame(int highscore, int maxLevel, const std::string& selectedShipId, double nowSec);
    void EnsureUnlocked(const std::string& shipId);
    bool IsShipUnlocked(const std::string& shipId) const;
    void SetSelectedShip(const std::string& shipId);

private:
    Profile current;
    bool loaded = false;

    static std::string Trim(const std::string& s);
    static void SplitCSV(const std::string& s, std::vector<std::string>& out);
    static std::string JoinCSV(const std::vector<std::string>& v);

    static constexpr const char* kPath() { return "profile.ini"; }
};

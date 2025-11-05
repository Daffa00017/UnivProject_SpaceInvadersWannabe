// ProfileManager.cpp
#include "ProfileManager.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

// ----------- tiny helpers -----------
std::string ProfileManager::Trim(const std::string& s) {
    size_t a = 0, b = s.size();
    while (a < b && std::isspace((unsigned char)s[a])) ++a;
    while (b > a && std::isspace((unsigned char)s[b - 1])) --b;
    return s.substr(a, b - a);
}

void ProfileManager::SplitCSV(const std::string& s, std::vector<std::string>& out) {
    out.clear();
    std::string cur;
    std::istringstream ss(s);
    while (std::getline(ss, cur, ',')) {
        cur = Trim(cur);
        if (!cur.empty()) out.push_back(cur);
    }
}

std::string ProfileManager::JoinCSV(const std::vector<std::string>& v) {
    std::ostringstream o;
    for (size_t i = 0; i < v.size(); ++i) { if (i) o << ','; o << v[i]; }
    return o.str();
}

// ----------- disk I/O (INI-like) -----------
bool ProfileManager::LoadFromDisk() {
    std::ifstream f(kPath());
    if (!f.is_open()) { loaded = false; return false; }

    Profile p; // defaults set
    std::string line;
    while (std::getline(f, line)) {
        line = Trim(line);
        if (line.empty() || line[0] == '#' || line[0] == ';') continue;
        size_t eq = line.find('=');
        if (eq == std::string::npos) continue;
        std::string key = Trim(line.substr(0, eq));
        std::string val = Trim(line.substr(eq + 1));

        if (key == "username")        p.username = val;
        else if (key == "highscore")  p.highscore = std::max(0, std::stoi(val));
        else if (key == "maxlevel")   p.maxLevel = std::max(1, std::stoi(val));
        else if (key == "selected")   p.selectedShipId = val;
        else if (key == "lastUpdatedSec") p.lastUpdatedSec = std::stod(val);
        else if (key == "unlocked")   SplitCSV(val, p.unlockedShips);
    }
    current = p;
    loaded = true;
    return true;
}

bool ProfileManager::SaveToDisk() const {
    std::ofstream f(kPath());
    if (!f.is_open()) return false;

    f << "# Space Invaders profile (single-user)\n";
    f << "username=" << current.username << "\n";
    f << "highscore=" << current.highscore << "\n";
    f << "maxlevel=" << current.maxLevel << "\n";
    f << "selected=" << current.selectedShipId << "\n";
    f << "unlocked=" << JoinCSV(current.unlockedShips) << "\n";
    f << "lastUpdatedSec=" << current.lastUpdatedSec << "\n";
    return true;
}

// ----------- login -----------
bool ProfileManager::Login(const std::string& username) {
    // try load existing (ok if fails)
    LoadFromDisk();

    if (loaded && current.username == username) {
        return true; // same user
    }

    // new or switching user -> create/reset profile
    Profile p;
    p.username = username;
    p.selectedShipId = "Classic";
    p.unlockedShips = { "Classic" };
    current = p;
    loaded = true;
    SaveToDisk();
    return true;
}

// ----------- game helpers -----------
void ProfileManager::SnapshotFromGame(int highscore, int maxLevel, const std::string& selectedShipId, double nowSec) {
    if (!loaded) return;
    current.highscore = std::max(current.highscore, highscore);
    current.maxLevel = std::max(current.maxLevel, maxLevel);
    current.selectedShipId = selectedShipId;
    current.lastUpdatedSec = nowSec;
}

void ProfileManager::EnsureUnlocked(const std::string& shipId) {
    if (!loaded) return;
    auto& v = current.unlockedShips;
    if (std::find(v.begin(), v.end(), shipId) == v.end()) v.push_back(shipId);
}

bool ProfileManager::IsShipUnlocked(const std::string& shipId) const {
    if (!loaded) return shipId == "Classic";
    const auto& v = current.unlockedShips;
    return std::find(v.begin(), v.end(), shipId) != v.end();
}

void ProfileManager::SetSelectedShip(const std::string& shipId) {
    if (!loaded) return;
    current.selectedShipId = shipId;
}

// Leaderboard.cpp
#include "Leaderboard.h"
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <cctype>

// ----- platform: ensure Saves/ exists -----
#ifdef _WIN32
#include <direct.h>
void Leaderboard::EnsureSavesDir() { _mkdir("Saves"); }  // OK if already exists
#else
#include <sys/stat.h>
#include <sys/types.h>
void Leaderboard::EnsureSavesDir() { mkdir("Saves", 0755); } // ignore EEXIST
#endif

// ---- tiny JSON helpers (manual, schema-specific) ----
std::string Leaderboard::JsonEscape(const std::string& s) {
    std::string out; out.reserve(s.size() + 8);
    for (char c : s) {
        switch (c) {
        case '\\': out += "\\\\"; break;
        case '\"': out += "\\\""; break;
        case '\n': out += "\\n";  break;
        case '\r': out += "\\r";  break;
        case '\t': out += "\\t";  break;
        default:   out += c;      break;
        }
    }
    return out;
}
static inline std::string Trim(const std::string& s) {
    size_t a = 0, b = s.size();
    while (a < b && std::isspace((unsigned char)s[a])) ++a;
    while (b > a && std::isspace((unsigned char)s[b - 1])) --b;
    return s.substr(a, b - a);
}
bool Leaderboard::JsonGetString(const std::string& src, const std::string& key, std::string& out) {
    const std::string pat = "\"" + key + "\"";
    size_t k = src.find(pat);                 if (k == std::string::npos) return false;
    size_t colon = src.find(':', k);          if (colon == std::string::npos) return false;
    size_t q1 = src.find('"', colon);         if (q1 == std::string::npos) return false;
    size_t q2 = src.find('"', q1 + 1);        if (q2 == std::string::npos) return false;

    // raw string between quotes
    std::string raw = src.substr(q1 + 1, q2 - (q1 + 1));

    // minimal unescape for \" \\ \n \r \t
    std::string buf; buf.reserve(raw.size());
    for (size_t i = 0; i < raw.size(); ++i) {
        if (raw[i] == '\\' && i + 1 < raw.size()) {
            char n = raw[i + 1];
            if (n == '\\' || n == '\"') { buf += n; ++i; continue; }
            if (n == 'n') { buf += '\n'; ++i; continue; }
            if (n == 'r') { buf += '\r'; ++i; continue; }
            if (n == 't') { buf += '\t'; ++i; continue; }
        }
        buf += raw[i];
    }
    out.swap(buf);
    return true;
}
bool Leaderboard::JsonGetInt(const std::string& src, const std::string& key, int& out) {
    std::string pat = "\"" + key + "\"";
    size_t k = src.find(pat); if (k == std::string::npos) return false;
    k = src.find(':', k); if (k == std::string::npos) return false;
    while (k + 1 < src.size() && src[k + 1] == ' ') ++k;
    size_t s = k + 1, e = s;
    while (e < src.size() && (std::isdigit((unsigned char)src[e]) || src[e] == '-')) ++e;
    if (e == s) return false;
    out = std::stoi(src.substr(s, e - s));
    return true;
}
bool Leaderboard::JsonGetDouble(const std::string& src, const std::string& key, double& out) {
    std::string pat = "\"" + key + "\"";
    size_t k = src.find(pat); if (k == std::string::npos) return false;
    k = src.find(':', k); if (k == std::string::npos) return false;
    while (k + 1 < src.size() && src[k + 1] == ' ') ++k;
    size_t s = k + 1, e = s;
    while (e < src.size() && (std::isdigit((unsigned char)src[e]) || src[e] == '-' || src[e] == '.')) ++e;
    if (e == s) return false;
    out = std::stod(src.substr(s, e - s));
    return true;
}
bool Leaderboard::JsonGetStringArray(const std::string& src, const std::string& key, std::vector<std::string>& out) {
    const std::string pat = "\"" + key + "\"";
    size_t k = src.find(pat);            if (k == std::string::npos) return false;
    size_t lb = src.find('[', k);        if (lb == std::string::npos) return false;
    size_t rb = src.find(']', lb);       if (rb == std::string::npos) return false;

    out.clear();
    size_t i = lb + 1;
    while (i < rb) {
        // skip spaces/commas
        while (i < rb && (src[i] == ' ' || src[i] == ',')) ++i;
        if (i >= rb) break;
        if (src[i] != '\"') { ++i; continue; }

        size_t q1 = i;
        size_t q2 = src.find('\"', q1 + 1);
        if (q2 == std::string::npos || q2 > rb) break;

        std::string raw = src.substr(q1 + 1, q2 - (q1 + 1));

        // minimal unescape
        std::string buf; buf.reserve(raw.size());
        for (size_t t = 0; t < raw.size(); ++t) {
            if (raw[t] == '\\' && t + 1 < raw.size()) {
                char n = raw[t + 1];
                if (n == '\\' || n == '\"') { buf += n; ++t; continue; }
            }
            buf += raw[t];
        }
        out.push_back(buf);
        i = q2 + 1;
    }
    return true;
}

static size_t FindMatchingSquare(const std::string& s, size_t openPos) {
    int depth = 0; bool inStr = false;
    for (size_t i = openPos; i < s.size(); ++i) {
        char c = s[i];
        if (inStr) {
            if (c == '\\' && i + 1 < s.size()) { ++i; continue; } // skip escape
            if (c == '\"') inStr = false;
            continue;
        }
        if (c == '\"') { inStr = true; continue; }
        if (c == '[') ++depth;
        else if (c == ']') { --depth; if (depth == 0) return i; }
    }
    return std::string::npos;
}

static size_t FindMatchingBrace(const std::string& s, size_t openPos) {
    int depth = 0; bool inStr = false;
    for (size_t i = openPos; i < s.size(); ++i) {
        char c = s[i];
        if (inStr) {
            if (c == '\\' && i + 1 < s.size()) { ++i; continue; }
            if (c == '\"') inStr = false;
            continue;
        }
        if (c == '\"') { inStr = true; continue; }
        if (c == '{') ++depth;
        else if (c == '}') { --depth; if (depth == 0) return i; }
    }
    return std::string::npos;
}

// ---- load/save JSON ----
bool Leaderboard::Load() {
    EnsureSavesDir();

    entries.clear();

    // if file doesn't exist yet, seed an empty file and return
    {
        std::ifstream test(kPath(), std::ios::binary);
        if (!test.is_open()) {
            std::ofstream seed(kPath(), std::ios::binary);
            if (seed.is_open()) {
                seed << "{\n  \"leaderboard\": []\n}\n";
            }
            return true; // nothing to parse yet
        }
    }

    std::ifstream f(kPath(), std::ios::binary);
    if (!f.is_open()) return false;

    std::stringstream ss; ss << f.rdbuf();
    std::string s = ss.str();

    // find "leaderboard": [
    std::string key = "\"leaderboard\"";
    size_t k = s.find(key); if (k == std::string::npos) return false;
    size_t arrStart = s.find('[', k);  if (arrStart == std::string::npos) return false;
    size_t arrEnd = FindMatchingSquare(s, arrStart);
    if (arrEnd == std::string::npos) return false;

    size_t i = arrStart + 1;
    while (i < arrEnd) {
        // next object
        size_t objStart = s.find('{', i);
        if (objStart == std::string::npos || objStart > arrEnd) break;

        size_t objEnd = FindMatchingBrace(s, objStart);
        if (objEnd == std::string::npos || objEnd > arrEnd) break;

        std::string block = s.substr(objStart, objEnd - objStart + 1);

        LBEntry row;
        JsonGetString(block, "username", row.name);
        JsonGetInt(block, "score", row.score);
        JsonGetInt(block, "level", row.level);
        JsonGetDouble(block, "timeSec", row.timeSec);
        JsonGetString(block, "selectedShipId", row.selectedShipId);
        JsonGetStringArray(block, "unlockedShips", row.unlockedShips);
        JsonGetDouble(block, "lastUpdatedSec", row.lastUpdatedSec);
        if (!row.name.empty()) entries.push_back(row);

        i = objEnd + 1; // continue after this object
    }
    return true;
}

bool Leaderboard::Save() const {
    EnsureSavesDir();

    std::ofstream f(kPath(), std::ios::binary);
    if (!f.is_open()) return false;

    auto q = [&](const std::string& s) { return "\"" + JsonEscape(s) + "\""; };

    f << "{\n  \"leaderboard\": [\n";
    for (size_t i = 0; i < entries.size(); ++i) {
        const auto& e = entries[i];
        if (i) f << ",\n";
        f << "    {\n";
        f << "      \"username\": " << q(e.name) << ",\n";
        f << "      \"score\": " << e.score << ",\n";
        f << "      \"level\": " << e.level << ",\n";
        f << "      \"timeSec\": " << e.timeSec << ",\n";
        f << "      \"selectedShipId\": " << q(e.selectedShipId) << ",\n";
        f << "      \"unlockedShips\": [";
        for (size_t k = 0; k < e.unlockedShips.size(); ++k) {
            if (k) f << ", ";
            f << q(e.unlockedShips[k]);
        }
        f << "],\n";
        f << "      \"lastUpdatedSec\": " << e.lastUpdatedSec << "\n";
        f << "    }";
    }
    f << "\n  ]\n}\n";
    return true;
}

// merge helper for unlocked ships
static void MergeUnique(std::vector<std::string>& dst, const std::vector<std::string>& src) {
    for (const auto& s : src) {
        if (std::find(dst.begin(), dst.end(), s) == dst.end()) dst.push_back(s);
    }
}

void Leaderboard::UpsertUser(const std::string& username,
    int score, int level, double timeSec,
    const std::string& selectedShipId,
    const std::vector<std::string>& unlockedShips,
    double lastUpdatedSec)
{
    auto it = std::find_if(entries.begin(), entries.end(),
        [&](const LBEntry& e) { return e.name == username; });

    if (it == entries.end()) {
        LBEntry e;
        e.name = username; e.score = score; e.level = level; e.timeSec = timeSec;
        e.selectedShipId = selectedShipId; e.unlockedShips = unlockedShips; e.lastUpdatedSec = lastUpdatedSec;
        entries.push_back(std::move(e));
    }
    else {
        // keep best score (or earlier time on tie)
        if (score > it->score || (score == it->score && timeSec < it->timeSec)) {
            it->score = score;
            it->level = level;
            it->timeSec = timeSec;
        }
        else {
            // still keep best of level separately
            it->level = std::max(it->level, level);
        }
        // always update snapshot fields
        it->selectedShipId = selectedShipId;
        MergeUnique(it->unlockedShips, unlockedShips);
        it->lastUpdatedSec = lastUpdatedSec;
    }

    // keep sorted for fast Top()
    std::sort(entries.begin(), entries.end(), [](const LBEntry& a, const LBEntry& b) {
        if (a.score != b.score) return a.score > b.score;
        return a.timeSec < b.timeSec;
        });
    if (entries.size() > 100) entries.resize(100);
}

std::vector<LBEntry> Leaderboard::Top(int n) const {
    if (n < 0) n = 0;
    if ((size_t)n > entries.size()) n = (int)entries.size();
    return std::vector<LBEntry>(entries.begin(), entries.begin() + n);
}

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

#include "httplib.h"
#include <thread>
#include <fstream>
#include <iostream>
#include <string>

namespace {
    // <<< PUT THE HELPER HERE >>>
    const char* ErrToStr(httplib::Error e) {
        using E = httplib::Error;
        switch (e) {
        case E::Connection:             return "connection failed (DNS/port/firewall)";
        case E::Read:                   return "read error";
        case E::Write:                  return "write error";
        case E::ExceedRedirectCount:    return "too many redirects";
        case E::Canceled:               return "canceled";
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
        case E::SSLConnection:          return "TLS connection error";
        case E::SSLLoadingCerts:        return "TLS cert load error";
        case E::SSLPeerCertificate:     return "TLS peer cert error";
        case E::SSLServerVerification:  return "TLS verification failed";
#endif
        default:                        return "other network error";
        }
    }

    static std::string JsonEscape(const std::string& s) {
        std::string o; o.reserve(s.size() + 8);
        for (unsigned char c : s) {
            switch (c) {
            case '\"': o += "\\\""; break;
            case '\\': o += "\\\\"; break;
            case '\b': o += "\\b";  break;
            case '\f': o += "\\f";  break;
            case '\n': o += "\\n";  break;
            case '\r': o += "\\r";  break;
            case '\t': o += "\\t";  break;
            default:   o += (char)c;
            }
        }
        return o;
    }
} // namespace

std::string LoadServerBase() {
    std::ifstream f("Saves/Server.txt");
    std::string s;
    if (f && std::getline(f, s) && !s.empty()) return s;
    return "http://localhost:5209";
}

void PostScoreAsync(const std::string& serverBase,
    const std::string& player, int score) {
    std::thread([serverBase, player, score] {
        try {
            std::cout << "[Net] Posting score: " << player << " = " << score
                << " to " << serverBase << "/api/scores\n";

            httplib::Client cli(serverBase.c_str());
            cli.set_connection_timeout(2, 0);
            cli.set_read_timeout(3, 0);
            cli.set_write_timeout(3, 0);
            cli.set_keep_alive(false);

            std::string body = std::string("{\"playerName\":\"")
                + JsonEscape(player) + "\",\"score\":" + std::to_string(score) + "}";

            auto res = cli.Post("/api/scores", body, "application/json");
            if (!res) {
                std::cout << "[Net] POST failed: " << ErrToStr(res.error()) << "\n";
                return;
            }
            std::cout << "[Net] POST status = " << res->status << "\n";
            if (res->status == 307 || res->status == 308) {
                std::cout << "[Net] Redirected (likely HTTPS). Use https base or disable UseHttpsRedirection.\n";
            }
            else {
                std::cout << "[Net] Response body: " << res->body << "\n";
            }
        }
        catch (...) {
            std::cout << "[Net] Exception during POST\n";
        }
        }).detach();
}

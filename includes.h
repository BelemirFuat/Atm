//includes header file 
#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <mutex>
#include <algorithm>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <cstring>
#include <arpa/inet.h>


std::string getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    time_t now_time = std::chrono::system_clock::to_time_t(now);

    // Format the time as [YYYY-MM-DD HH:MM:SS]
    std::stringstream ss;
    ss << std::put_time(localtime(&now_time), "[%Y-%m-%d %H:%M:%S]");
    return ss.str();
}

void logMessage(const std::string& message, std::string fileName = "Unknown") {
    std::ofstream logFile(fileName+".log", std::ios_base::app); // Open log file in append mode
    if (logFile.is_open()) {
        logFile << getCurrentTime() << " " << message << std::endl;
        logFile.close();
    } else {
        std::cerr << "Unable to open log file!" << std::endl;
    }
}

std::vector<std::string> splitString(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = str.find(delimiter);
    
    while (end != std::string::npos) {
        tokens.push_back(str.substr(start, end - start));
        start = end + 1;
        end = str.find(delimiter, start);
    }
    
    tokens.push_back(str.substr(start));
    
    return tokens;
}

// Large prime number for the hash function (chosen to reduce collisions)
const unsigned long long PRIME = 257;
const unsigned long long MOD = 0xFFFFFFFFFFFFFFFF; // Using max 64-bit unsigned value for large number space

// Function to convert a 32-character std::string into a unique number
unsigned long long stringToUniqueNumber(const std::string& str) {
    unsigned long long hashValue = 0;

    for (char ch : str) {
        hashValue = (hashValue * PRIME + ch) % MOD;
    }

    return hashValue;
}

std::string generateKey(const std::string& text, const std::string& keyword) {
    std::string key = keyword;
    while (key.length() < text.length()) {
        key += keyword;
    }
    return key.substr(0, text.length());
}

// Function to encrypt the text using the Vigenère cipher
std::string encrypt(const std::string& text, const std::string& keyword) {
    std::string key = generateKey(text, keyword);
    std::string encryptedText = "";

    for (size_t i = 0; i < text.length(); ++i) {
        char ch = text[i];
        if (isupper(ch)) {
            encryptedText += char((ch + key[i] - 2 * 'A') % 26 + 'A');
        } else if (islower(ch)) {
            encryptedText += char((ch + key[i] - 2 * 'a') % 26 + 'a');
        } else {
            encryptedText += ch; // Non-alphabetic characters remain the same
        }
    }
    return encryptedText;
}

// Function to decrypt the text using the Vigenère cipher
std::string decrypt(const std::string& encryptedText, const std::string& keyword) {
    std::string key = generateKey(encryptedText, keyword);
    std::string decryptedText = "";

    for (size_t i = 0; i < encryptedText.length(); ++i) {
        char ch = encryptedText[i];
        if (isupper(ch)) {
            decryptedText += char((ch - key[i] + 26) % 26 + 'A');
        } else if (islower(ch)) {
            decryptedText += char((ch - key[i] + 26) % 26 + 'a');
        } else {
            decryptedText += ch; // Non-alphabetic characters remain the same
        }
    }
    return decryptedText;
}

std::string getIPAdress(int socket)
{
    struct sockaddr_in peer_address;
    socklen_t peer_addrlen = sizeof(peer_address);

    if (getpeername(socket, (struct sockaddr *)&peer_address, &peer_addrlen) == -1) {
        perror("getpeername failed");
        exit(EXIT_FAILURE);
    }

    // Convert the IP address to a human-readable form
    char ip_address[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &peer_address.sin_addr, ip_address, INET_ADDRSTRLEN);
    return ip_address;
}
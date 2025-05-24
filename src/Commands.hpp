#pragma once

#include <functional>
#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>

class Map;

using CommandFunction = std::function<int(const std::string&, const std::vector<std::string>&, const Map&)>;

class Commands {
private:
    std::unordered_map<std::string, CommandFunction> m_commands;
    const Map& m_map;

public:
    Commands(int argc, char* argv[], const Map& map) : m_map(map) {
        (void)argc; (void)argv; // Éviter les warnings
    }

    void registerCommand(const std::string& name, CommandFunction func) {
        m_commands[name] = func;
    }

    void listCommands() const {
        for (const auto& pair : m_commands) {
            std::cout << "  - " << pair.first << std::endl;
        }
    }

    int execute(int argc, char* argv[]) {
        if (argc < 2) {
            std::cerr << "No command specified" << std::endl;
            return 1;
        }

        std::string command = argv[1];
        auto it = m_commands.find(command);

        if (it == m_commands.end()) {
            std::cerr << "Unknown command: " << command << std::endl;
            std::cout << "Available commands:" << std::endl;
            listCommands();
            return 1;
        }

        // Construire le vecteur d'arguments (sans le nom du programme et la commande)
        std::vector<std::string> args;
        for (int i = 2; i < argc; ++i) {
            args.push_back(argv[i]);
        }

        return it->second(argv[0], args, m_map);
    }
};
#pragma once

#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "Map.hpp"

class Commands {
   public:
    // Type pour les fonctions de commande
    // Prend en paramètre : le paramètre pré-construit et les arguments restants
    using CommandFunction =
        std::function<int(const std::string&, const std::vector<std::string>&, const Map&)>;

   private:
    std::unordered_map<std::string, CommandFunction> commands_;
    std::string programParam_;
    Map map_;

   public:
    // Constructeur qui extrait le paramètre du programme
    Commands(int argc, char* argv[], const Map& map) {
        if (argc > 1) {
            programParam_ = argv[1];
            map_ = map;
        }
    }

    // Enregistrer une commande avec sa fonction
    void registerCommand(const std::string& commandName, CommandFunction func) {
        commands_[commandName] = func;
    }

    // Exécuter les commandes à partir des arguments
    int execute(int argc, char* argv[]) {
        if (argc < 3) {
            std::cerr << "Usage: " << argv[0] << " <programParam> <command> [args...]" << std::endl;
            return 1;
        }

        std::string commandName = argv[2];

        // Vérifier si la commande existe
        auto it = commands_.find(commandName);
        if (it == commands_.end()) {
            std::cerr << "Commande inconnue: " << commandName << std::endl;
            return 1;
        }

        // Récupérer les arguments restants
        std::vector<std::string> remainingArgs;
        for (int i = 3; i < argc; ++i) {
            remainingArgs.push_back(argv[i]);
        }

        // Exécuter la commande avec le paramètre pré-construit et les arguments
        return it->second(programParam_, remainingArgs, map_);
    }

    // Getter pour le paramètre du programme (si besoin)
    const std::string& getProgramParam() const {
        return programParam_;
    }

    // Lister les commandes disponibles
    void listCommands() const {
        std::cout << "Commandes disponibles:" << std::endl;
        for (const auto& pair : commands_) {
            std::cout << "  - " << pair.first << std::endl;
        }
    }
};

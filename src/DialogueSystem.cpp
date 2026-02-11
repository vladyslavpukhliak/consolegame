#include "DialogueSystem.h"
#include "Graphics.h"
#include "Services.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <conio.h>
#include <chrono>
#include <thread>


using namespace rapidjson;

// Рекурсивна функція для діалогу з персонажем
void DialogueSystem::processDialogue(const Value& mainNode, const Value& node, Level& level, Player& player, Enemy& enemyEntry, const std::string& art, const std::string& path) {
    if (!node.IsObject()) return;

    // Має бути лише один ключ: фраза NPC, а значення — варіанти відповіді
    if (node.MemberCount() != 1) {
        std::cerr << "Кожен рівень повинен містити лише одну фразу NPC.\n";
        return;
    }

    const auto& npcLine = node.MemberBegin();
    const std::string firstLine = npcLine->name.GetString();

    std::cout << "\x1B[2J\x1B[H";
    printf(art.c_str());
    printf("\n%s:\n", Services::graphics().colorize(Services::graphics().Utf8ToAnsi(enemyEntry._name), enemyEntry._color).c_str());

#pragma region HandleBufs

    if(firstLine == "bufs"){
        const rapidjson::Value& bufs_json = npcLine->value;

        if (bufs_json.HasMember("money") && bufs_json["money"].IsInt()) {
            int cost = bufs_json["money"].GetInt();
            if (cost < 0) { // Якщо вартість від'ємна (списання коштів)
                if (!player.TryCharge(abs(cost))) {
                    std::cout << "Недостатньо коштів.";
                    return; // Виходимо з функції, не виконуючи буфи
                }
            }
        }
            
        // Кошти успішно списано (якщо є цінник), продовжуємо виконання

        enemyEntry._fear = bufs_json.HasMember("fear") && bufs_json["fear"].IsInt() ?
            (bufs_json["fear"].GetInt() == 1) : enemyEntry._fear;

        enemyEntry._isMovable = bufs_json.HasMember("movable") && bufs_json["movable"].IsInt() ?
            (bufs_json["movable"].GetInt() == 1) : enemyEntry._isMovable;

        // не певен шо працює цей рядок, це через UK layout в JSON
        enemyEntry._isFriendly = bufs_json.HasMember("friendly") && bufs_json["friendly"].IsInt() ?
            (bufs_json["friendly"].GetInt() == 1) : enemyEntry._isFriendly;

        std::string lineToSay(bufs_json.HasMember("line") && bufs_json["line"].IsString() ?
            bufs_json["line"].GetString() : "Розмову закінчено.");

        Services::graphics().print("\n"+lineToSay+"\n", 1000);

        // Чи буде босфайт?
        fightArt = "",fightMusic="";
        std::string desc = "";
        if (bufs_json.HasMember("bossfight") && bufs_json["bossfight"].IsString()
            && bufs_json["bossfight"].GetStringLength() > 0) {
            desc = Services::graphics().Utf8ToAnsi(bufs_json["bossfight"].GetString());
            // fallback to raw UTF-8 if conversion produced empty
            if (desc.empty()) desc = bufs_json["bossfight"].GetString();
        }
        if (bufs_json.HasMember("fightArt") && bufs_json["fightArt"].IsString()
            && bufs_json["fightArt"].GetStringLength() > 0) {
            fightArt = bufs_json["fightArt"].GetString();
        }
        if (bufs_json.HasMember("fightMusic") && bufs_json["fightMusic"].IsString()
            && bufs_json["fightMusic"].GetStringLength() > 0) {
            fightMusic = bufs_json["fightMusic"].GetString();
        }

        if (!desc.empty()) {
            std::u8string bossfightPathU8(reinterpret_cast<const char8_t*>(desc.c_str()));
            bossfightPath = bossfightPathU8;
        }
        isEndOfConversation = true;
        return;
    }
#pragma endregion

    Services::graphics().print(firstLine, 1000);


    const Value& responses = npcLine->value;
    if (!responses.IsObject()) return;

    std::vector<std::string> keys;
    if (!isEndOfConversation)
    {
        for (auto it = responses.MemberBegin(); it != responses.MemberEnd(); ++it) {
            keys.push_back(it->name.GetString());
        }
    }

    while (true) {
        printf("\n\nНаявні кошти:%d UAH\n", player.GetAvailableMoney());
        std::cout << "Ваші варіанти відповіді:\n";
        if (!isEndOfConversation) {
            for (size_t i = 0; i < keys.size(); ++i) {
                std::cout << "  " << (i + 1) << ". " << keys[i] << "\n";
            }
        }
        std::cout << "  0. Завершити діалог\n";

        int choice;
        std::cout << "Ваш вибір: ";
        if (!(std::cin >> choice) || choice < 0 || choice >(int)keys.size()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Невірний вибір, спробуйте ще.\n";
            continue;
        }
        if (choice == 0 || isEndOfConversation) return;

        const std::string& selected = keys[choice - 1];
        const Value& next = responses[selected.c_str()];

        if (next.IsObject() && next.MemberCount() > 0) {
            processDialogue(mainNode, next, level, player, enemyEntry, art, path + " -> " + selected);
        }
        else {
            // Відобразити текст на що ГГ відповідає.
            std::cout << "\x1B[2J\x1B[H";
            printf(art.c_str());
            printf("\n%s:\n", Services::graphics().colorize(Services::graphics().Utf8ToAnsi(enemyEntry._name), enemyEntry._color).c_str());
            printf("%s\n", firstLine.c_str());

            // Анімована відповідь:
            Services::graphics().print("Ви: " + selected, 1000);
            std::cout << "\nДіалог завершено.";
            //processDialogue(mainNode, mainNode, player, enemyEntry, art, "");
            return;
        }
    }
}


void DialogueSystem::initDialogue(const std::filesystem::path& path, const std::string& art, Level& level, Player& player, Enemy& enemyEntry) {

    std::ifstream ifs(path, std::ios::binary);

    if (!ifs.is_open()) {
        std::cerr << "Не вдалося відкрити файл dialogue.json\n";
    }
    IStreamWrapper isw(ifs);

    Document doc;
    doc.ParseStream(isw);
    if (doc.HasParseError() || !doc.IsObject()) {
        std::cerr << "Помилка парсингу JSON або неправильний формат.\n";
    }

    processDialogue(doc, doc, level, player, enemyEntry, art, "");
    std::cout << "Кінець взаємодії.\n";
    if (!bossfightPath.empty()) {
        level.loadBossfight(bossfightPath, fightArt, fightMusic, player);
		bossfightPath.clear();
    }
    else {
        GameSystem::UnPauseTheGame();
        Services::graphics().init();
    }

    isEndOfConversation = false;
}

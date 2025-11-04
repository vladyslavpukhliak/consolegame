#pragma once
#include "Enemy.h"
#include "Player.h"
#include <filesystem> 
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"

class DialogueSystem
{
public:
	//void initDialogue(const std::filesystem::path& path, const std::string& art, Player& player, Enemy& enemyEntry);
	void initDialogue(const std::string& path, const std::string& art, Player& player, Enemy& enemyEntry);
private:
	void processDialogue(const rapidjson::Value& mainode, const rapidjson::Value& node, Player& player, Enemy& enemyEntry, const std::string& art, const std::string& path = "");
	bool isEndOfConversation = false;
};


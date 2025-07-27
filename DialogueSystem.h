#pragma once
#include "Enemy.h"
#include "Player.h"
#include "include/rapidjson/document.h"
#include "include/rapidjson/istreamwrapper.h"

class DialogueSystem
{
public:
	void initDialogue(const std::string& path, const std::string& art, Player& player, Enemy& enemyEntry);
private:
	void processDialogue(const rapidjson::Value& mainode, const rapidjson::Value& node, Player& player, Enemy& enemyEntry, const std::string& art, const std::string& path = "");
	bool isEndOfConversation = false;
};


#include "FileSystem.h"

vector<EnemyInfo> enemiesVector;
ordered_json jsonFile;
//vector<EnemyInfo> getVectorOfenemiesVector() { return enemiesVector; }
//Cannon& getCannonParams() { return cannon; }
// ���������� �� ���� ����, ������ �������� �������
void checkFile(ifstream& f) {
	if (!f.is_open()) {
		system("cls");
		printf("help!");
		cerr << "�������: �������� �������� ����� ��������.\n";
	}
}

// TODO: ������������� �����.
// �������� �������� �������� ����� ��������� ���� ����� enemies.json, � ���� �������� �� ���� ����������� ��.
void initVectorOfenemiesVector(string filePath) {
	ifstream f(filePath); // ���������� ���� �� ����, ��� ����� ��������� ��� �����
	
	
	
	//checkFile(f); // ��������� �� ���� ����

	f >> jsonFile; // �������� ���� ����� � ����
	
	EnemyInfo enemy;
	enemiesVector.clear();

	// ����������� ��������
	for (auto& category : jsonFile.items()) { // ����� �������� (default, hounds)
		// TODO: ����� �� ������ ��� ������ (default, hound) ���˲������
		for (auto& element : category.value().items()) { // ����� ������ � �������

			enemy.name = element.key(); // ����� ������
			auto& data = element.value(); // ��� ������

			enemy.tile = data["tile"].get<string>()[0]; // ������
			enemy.color = data["color"];
			enemy.description = data["description"];
			enemy.level = data["level"];
			enemy.attack = data["attack"];
			enemy.health = data["health"];
			enemy.experience = data["experience"];

			enemiesVector.push_back(enemy);
		}
	}
	f.close();
}

void initCannonParams(string filePath) {
	ifstream f(filePath); // ���������� ���� �� ����, ��� ����� ��������� ��� �����
	if (f.is_open()) {
		try {
			f >> jsonFile;
		}
		catch (const nlohmann::json::parse_error& e) {
			std::cerr << "JSON parse error: " << e.what() << std::endl;
		}
	}
	else {
		std::cerr << "Could not open file" << std::endl;
	}
	Cannon cannon;
	// ����������� ��������
	for (auto& category : jsonFile.items()) { // ����� �������� (gun, )
		printf("%s", category.key().c_str()); // test PASSED
		for (auto& element : category.value().items()) { // ����� �������� � �������

			cannon.name = element.key(); // �����
			printf("%s", cannon.name.c_str()); // test PASSED
			auto& data = element.value(); // ���

			std::vector<std::string> tileStrings = data["tile"].get<std::vector<std::string>>();

			// ����������� � ����� char
			std::vector<char> tileChars;
			for (const auto& str : tileStrings) {
				if (!str.empty()) {
					tileChars.push_back(str[0]);  // ������ ������ ������ � ������� �����
				}
			}
			for (size_t i = 0; i < tileChars.size(); i++)
			{
				char a = cannon.tile[i] = tileChars[i];
				printf("%c", a); // test PASSED
			}
			/*for (int i = 0; i < CANNON_TILE_COUNT; ++i) {
				cannon.tile[i] = data["tile"][i].get<char>();
			}*/
			for (int i = 0; i < CANNON_PROJECTILE_COUNT; ++i) {
				cannon.projectile[i] = data["projectile"][i].get<char>();
			}
			cannon.color = data["color"];
			cannon.description = data["description"];
			cannon.playerDeathLine = data["playerDeathLine"];
		}
	}
	f.close();
	for (int i = 0; i < 4;++i) printf("%c",cannon.projectile[i]);
}


// TODO: �� �� �������� �� �������, � ���� ����� � �������� ������, �� ��� ������������ ��� ��������.
//void getEnemiesVector() {
//	// �� ������� ��������� ��� � � � ���������, ���� ������ ��� ��� �������������.
//	if (enemiesVector.size() == 0) initVectorOfenemiesVector();
//
//	// �������� ������� �����
//	setUTF8();
//	for (size_t i = 0; i < enemiesVector.size(); i++)
//		printf("%d. %s\n", i + 1, enemiesVector[i].name.c_str());
//	setLocale();
//}

void clearVector() {
	delete[] enemiesVector.data();
	jsonFile.clear();
}
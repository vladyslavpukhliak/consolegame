#include "FileSystem.h"

vector<EnemyInfo> enemiesVector;
ordered_json jsonFile;
//vector<EnemyInfo> getVectorOfenemiesVector() { return enemiesVector; }
//Cannon& getCannonParams() { return cannon; }
// Перевіряємо чи існує файл, інакше виводимо помилку
void checkFile(ifstream& f) {
	if (!f.is_open()) {
		system("cls");
		printf("help!");
		cerr << "Помилка: перевірте цілісність файлів програми.\n";
	}
}

// TODO: Перейменувати метод.
// Спочатку спробуємо написати метод конкретно який зчитує enemies.json, а потім адаптуємо під більш універсальну річ.
void initVectorOfenemiesVector(string filePath) {
	ifstream f(filePath); // ініціалізуємо файл як зміну, щоб надалі оперувати ним самим
	
	
	
	//checkFile(f); // перевіримо чи існує файл

	f >> jsonFile; // записуємо зміст файлу у зміну
	
	EnemyInfo enemy;
	enemiesVector.clear();

	// Перерахунок категорій
	for (auto& category : jsonFile.items()) { // Обхід категорій (default, hounds)
		// TODO: масив що містить два масива (default, hound) РЕАЛІЗУВАТИ
		for (auto& element : category.value().items()) { // Обхід ворогів у категорії

			enemy.name = element.key(); // Назва ворога
			auto& data = element.value(); // Дані ворога

			enemy.tile = data["tile"].get<string>()[0]; // Символ
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
	ifstream f(filePath); // ініціалізуємо файл як зміну, щоб надалі оперувати ним самим
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
	// Перерахунок категорій
	for (auto& category : jsonFile.items()) { // Обхід категорій (gun, )
		printf("%s", category.key().c_str()); // test PASSED
		for (auto& element : category.value().items()) { // Обхід елементів у категорії

			cannon.name = element.key(); // Назва
			printf("%s", cannon.name.c_str()); // test PASSED
			auto& data = element.value(); // Дані

			std::vector<std::string> tileStrings = data["tile"].get<std::vector<std::string>>();

			// Конвертація в масив char
			std::vector<char> tileChars;
			for (const auto& str : tileStrings) {
				if (!str.empty()) {
					tileChars.push_back(str[0]);  // Беремо перший символ з кожного рядка
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


// TODO: Ця річ насправді не потрібна, а була взята з минулого проєкту, її слід модифікувати або видалити.
//void getEnemiesVector() {
//	// Не потрібно проводити одні й ті ж процедури, якщо вектор вже був ініціалізований.
//	if (enemiesVector.size() == 0) initVectorOfenemiesVector();
//
//	// Виводимо доступні опції
//	setUTF8();
//	for (size_t i = 0; i < enemiesVector.size(); i++)
//		printf("%d. %s\n", i + 1, enemiesVector[i].name.c_str());
//	setLocale();
//}

void clearVector() {
	delete[] enemiesVector.data();
	jsonFile.clear();
}
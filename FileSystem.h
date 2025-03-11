#include "lib/nlohmann/json.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "entities.h"
using namespace std;
using namespace nlohmann;



void checkFile(ifstream& file);
//vector<EnemyInfo> getVectorOfenemiesVector();
void initCannonParams(string filePath);
//Cannon& getCannonParams();
void clearVector();
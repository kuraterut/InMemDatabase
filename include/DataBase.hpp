#include <iostream>
#include <fstream>
#include <stdio.h>
#include <vector>
#include <array>
#include <bits/stdc++.h>
#include <variant>
#include <map>
#include <nlohmann/json.hpp>
#include "Parser.hpp"



using namespace std;
using json = nlohmann::json;

class Database{
private:
	map<string, Table> tables;

public:
	ResultSet execute(const string& query);

	bool saveToFile(const string& filePath);
	Database loadFromFile(const string& filePath);
	
private:
	bool findAttr(const vector<ATTRIBUTE>& vec, ATTRIBUTE attr);
	string toLowerCase(const string& word);
};




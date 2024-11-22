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

// #ifndef FILE_H
// #define FILE_H
// #include "Help.hpp"
// #endif


using namespace std;
using json = nlohmann::json;

class Database{
private:
	map<string, Table> tables;

public:
	// ResultSet executeGet(string query);
	int executePost(const string& query);
	ResultSet executeGet(const string& query);


	Database loadFromFile(const string& filePath);
	bool saveToFile(const string& filePath);

	map<string, Table> getMap(){
		return tables;
	}
private:
	bool findAttr(const vector<ATTRIBUTE>& vec, ATTRIBUTE attr);

};




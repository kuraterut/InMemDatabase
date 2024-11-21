#include <iostream>
#include <fstream>
#include <stdio.h>
#include <vector>
#include <array>
#include <bits/stdc++.h>
#include <variant>
#include <map>
#include "Parser.hpp"

// #ifndef FILE_H
// #define FILE_H
// #include "Help.hpp"
// #endif


using namespace std;

class Database{
private:
	map<string, Table> tables;

public:
	// ResultSet executeGet(string query);
	int executePost(const string& query);
	vector<vector<variant<string, int, bool>>> executeGet(const string& query);


	Database loadFromFile(const string& filePath);
	bool saveToFile(const string& filePath);

	map<string, Table> getMap(){
		return tables;
	}


};




#include <string>
#include <regex>
#include <bits/stdc++.h>
#include <cstring>
#include <stdio.h>
#include <cctype>
#include <utility>
#include <string>
#include <algorithm>
#include <iomanip>
#include <set>
#include "Table.hpp"


using namespace std;


class Parser{
public:
	int parsePost(const string& query, map<string, Table>& tables);

	// ResultSet parseGet(const string& query, map<string, Table>& tables);
	ResultSet parseGet(const string& query, map<string, Table>& tables);
	// static ResultSet parseGet(string query, map<string, Table> tables);

private:
	// ResultSet parseSelect(string query, map<string, Table> tables);
	
	int parseInsert(const string& query, map<string, Table>& tables);
	int parseDelete(const string& query, map<string, Table>& tables);
	int parseCreate(const string& query, map<string, Table>& tables);
	int parseUpdate(const string& query, map<string, Table>& tables);
	ResultSet parseSelect(const string& query, map<string, Table>& tables);

	string toLowerCase(const string& word);
	vector<ATTRIBUTE> parseAttributes(const string& attributes);
	TYPES parseType(const string& type);
	int parseInsertValues(const string& values, Table& table);

	void insertVal(int columnNum, string value, vector<variant<string, int, bool>>& newRow, Table& table);
	void insertColVal(string columnName, string value, vector<variant<string, int, bool>>& newRow, Table& table);
	int findColIndexByName(const string& columnName, Table& table);
	void insertDefaultValue(int columnNum, vector<variant<string, int, bool>>& newRow, Table& table);
	bool findAttr(const vector<ATTRIBUTE>& vec, ATTRIBUTE attr);
	void checkUnique(variant<string, int, bool> val, Table& table, int columnNum, vector<variant<string, int, bool>>& newRow);


	int precedence(const string& op);
	bool isOperator(const string& op);
	bool isStringDigit(const string& str);
	bool isArithmeticOperator(const string& op);
	
	variant<string, int, bool> executeArithmeticCondition(const vector<variant<string, int, bool>>& row, const vector<string>& vec, Table& table);
	vector<int> parseColumnsToVecIndex(const string& columns, Table& table);

	vector<pair<int, variant<string, int, bool>>> parseAssignments(const vector<variant<string, int, bool>>& row, const string& assignments, Table& table);
public:
	bool checkCondition(const vector<variant<string, int, bool>>& row, const string& condition, Table& table);

};
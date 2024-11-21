#include "DataBase.hpp"

using namespace std;

int Database::executePost(const string& query){
	Parser parser = Parser();
	return parser.parsePost(query, tables);
}

vector<vector<variant<string, int, bool>>> Database::executeGet(const string& query){
	Parser parser = Parser();
	return parser.parseGet(query, tables);
}
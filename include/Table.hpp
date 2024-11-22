#include <vector>
#include <variant>
#include <set>
#include "ResultSet.hpp"

using namespace std;



class Column{
public:
	string columnName;
	vector<ATTRIBUTE> attributes;
	TYPES type;
	bool hasSize;
	int size; 
	bool hasDefaultValue;
	variant<string, int, bool> value;
	bool isUnique;
	set<variant<string, int, bool>> setValues;
};

class TableMetadata{
public:
	string tableName;
	vector<Column> columnsInfo;
};

class Table{
public:
	TableMetadata metadata;
	vector<vector<variant<string, int, bool>>> rows;
};


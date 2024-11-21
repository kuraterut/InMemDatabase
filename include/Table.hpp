#include <vector>
#include <variant>
#include <set>

using namespace std;

enum class ATTRIBUTE{
	KEY,
	UNIQUE,
	AUTOINCREMENT
};

enum class TYPES{
	STRING,
	BYTES,
	INT32,
	BOOL
};

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


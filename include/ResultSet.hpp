#include <vector>
#include <variant>
#include <string>

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


class Row{
private:
	vector<variant<string, int, bool>> row;
	vector<pair<string, TYPES>> columns;
public:
	Row() = delete;
	Row(const vector<variant<string, int, bool>>& rowCur, const vector<pair<string, TYPES>>& columnsCur){
		row = rowCur;
		columns = columnsCur;
	}

	int getInt(const string& column){
		pair<int, TYPES> para = findColByName(column);
		
		if(para.first == -1 || para.second != TYPES::INT32){
			throw string{"ERROR: invalid integer column"}; 
		}
		return get<int>(row[para.first]);
	}
	string getString(const string& column){
		pair<int, TYPES> para = findColByName(column);
		if(para.first == -1 || para.second != TYPES::STRING){
			throw string{"ERROR: invalid string column"}; 
		}
		return get<string>(row[para.first]);
	}
	bool getBool(const string& column){
		pair<int, TYPES> para = findColByName(column);
		if(para.first == -1 || para.second != TYPES::BOOL){
			throw string{"ERROR: invalid boolean column"};
		}
		return get<bool>(row[para.first]);
	}
	string getBytes(const string& column){
		pair<int, TYPES> para = findColByName(column);
		if(para.first == -1 || para.second != TYPES::BYTES){
			throw string{"ERROR: invalid bytes column"};
		}
		return get<string>(row[para.first]);
	}

private:
	pair<int, TYPES> findColByName(const string& column){
		for(int i = 0; i < columns.size(); i++){
			if (columns[i].first == column){
				pair<int, TYPES> para;
				para.first = i;
				para.second = columns[i].second;
				return para;
			}
		}
		pair<int, TYPES> err;
		err.first = -1;
		err.second = TYPES::INT32;
		return err;
	}

};

class ResultSet{
private:
	vector<Row> rows;
	bool isOkFlag;
	string error;

public:
	ResultSet() = default;
	ResultSet(bool isOkCur){
		isOkFlag = isOkCur;
	}
	ResultSet(vector<Row> vec, bool isOkCur){
		rows = vec;
		isOkFlag = isOkCur;
	}
	ResultSet(string errorText, bool isOkCur){
		error = errorText;
		isOkFlag = isOkCur;
	}


	bool isOk() const {return isOkFlag;}

	string getError() const {return error;}
	
	void addRow(const Row& row) {rows.push_back(row);}

	auto begin() {return rows.begin();}
	auto end() {return rows.end();}

	auto begin() const {return rows.begin();}
	auto end() const {return rows.end();}

};

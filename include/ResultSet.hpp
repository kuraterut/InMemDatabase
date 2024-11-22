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
			cout << "ERROR: invalid integer column" << endl;
			exit(-1); 
		}
		return get<int>(row[para.first]);
	}
	string getString(const string& column){
		pair<int, TYPES> para = findColByName(column);
		if(para.first == -1 || para.second != TYPES::STRING){
			cout << "ERROR: invalid string column" << endl;
			exit(-1); 
		}
		return get<string>(row[para.first]);
	}
	bool getBool(const string& column){
		pair<int, TYPES> para = findColByName(column);
		if(para.first == -1 || para.second != TYPES::BOOL){
			cout << "ERROR: invalid boolean column" << endl;
			exit(-1); 
		}
		return get<bool>(row[para.first]);
	}
	string getBytes(const string& column){
		pair<int, TYPES> para = findColByName(column);
		if(para.first == -1 || para.second != TYPES::BYTES){
			cout << "ERROR: invalid bytes column" << endl;
			exit(-1); 
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
	bool isOk;
public:
	ResultSet() = default;
	ResultSet(bool isOkCur){
		isOk = isOkCur;
	}
	
	void addRow(const Row& row){
		rows.push_back(row);
	}

	auto begin(){return rows.begin();}
	auto end(){return rows.end();}

	auto begin() const {return rows.begin();}
	auto end() const {return rows.end();}

};

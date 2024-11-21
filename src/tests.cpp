#include "DataBase.hpp"

using namespace std;

int main(){
	Database db = Database();
	string create = "cReAte taBLe lol   ({key} name0 :    int32 = 2,    name1 : string[32] = \"haha\")";
	string insert1 = "insert (,\"hehehe\") to lol";
	string insert2 = "insert (3,,) to lol";
	string insert3 = "insert (name1 = \"elelelel\", name0 = 24) to lol";

	string select1 = "select name1, name0 from lol where |name1| = 6 || name0 % 2 = 0";
	string deleteReq = "delete lol where true";
	string select2 = "select name0, name1 from lol where true";

	int createPost = db.executePost(create);
	int insertPost1 = db.executePost(insert1);
	int insertPost2 = db.executePost(insert2);
	int insertPost3 = db.executePost(insert3);
	vector<vector<variant<string, int, bool>>> selectGet1 = db.executeGet(select1);
	for (int i = 0; i < selectGet1.size(); ++i){
		for (int j = 0; j < selectGet1[i].size(); ++j){
			if(selectGet1[i][j].index() == 0){
				cout << get<string>(selectGet1[i][j]) << endl;
			}
			else if(selectGet1[i][j].index() == 1){
				cout << get<int>(selectGet1[i][j]) << endl;
			}
			else if(selectGet1[i][j].index() == 2){
				cout << get<bool>(selectGet1[i][j]) << endl;
			}
			
		}
	}

	int deletePost1 = db.executePost(deleteReq);
	
	cout << endl << deletePost1 << endl;
	
	vector<vector<variant<string, int, bool>>> selectGet2 = db.executeGet(select2);
	
	for (int i = 0; i < selectGet2.size(); ++i){
		for (int j = 0; j < selectGet2[i].size(); ++j){
			if(selectGet2[i][j].index() == 0){
				cout << get<string>(selectGet2[i][j]) << endl;
			}
			else if(selectGet2[i][j].index() == 1){
				cout << get<int>(selectGet2[i][j]) << endl;
			}
			else if(selectGet2[i][j].index() == 2){
				cout << get<bool>(selectGet2[i][j]) << endl;
			}
			
		}
	}


	// cout << get<int>(db.getMap()["lol"].rows[0][0]) << endl;
	// cout << get<string>(db.getMap()["lol"].rows[0][1]) << endl;
	// cout << get<int>(db.getMap()["lol"].rows[1][0]) << endl;
	// cout << get<string>(db.getMap()["lol"].rows[1][1]) << endl;
	// cout << get<int>(db.getMap()["lol"].rows[2][0]) << endl;
	// cout << get<string>(db.getMap()["lol"].rows[2][1]) << endl;
	return 0;
}
#include "DataBase.hpp"


using namespace std;




int main(){
	Database db = Database();
	string create = "cReAte taBLe lol   ({key} name0 :    int32 = 2,    name1 : string[32] = \"haha\")";
	string insert1 = "insert (,\"hehehe\") to lol";
	string insert2 = "insert (3,,) to lol";
	string insert3 = "insert (name1 = \"elelelel\", name0 = 5) to lol";

	string select1 = "select name1, name0 from lol where |name1| = 6 || name0 % 2 = 0";
	string deleteReq = "delete lol where true";
	string select2 = "select name0, name1 from lol where true";

	string update1 = "update lol set name0 = name0+1 where name0 = 2";


	int createPost = db.executePost(create);
	int insertPost1 = db.executePost(insert1);
	int insertPost2 = db.executePost(insert2);
	int insertPost3 = db.executePost(insert3);
	int updatePost1 = db.executePost(update1);
	ResultSet selectGet1 = db.executeGet(select1);
	// ResultSet selectGet2 = db.executeGet(select2);
	for(auto elem : selectGet1){
		cout << elem.getInt("name0") << endl;
		cout << elem.getString("name1") << endl;
		cout << endl;
	}
	cout << endl;

	





	
	// ResultSet selectGet2 = db1.executeGet(select2);
	
	


	// cout << get<int>(db.getMap()["lol"].rows[0][0]) << endl;
	// cout << get<string>(db.getMap()["lol"].rows[0][1]) << endl;
	// cout << get<int>(db.getMap()["lol"].rows[1][0]) << endl;
	// cout << get<string>(db.getMap()["lol"].rows[1][1]) << endl;
	// cout << get<int>(db.getMap()["lol"].rows[2][0]) << endl;
	// cout << get<string>(db.getMap()["lol"].rows[2][1]) << endl;
	return 0;
}
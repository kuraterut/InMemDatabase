#include <vector>
#include <variant>

using namespace std;

class Select {
private:
	Table table;
	vector<string> columns;
	Expression expr;

private:
	bool executeExpr(vector<variant<bool, string, int>> row);

public:
	ResultSet executeSelect();
};
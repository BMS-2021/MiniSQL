#include "BPTree.h"
#include "../macro.h"

#include <map>
#include <string>
using std::map;
using std::string;

const string index_file = "idxf";
const int TREE_SIZE = 666;
const int TREE_DEGREE = 4;

class IndexManager {
public:
	IndexManager() = default;
	~IndexManager();

	void create(const string& treename, const value_type& type);
	void drop(const string& treename, const value_type& type);

	ELEMENTTYPE search(const string& treename, const sql_value& val, const vector<sql_value>& vec) const;
	ELEMENTTYPE searchHead(const string& treename, const sql_value& val, const vector<sql_value>& vec) const;
	ELEMENTTYPE searchNext(const string& treename, const sql_value& val, const vector<sql_value>& vec) const;

	ELEMENTTYPE insert(const string& treename, const sql_value& val, vector<sql_value>& vec, queue<ELEMENTTYPE> que);
	ELEMENTTYPE remove(const string& treename, const sql_value& val, vector<sql_value>& vec, queue<ELEMENTTYPE> que);

	void save();
	void load();

private:
	map<string, BPTree<int>> intMap;
	map<string, BPTree<float>> floatMap;
	map<string, BPTree<string>> strMap;
};
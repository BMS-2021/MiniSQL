#include "BPTree.h"
#include "../macro.h"

#include <map>
#include <string>
using std::map;
using std::string;
using std::make_pair;

const string index_file = "trees.ind";
const int TREE_SIZE = 666;
const int TREE_DEGREE = 4;

using std::get;
using std::make_tuple;

class IndexManager {
public:
	IndexManager() = default;
	~IndexManager();

	void create(const string& treename);
	void create(const string& treename, const vector<sql_value>& indexs);
	void drop(const string& treename);

	ELEMENTTYPE search(const string& treename, const sql_value& val, uint32_t idx_pos) const;
	ELEMENTTYPE searchHead(const string& treename, const sql_value& val) const;
	ELEMENTTYPE searchNext(const string& treename, const sql_value& val, uint32_t idx_pos) const;

	void insert(const string& treename, const sql_value& val, uint32_t idx_pos, ELEMENTTYPE new_key);
	void remove(const string& treename, const sql_value& val, uint32_t idx_pos);

	void save();
	void load();

private:
	map<string, BPTree> index_manager;
};
	
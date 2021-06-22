#include "BPTree.h"
#include "../macro.h"

#include <map>
#include <string>
using std::map;
using std::string;
using std::make_pair;

const string vecque_file = "vnq.ind";
const string index_file = "trees.ind";
const int TREE_SIZE = 666;
const int TREE_DEGREE = 4;

using int_tuple = std::tuple<BPTree<int>, std::vector<int>, std::queue<ELEMENTTYPE>>;
using float_tuple = std::tuple<BPTree<float>, std::vector<float>, std::queue<ELEMENTTYPE>>;
using string_tuple = std::tuple<BPTree<string>, std::vector<string>, std::queue<ELEMENTTYPE>>;
using std::get;
using std::make_tuple;


class IndexManager {
public:
	IndexManager() = default;
	~IndexManager();

	void create(const string& treename, const value_type& type);
	void create(const string& treename, const vector<sql_value>& indexs);
	void drop(const string& treename, const value_type& type);

	ELEMENTTYPE search(const string& treename, const sql_value& val) const;
	ELEMENTTYPE searchHead(const string& treename, const sql_value& val) const;
	ELEMENTTYPE searchNext(const string& treename, const sql_value& val) const;

	void insert(const string& treename, const sql_value& val);
	void remove(const string& treename, const sql_value& val);

	void save();
	void load();

private:
	std::map<string, int_tuple> intmgr;
	std::map<string, float_tuple> floatmgr;
	std::map<string, string_tuple> stringmgr;
};
	
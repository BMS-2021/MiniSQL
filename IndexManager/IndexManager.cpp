#include "IndexManager.h"

#include <fstream>

IndexManager::~IndexManager() {
  // TODO: destruct function
}

void create(const string& treename, const value_type& type)
{
	switch (type)
	{
	case INT:
		intMap.insert(std::make_pair(treename, new BPTree<int>(TREE_SIZE, TREE_DEGREE)));
		break;
	case FLOAT:
		floatMap.insert(std::make_pair(treename, new BPTree<float>(TREE_SIZE, TREE_DEGREE)));
		break;
	case CHAR:
		strMap.insert(std::make_pair(treename, new BPTree<string>(TREE_SIZE, TREE_DEGREE)));
		break;
	}
}

void IndexManager::drop(const string& treename, const value_type& type)
{
	switch (type)
	{
	case INT: {
		auto it = intMap.find(treename);
		delete it->second;
		break;
	}
	case FLOAT: {
		auto it = floatMap.find(treename);
		if (it == floatMap.end()) {
			throw std::exception("tree not found");
		}
		delete it->second;
		break;
	}
	case CHAR: {
		auto it = strMap.find(treename);
		delete it->second;
		break;
	}
	}
}

ELEMENTTYPE IndexManager::search(const string& treename, const sql_value& val, const vector<sql_value>& vec) const
{
	switch (val.sql_type)
	{
	case INT: {
		auto it = intMap.find(treename);
		return it->second.search(vec, val.sql_int);
	}
	case FLOAT: {
		auto it = floatMap.find(treename);
		return it->second.search(vec, val.sql_float);
	}
	case CHAR: {
		auto it = strMap.find(treename);
		return it->second.search(vec, val.sql_str);
	}
	}
	return 0;
}

ELEMENTTYPE IndexManager::searchHead(const string& treename, const sql_value& val, const vector<sql_value>& vec) const
{
	switch (val.sql_type)
	{
	case INT: {
		auto it = intMap.find(treename);
		return it->second.searchHead();
	}
	case FLOAT: {
		auto it = floatMap.find(treename);
		return it->second.searchHead();
	}
	case CHAR: {
		auto it = strMap.find(treename);
		return it->second.searchHead();
	}
	}
}

ELEMENTTYPE IndexManager::searchNext(const string& treename, const sql_value& val, const vector<sql_value>& vec) const
{
	switch (val.sql_type)
	{
	case INT: {
		auto it = intMap.find(treename);
		return it->second.searchNext(vec, val.sql_int);
	}
	case FLOAT: {
		auto it = floatMap.find(treename);
		return it->second.searchNext(vec, val.sql_float);
	}
	case CHAR: {
		auto it = strMap.find(treename);
		return it->second.searchNext(vec, val.sql_str);
	}
	}
	return 0;
}

ELEMENTTYPE IndexManager::insert(const string& treename, const sql_value& val, vector<sql_value>& vec, queue<ELEMENTTYPE> que)
{
	switch (val.sql_type)
	{
	case INT: {
		auto it = intMap.find(treename);
		return it->second.insert(vec, que, val);
	}
	case FLOAT: {
		auto it = floatMap.find(treename);
		return it->second.insert(vec, que, val);
	}
	case CHAR: {
		auto it = strMap.find(treename);
		return it->second.insert(vec, que, val);
	}
	}
}

ELEMENTTYPE IndexManager::remove(const string& treename, const sql_value& val, vector<sql_value>& vec, queue<ELEMENTTYPE> que)
{
	switch (val.sql_type)
	{
	case INT: {
		auto it = intMap.find(treename);
		return it->second.remove(vec, que, val);
	}
	case FLOAT: {
		auto it = floatMap.find(treename);
		return it->second.remove(vec, que, val);
	}
	case CHAR: {
		auto it = strMap.find(treename);
		return it->second.remove(vec, que, val);
	}
	}
}

void IndexManager::save() {
	std::ofstream f(index_file);

	f << intMap.size() << " " << floatMap.size() << " " << strMap.size() << std::endl;
	for (auto& x : intMap)
		x.second.write_file(index_file, x.first);
	for (auto& x : floatMap)
		x.second.write_file(index_file, x.first);
	for (auto& x : strMap)
		x.second.write_file(index_file, x.first);
}

void IndexManager::load() {
	std::ifstream f(index_file);
	int int_sz, float_sz, str_sz;
	f >> int_sz >> float_sz >> str_sz >> std::endl;
	for (int i = 0; i < int_sz; ++i) {
		string file_info, treename;
		std::getline(f, file_info);
		auto int_bpt = BPTree<int>(file_info, 233, treename);
		intMap.insert(std::make_pair(treename, int_bpt));
	}
	for (int i = 0; i < float_sz; ++i) {
		string file_info, treename;
		std::getline(f, file_info);
		auto float_bpt = BPTree<int>(file_info, 233, treename);
		floatMap.insert(std::make_pair(treename, float_bpt));
	}
	for (int i = 0; i < str_sz; ++i) {
		string file_info, treename;
		std::getline(f, file_info);
		auto str_bpt = BPTree<int>(file_info, 233, treename);
		strMap.insert(std::make_pair(treename, str_bpt));
	}
}
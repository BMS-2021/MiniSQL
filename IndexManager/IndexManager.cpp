#include "IndexManager.h"

#include <fstream>

IndexManager::~IndexManager() {
  // TODO: destruct function
}

void IndexManager::create(const string& treename, const value_type& type)
{
	switch (type)
	{
	case value_type::INT:
		intmgr.insert(make_pair(treename,
			make_tuple(
				BPTree<int>(TREE_SIZE, TREE_DEGREE),
				vector<int>(),
				queue<ELEMENTTYPE>()
			)));
		break;
	case value_type::FLOAT:
		floatmgr.insert(make_pair(treename,
			make_tuple(
				BPTree<float>(TREE_SIZE, TREE_DEGREE),
				vector<float>(),
				queue<ELEMENTTYPE>()
			)));
		break;
	case value_type::CHAR:
		stringmgr.insert(make_pair(treename,
			make_tuple(
				BPTree<string>(TREE_SIZE, TREE_DEGREE),
				vector<string>(),
				queue<ELEMENTTYPE>()
			)));
		break;
	default:
		break;
	}
}

void IndexManager::create(const string& treename, const vector<sql_value>& indexs)
{
	switch (indexs.at(0).sql_type.type)
	{
	case value_type::INT: {
		auto idxtree = BPTree<int>(TREE_SIZE, TREE_DEGREE);
		auto vec = vector<int>();
		auto que = queue<ELEMENTTYPE>();
		for (auto& x : indexs)
			idxtree.insert(vec, que, x.sql_int);
		intmgr.insert(make_pair(treename, make_tuple(idxtree, vec, que)));
		break;
	}
	case value_type::FLOAT: {
		auto idxtree = BPTree<float>(TREE_SIZE, TREE_DEGREE);
		auto vec = vector<float>();
		auto que = queue<ELEMENTTYPE>();
		for (auto& x : indexs)
			idxtree.insert(vec, que, x.sql_float);
		floatmgr.insert(make_pair(treename, make_tuple(idxtree, vec, que)));
	}
	case value_type::CHAR: {
		auto idxtree = BPTree<string>(TREE_SIZE, TREE_DEGREE);
		auto vec = vector<string>();
		auto que = queue<ELEMENTTYPE>();
		for (auto& x : indexs)
			idxtree.insert(vec, que, x.sql_str);
		stringmgr.insert(make_pair(treename, make_tuple(idxtree, vec, que)));
	}
	default:
		break;
	}
}

void IndexManager::drop(const string& treename, const value_type& type)
{
	switch (type)
	{
	case value_type::INT:
		intmgr.erase(treename);
		break;
	case value_type::FLOAT:
		floatmgr.erase(treename);
		break;
	case value_type::CHAR:
		stringmgr.erase(treename);
		break;
	default:
		break;
	}
}

ELEMENTTYPE IndexManager::search(const string& treename, const sql_value& val) const
{
	switch (val.sql_type.type)
	{
	case value_type::INT: {
		auto it = intmgr.find(treename)->second;
		return get<0>(it).search(get<1>(it), val.sql_int);
	}
	case value_type::FLOAT: {
		auto it = floatmgr.find(treename)->second;
		return get<0>(it).search(get<1>(it), val.sql_float);
	}
	case value_type::CHAR: {
		auto it = stringmgr.find(treename)->second;
		return get<0>(it).search(get<1>(it), val.sql_str);
	}
	default:
		break;
	}
}

ELEMENTTYPE IndexManager::searchHead(const string& treename, const sql_value& val) const
{
	switch (val.sql_type.type)
	{
	case value_type::INT: {
		auto it = intmgr.find(treename)->second;
		return get<0>(it).searchHead();
	}
	case value_type::FLOAT: {
		auto it = floatmgr.find(treename)->second;
		return get<0>(it).searchHead();
	}
	case value_type::CHAR: {
		auto it = stringmgr.find(treename)->second;
		return get<0>(it).searchHead();
	}
	default:
		break;
	}
}

ELEMENTTYPE IndexManager::searchNext(const string& treename, const sql_value& val) const
{
	switch (val.sql_type.type)
	{
	case value_type::INT: {
		auto it = intmgr.find(treename)->second;
		return get<0>(it).searchNext(get<1>(it), val.sql_int);
	}
	case value_type::FLOAT: {
		auto it = floatmgr.find(treename)->second;
		return get<0>(it).searchNext(get<1>(it), val.sql_float);
	}
	case value_type::CHAR: {
		auto it = stringmgr.find(treename)->second;
		return get<0>(it).searchNext(get<1>(it), val.sql_str);
	}
	default:
		break;
	}
}

void IndexManager::insert(const string& treename, const sql_value& val)
{
	switch (val.sql_type.type)
	{
	case value_type::INT: {
		auto it = intmgr.find(treename)->second;
		get<0>(it).insert(get<1>(it), get<2>(it), val.sql_int);
		break;
	}
	case value_type::FLOAT: {
		auto it = floatmgr.find(treename)->second;
		get<0>(it).insert(get<1>(it), get<2>(it), val.sql_float);
		break;
	}
	case value_type::CHAR: {
		auto it = stringmgr.find(treename)->second;
		get<0>(it).insert(get<1>(it), get<2>(it), val.sql_str);
		break;
	}
	default:
		break;
	}
}

void IndexManager::remove(const string& treename, const sql_value& val)
{
	switch (val.sql_type.type)
	{
	case value_type::INT: {
		auto it = intmgr.find(treename)->second;
		get<0>(it).remove(get<1>(it), get<2>(it), val.sql_int);
		break;
	}
	case value_type::FLOAT: {
		auto it = floatmgr.find(treename)->second;
		get<0>(it).remove(get<1>(it), get<2>(it), val.sql_float);
		break;
	}
	case value_type::CHAR: {
		auto it = stringmgr.find(treename)->second;
		get<0>(it).remove(get<1>(it), get<2>(it), val.sql_str);
		break;
	}
	default:
		break;
	}
}


void IndexManager::save() {
	std::ofstream f(index_file);
	std::ofstream g(vecque_file);
	f << intmgr.size() << " " << floatmgr.size() << " " << stringmgr.size() << std::endl;
	for (auto& x : intmgr) {
		auto _map = get<0>(x.second);
		auto _vec = get<1>(x.second);
		auto _que = get<2>(x.second);
		_map.write_file(index_file, x.first);
		g << _vec.size() << " " << _que.size();
		for (auto& v : _vec)
			g << " " << v;
		while (!_que.empty()) {
			g << " " << _que.front();
			_que.pop();
		}
	}
	for (auto& x : floatmgr) {
        auto _map = get<0>(x.second);
        auto _vec = get<1>(x.second);
        auto _que = get<2>(x.second);
		_map.write_file(index_file, x.first);
		g << x.first << " " << _vec.size() << " " << _que.size();
		for (auto& v : _vec)
			g << " " << v;
		while (!_que.empty()) {
			g << " " << _que.front();
			_que.pop();
		}
	}
	for (auto& x : stringmgr) {
        auto _map = get<0>(x.second);
        auto _vec = get<1>(x.second);
        auto _que = get<2>(x.second);
		_map.write_file(index_file, x.first);
		g << x.first << " " << _vec.size() << " " << _que.size();
		for (auto& v : _vec)
			g << " " << v;
		while (!_que.empty()) {
			g << " " << _que.front();
			_que.pop();
		}
	}
}
void IndexManager::load() {
	std::ifstream f(index_file);
	std::ifstream g(vecque_file);
	int int_sz, float_sz, str_sz;
	f >> int_sz >> float_sz >> str_sz;
	for (int i = 0; i < int_sz; ++i) {
		string file_info, treename;
		std::getline(f, file_info);
		auto int_bpt = BPTree<int>(file_info, 233, treename);
		int vec_sz, que_sz;
		g >> vec_sz >> que_sz;
		vector<int> v;
		queue<ELEMENTTYPE> q;
		for (int i = 0; i < vec_sz; ++i) {
			int w; g >> w; v.push_back(w);
		}
		for (int i = 0; i < que_sz; ++i) {
			ELEMENTTYPE w; g >> w; q.push(w);
		}
		intmgr.insert(make_pair(treename, make_tuple(int_bpt, v, q)));
	}
	for (int i = 0; i < float_sz; ++i) {
		string file_info, treename;
		std::getline(f, file_info);
		auto float_bpt = BPTree<float>(file_info, 233, treename);
		int vec_sz, que_sz;
		g >> vec_sz >> que_sz;
		vector<float> v;
		queue<ELEMENTTYPE> q;
		for (int i = 0; i < vec_sz; ++i) {
			float w; g >> w; v.push_back(w);
		}
		for (int i = 0; i < que_sz; ++i) {
			ELEMENTTYPE w; g >> w; q.push(w);
		}
		floatmgr.insert(make_pair(treename, make_tuple(float_bpt, v, q)));
	}
	for (int i = 0; i < str_sz; ++i) {
		string file_info, treename;
		std::getline(f, file_info);
		auto string_bpt = BPTree<string>(file_info, 233, treename);
		int vec_sz, que_sz;
		g >> vec_sz >> que_sz;
		vector<string> v;
		queue<ELEMENTTYPE> q;
		for (int i = 0; i < vec_sz; ++i) {
			string w; g >> w; v.push_back(w);
		}
		for (int i = 0; i < que_sz; ++i) {
			ELEMENTTYPE w; g >> w; q.push(w);
		}
        stringmgr.insert(make_pair(treename, make_tuple(string_bpt, v, q)));
	}
}
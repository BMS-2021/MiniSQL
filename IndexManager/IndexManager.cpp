#include "IndexManager.h"

#include <fstream>

IndexManager::~IndexManager() {
  // TODO: destruct function
}

void IndexManager::create(const string& treename) {
  index_manager.insert(make_pair(treename, BPTree(TREE_SIZE, TREE_DEGREE)));
}

void IndexManager::create(const string& treename,
                          const vector<sql_value>& indexs) {
  //    auto idxtree = BPTree(TREE_SIZE, TREE_DEGREE);
  //    auto vec = vector<int>();
  //    auto que = queue<ELEMENTTYPE>();
  //    for (auto &x : indexs)
  //        idxtree.insert(x.sql_int);
  //    intmgr.insert(make_pair(treename, make_tuple(idxtree, vec, que)));
  //    break;
}

void IndexManager::drop(const string& treename) {
  index_manager.erase(treename);
}

ELEMENTTYPE IndexManager::search(const string& treename,
                                 const sql_value& val,
                                 uint32_t idx_pos,
                                 const macro::table& table) const {
  return index_manager.find(treename)->second.search(idx_pos, table, val);
}

ELEMENTTYPE IndexManager::searchHead(const string& treename,
                                     const sql_value& val) const {
  return index_manager.find(treename)->second.searchHead();
}

ELEMENTTYPE IndexManager::searchNext(const string& treename,
                                     const sql_value& val,
                                     uint32_t idx_pos,
                                     const macro::table& table) const {
  return index_manager.find(treename)->second.searchNext(idx_pos, table, val);
}

void IndexManager::insert(const string& treename,
                          const sql_value& val,
                          uint32_t idx_pos,
                          const macro::table& table,
                          ELEMENTTYPE new_key) {
  index_manager.find(treename)->second.insert(idx_pos, table, new_key, val);
}

void IndexManager::remove(const string& treename,
                          const sql_value& val,
                          uint32_t idx_pos,
                          const macro::table& table) {
  index_manager.find(treename)->second.remove(idx_pos, table, val);
}

void IndexManager::save() {
  std::ofstream f(index_file);
  f << index_manager.size() << std::endl;
  for (auto& x : index_manager) {
    x.second.write_file(index_file, x.first);
  }
}

void IndexManager::load() {
  std::ifstream f(index_file);
  int map_sz;
  f >> map_sz;
  for (int i = 0; i < map_sz; ++i) {
    string treestr, name;
    f >> treestr;
    auto tree = BPTree(treestr, TREE_SIZE, name);
    index_manager.insert(make_pair(name, tree));
  }
}

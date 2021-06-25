#include "IndexManager.h"

#include <fstream>

IndexManager::~IndexManager() {
  // TODO: destruct function
}

void IndexManager::create(const string& treename) {
  index_manager.emplace(treename, std::make_unique<BPTree>(TREE_SIZE, TREE_DEGREE));
}

void IndexManager::create(const string& treename,
                          const vector<std::pair<uint32_t, sql_value>>& indexs,
                          uint32_t idx_pos,
                          const macro::table table) {
  auto idxtree = std::make_unique<BPTree>(TREE_SIZE, TREE_DEGREE);

  for (auto& x : indexs)

    idxtree->insert(idx_pos, table, x.first, x.second);
  index_manager.emplace(treename, std::move(idxtree));
}

void IndexManager::drop(const string& treename) {
  index_manager.erase(treename);
}

ELEMENTTYPE IndexManager::search(const string& treename,
                                 const sql_value& val,
                                 uint32_t idx_pos,
                                 const macro::table& table) const {
  return index_manager.find(treename)->second->search(idx_pos, table, val);
}

ELEMENTTYPE IndexManager::searchHead(const string& treename,
                                     const sql_value& val) const {
  return index_manager.find(treename)->second->searchHead();
}

ELEMENTTYPE IndexManager::searchNext(const string& treename,
                                     const sql_value& val,
                                     uint32_t idx_pos,
                                     const macro::table& table) const {
  return index_manager.find(treename)->second->searchNext(idx_pos, table, val);
}

void IndexManager::insert(const string& treename,
                          const sql_value& val,
                          uint32_t idx_pos,
                          const macro::table& table,
                          ELEMENTTYPE new_key) {
  index_manager.find(treename)->second->insert(idx_pos, table, new_key, val);
}

void IndexManager::remove(const string& treename,
                          const sql_value& val,
                          uint32_t idx_pos,
                          const macro::table& table,
                          std::unordered_map<uint32_t, sql_tuple> umap) {
  index_manager.find(treename)->second->remove(idx_pos, table, val, umap);
}

void IndexManager::save() {
  std::ofstream f(index_file);
  f << index_manager.size() << std::endl;
  for (auto& x : index_manager) {
    x.second->write_file(index_file, x.first);
  }
}

void IndexManager::load() {
  std::ifstream f(index_file);
  if (!f)
    return;
  int map_sz;
  string map_sz_str;
  std::getline(f, map_sz_str);
  map_sz = std::stoi(map_sz_str);
  for (int i = 0; i < map_sz; ++i) {
    string treestr, name;
    std::getline(f, treestr);
    auto tree = std::make_unique<BPTree>(treestr, TREE_SIZE, name);
    index_manager.emplace(name, std::move(tree));
  }
}

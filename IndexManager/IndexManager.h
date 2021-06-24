#include "../macro.h"
#include "BPTree.h"

#include <map>
#include <string>
using std::make_pair;
using std::map;
using std::string;

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
  void IndexManager::create(
      const string& treename,
      const vector<std::pair<uint32_t, sql_value>>& indexs,
      uint32_t idx_pos,
      const macro::table table);
  void drop(const string& treename);

  ELEMENTTYPE search(const string& treename,
                     const sql_value& val,
                     uint32_t idx_pos,
                     const macro::table& table) const;
  ELEMENTTYPE searchHead(const string& treename, const sql_value& val) const;
  ELEMENTTYPE searchNext(const string& treename,
                         const sql_value& val,
                         uint32_t idx_pos,
                         const macro::table& table) const;

  void insert(const string& treename,
              const sql_value& val,
              uint32_t idx_pos,
              const macro::table& table,
              ELEMENTTYPE new_key);
  void remove(const string& treename,
              const sql_value& val,
              uint32_t idx_pos,
              const macro::table& table);

  void save();
  void load();

 private:
  map<string, BPTree> index_manager;
};

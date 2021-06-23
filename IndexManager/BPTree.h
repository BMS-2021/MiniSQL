#pragma once

#include "../utils/exception.h"
#include "BPInclude.h"
#include "BPTreeNode.h"

#include <fstream>
#include <sstream>

using std::string;

class BPTree {
 public:
  void insert(uint32_t idx_pos,
              const macro::table& table,
              ELEMENTTYPE rec_id,
              const sql_value& target);

  ELEMENTTYPE search(uint32_t idx_pos,
                     const macro::table& table,
                     const sql_value& target) const;

  void remove(uint32_t idx_pos,
              const macro::table& table,
              const sql_value& target);

  void write_file(string filename, string treename);

  ELEMENTTYPE searchHead() const;

  ELEMENTTYPE searchNext(uint32_t idx_pos,
                         const macro::table& table,
                         const sql_value& target) const;

  BPTree() = default;

  ~BPTree();

  BPTree(uint32_t max_size, uint32_t deg);

  BPTree(string treestr, uint32_t max_size, string& treename);

 private:
  BPTreeNode* root = nullptr;
  uint32_t degree;
  uint32_t size;

  BPTreeNode* pool = nullptr;
  queue<uint32_t> deleted_id;

  BPTreeNode* new_node();

  void delete_node(BPTreeNode* node);
};

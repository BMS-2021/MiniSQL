//
// Created by ralxyz on 6/22/21.
//

#include "../RecordManager/RecordManager.h"
#include "BPTree.h"

extern RecordManager rec_mgt;

sql_value find_idx_val(ELEMENTTYPE rec_id,
                       uint32_t idx_pos,
                       const macro::table& table) {
  return rec_mgt.getRecord(table, rec_id).element.at(idx_pos);
}

sql_value find_idx_val(ELEMENTTYPE rec_id,
                       uint32_t idx_pos,
                       const macro::table& table,
                       std::unordered_map<uint32_t, sql_tuple> umap) {
  auto res = umap.find(rec_id);
  return res == umap.end() ? find_idx_val(rec_id, idx_pos, table)
                           : res->second.element.at(idx_pos);
}

uint32_t BPTreeNode::binary_search(uint32_t idx_pos,
                                   const macro::table& table,
                                   const sql_value& target) const {
  int l = 0, r = key.size() - 1;
  while (l <= r) {
    int mid = (l + r) / 2;
    if (find_idx_val(key.at(mid), idx_pos, table) <= target)
      l = mid + 1;
    else
      r = mid - 1;
  }
  return l;
}

uint32_t BPTreeNode::binary_search(
    uint32_t idx_pos,
    const macro::table& table,
    const sql_value& target,
    std::unordered_map<uint32_t, sql_tuple> umap) const {
  int l = 0, r = key.size() - 1;
  while (l <= r) {
    int mid = (l + r) / 2;
    if (find_idx_val(key.at(mid), idx_pos, table, umap) <= target)
      l = mid + 1;
    else
      r = mid - 1;
  }
  return l;
}
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
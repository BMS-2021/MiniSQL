//
// Created by ralxyz on 6/22/21.
//

#include "BPTree.h"
#include "../RecordManager/RecordManager.h"

extern RecordManager rec_mgt;

sql_value find_idx_val(ELEMENTTYPE rec_id, uint32_t idx_pos)
{
    return rec_mgt.getRecord(rec_id).element.at(idx_pos);
}

uint32_t BPTreeNode::binary_search(uint32_t idx_pos, const sql_value& target) const
{
    int l = 0, r = key.size() - 1;
    while (l <= r) {
        int mid = (l + r) / 2;
        if (find_idx_val(key.at(mid), idx_pos) <= target)
            l = mid + 1;
        else r = mid - 1;
    }
    return l;
}
#pragma once

#include "../macro.h"
#include "BPInclude.h"

sql_value find_idx_val(ELEMENTTYPE rec_id, uint32_t idx_pos);

class BPTreeNode
{
public:
	BPTreeNode* fa, * nxt;
	vector<BPTreeNode*> ch;
	vector<ELEMENTTYPE> key;

	// for leaf node: insert pos
	// for non-leaf node: which child is in
    uint32_t binary_search(uint32_t idx_pos, const sql_value& target) const;
};



#include "BPTree.h"

inline sql_value BPTree::find_idx_val(ELEMENTTYPE rec_id, uint32_t idx_pos)
{
	auto idx = rec_mgt.getRecord(rec_id).element.at(idx_pos);
	switch (idx.sql_type) {
	case value_type::INT:
		return idx.sql_int;
	case value_type::FLOAT:
		return idx.sql_float;
	case value_type::CHAR:
		return idx.sql_str;
	}
}

void insert(uint32_t idx_pos, ELEMENTTYPE rec_id, const void insert(uint32_t idx_pos, ELEMENTTYPE rec_id, const sql_value& target) {
    /*
    Case 1: not full -> directly add
    Case 2: full -> split
            1) split at middle
            2) move second half to sibling
            3) set split point to fa
            4) arrange pointer
            5) solve fa recursively
    */
    BPTreeNode *cur = root;
    if (cur == nullptr) {  // no root
        cur = new_node();
        cur->fa = nullptr;
        root = cur;
    }
    while (cur->ch.size() > 0 && cur->ch.at(0) != nullptr) {
        auto i = 0U;
		for (; i < cur->key.size(); ++i)
			if (find_idx_val(cur->key.at(i), idx_pos) > target)
				break;
        cur = cur->ch[i];
    }  // find until leaf
	ELEMENTTYPE new_key = rec_id;
    T tar = target;  // target value (actually to find, will change when split up)
    BPTreeNode<T> *child = nullptr;

    while (true) {                              // from botton to top
        auto tmp = cur->binary_search(tab, tar);  // find pos to insert
        if (tmp >= cur->key.size()) {
            cur->key.push_back(new_key);
            cur->ch.push_back(child);
        } else {
            cur->key.insert(cur->key.begin() + tmp, new_key);
            cur->ch.insert(cur->ch.begin() + tmp + (cur->ch.at(0) != nullptr), child);
        }
        if (cur->ch.size() <= degree)  // degree(leaf) | degree-1(non-leaf)
            break;                       // no need to split

        /* split */
        int non_leaf = cur->ch.at(0) != nullptr;  // actually use bool as 0/1
        uint32_t split_pos = (degree + 1) / 2 -
                             (non_leaf);  // split at deg/2(leaf) | deg/2-1(non-leaf
        BPTreeNode<T> *sibling = new_node();
        BPTreeNode<T> *fa;

        // arrange pointer
        if (cur->fa == nullptr) {
            fa = new_node();
            fa->fa = nullptr;
            fa->ch.push_back(cur);
            cur->fa = root = fa;
        } else {
            fa = cur->fa;
        }
        sibling->fa = fa;

        // update value to update
        new_key = cur->key.at(split_pos);
        tar = tab.at(new_key);
        child = sibling;

        // modify value
        for (auto i = split_pos + non_leaf; i < cur->key.size(); ++i)
            sibling->key.push_back(cur->key.at(i));
        for (auto i = split_pos + non_leaf; i < cur->ch.size(); ++i) {
            sibling->ch.push_back(cur->ch.at(i));
            if (cur->ch.at(i) != nullptr)
                cur->ch.at(i)->fa = sibling;
        }
        cur->key.erase(cur->key.begin() + split_pos, cur->key.end());
        cur->ch.erase(cur->ch.begin() + split_pos + non_leaf, cur->ch.end());

        sibling->nxt = cur->nxt;
        cur->nxt = sibling;

        // recursive
        cur = fa;
    }
}& target) {
	/*
	Case 1: not full -> directly add
	Case 2: full -> split
			1) split at middle
			2) move second half to sibling
			3) set split point to fa
			4) arrange pointer
			5) solve fa recursively
	*/
	BPTreeNode* cur = root;
	if (cur == nullptr) {  // no root
		cur = new_node();
		cur->fa = nullptr;
		root = cur;
	}
	while (cur->ch.size() > 0 && cur->ch.at(0) != nullptr) {
		auto i = 0U;
		for (; i < cur->key.size(); ++i)
			if (find_idx)
				cur = cur->ch[i];
	}  // find until leaf
	ELEMENTTYPE new_key = rec_id;
	T tar = target;  // target value (actually to find, will change when split up)
	BPTreeNode<T>* child = nullptr;

	while (true) {                              // from botton to top
		auto tmp = cur->binary_search(tab, tar);  // find pos to insert
		if (tmp >= cur->key.size()) {
			cur->key.push_back(new_key);
			cur->ch.push_back(child);
		}
		else {
			cur->key.insert(cur->key.begin() + tmp, new_key);
			cur->ch.insert(cur->ch.begin() + tmp + (cur->ch.at(0) != nullptr), child);
		}
		if (cur->ch.size() <= degree)  // degree(leaf) | degree-1(non-leaf)
			break;                       // no need to split

		/* split */
		int non_leaf = cur->ch.at(0) != nullptr;  // actually use bool as 0/1
		uint32_t split_pos = (degree + 1) / 2 -
			(non_leaf);  // split at deg/2(leaf) | deg/2-1(non-leaf
		BPTreeNode<T>* sibling = new_node();
		BPTreeNode<T>* fa;

		// arrange pointer
		if (cur->fa == nullptr) {
			fa = new_node();
			fa->fa = nullptr;
			fa->ch.push_back(cur);
			cur->fa = root = fa;
		}
		else {
			fa = cur->fa;
		}
		sibling->fa = fa;

		// update value to update
		new_key = cur->key.at(split_pos);
		tar = tab.at(new_key);
		child = sibling;

		// modify value
		for (auto i = split_pos + non_leaf; i < cur->key.size(); ++i)
			sibling->key.push_back(cur->key.at(i));
		for (auto i = split_pos + non_leaf; i < cur->ch.size(); ++i) {
			sibling->ch.push_back(cur->ch.at(i));
			if (cur->ch.at(i) != nullptr)
				cur->ch.at(i)->fa = sibling;
		}
		cur->key.erase(cur->key.begin() + split_pos, cur->key.end());
		cur->ch.erase(cur->ch.begin() + split_pos + non_leaf, cur->ch.end());

		sibling->nxt = cur->nxt;
		cur->nxt = sibling;

		// recursive
		cur = fa;
	}
}
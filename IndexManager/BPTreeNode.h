#pragma once

#include "BPInclude.h"

template<typename T>
class BPTreeNode
{
public:
	BPTreeNode *fa, *nxt;
	vector<BPTreeNode*> ch;
	vector<ELEMENTTYPE> key;

	// for leaf node: insert pos
	// for non-leaf node: which child is in
	uint32_t binary_search(const vector<T>& tab, const T& target) const;
};

template<typename T>
inline uint32_t BPTreeNode<T>::binary_search(const vector<T>& tab, const T& target) const
{
	int l = 0, r = key.size() - 1;
	while (l <= r) {
		int mid = (l + r) / 2;
		if (tab.at(key.at(mid)) <= target)
			l = mid + 1;
		else r = mid - 1;
	}
	return l;
}
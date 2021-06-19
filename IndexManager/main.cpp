#include "BPTree.h"

#include <iostream>

int main() {
	BPTree<int> t = BPTree<int>(100, 3);
	
	vector<int> p;
	queue<ELEMENTTYPE> q;
	t.insert(p, q, 1);
	t.insert(p, q, 2);
	t.insert(p, q, 3);
	t.insert(p, q, 4);
	t.remove(p, q, 4);
	t.remove(p, q, 3);
	t.remove(p, q, 2);
	
	auto m = t.search(p, 1);
	std::cout << m;
}
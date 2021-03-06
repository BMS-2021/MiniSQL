#include "BPTree.h"
#include "../RecordManager/RecordManager.h"

extern RecordManager rec_mgt;

void BPTree::insert(uint32_t idx_pos,
                    const macro::table& table,
                    ELEMENTTYPE rec_id,
                    const sql_value& target) {
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
  while (!cur->ch.empty() && cur->ch.at(0) != nullptr) {
    auto i = 0U;
    for (; i < cur->key.size(); ++i)
      if (find_idx_val(cur->key.at(i), idx_pos, table) > target)
        break;
    cur = cur->ch[i];
  }  // find until leaf

  ELEMENTTYPE new_key = rec_id;
  sql_value tar =
      target;  // target value (actually to find, will change when split up)
  BPTreeNode* child = nullptr;

  while (true) {                                         // from botton to top
    auto tmp = cur->binary_search(idx_pos, table, tar);  // find pos to insert
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
    BPTreeNode* sibling = new_node();
    BPTreeNode* fa;

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
    tar = find_idx_val(new_key, idx_pos, table);
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

ELEMENTTYPE BPTree::search(uint32_t idx_pos,
                           const macro::table& table,
                           const sql_value& target) const {
  BPTreeNode* cur = root;
  if (cur == nullptr) {
    throw sql_exception(300, "index manager", "element not found");
  }
  while (cur->ch.at(0) != nullptr) {
    auto i = 0U;
    for (; i < cur->key.size(); ++i)
      if (find_idx_val(cur->key.at(i), idx_pos, table) > target)
        break;
    cur = cur->ch[i];
  }
  auto tmp = cur->binary_search(idx_pos, table, target);
  if (tmp == 0 ||
      find_idx_val(cur->key.at(tmp - 1), idx_pos, table) != target) {
    throw sql_exception(300, "index manager", "element not found");
  }
  return cur->key.at(tmp - 1);
}

void BPTree::remove(uint32_t idx_pos,
                    const macro::table& table,
                    const sql_value& target,
                    std::unordered_map<uint32_t, sql_tuple> umap) {
  /*
  Case I. key not appear in internal node
          1. size enough -> directly delete
          2. size not enough -> rotate
  Case II. key appear in internal node
          1. size enough -> delete, replace with successor
`		2. size not enough -> rotate
          3. rotate not enough -> merge
  Case III. height-- (only key in root is deleted)

  TODO:
          1. delete node v
          2. s <- get successor
          recursively:
          1. replace v with s if not deleted
          2. if size enough "break"
          3. check neighbour
          4. if either neighbour enough -> rotate
                  lrotate:
                          x <- rneighbour.popfront
                          cur.pushback:x
                          fa.key[get_wh:rneighbour] <- rneightbour.front
                  rrotate:
                          x <- lneighbour.popback
                          if non-leaf:cur
                                  cur.pushfront:fa.key
                          else
                                  cur.pushfront:x
                          fa.key[get_wh:cur] <- x
          5. else merge
                  replace fa.key with s if fa.key=v
                  lmerge:
                          if nonleaf:cur cur.key.pushfront:fa.key
                          fa.erase:cur
                          mv cur to lneighbour.end
                  rmerge:
                          if nonleaf:cur cur.key.pushback:fa.key
                          fa.erase:cur
                          mv cur to rneighbour.front
                  if empty:rt.key rt=neighbour
  */

  BPTreeNode* cur = root;
  if (cur == nullptr) {
    throw sql_exception(300, "index manager", "element not found");
  }
  while (cur->ch.at(0) != nullptr) {
    auto i = 0U;
    for (; i < cur->key.size(); ++i)
      if (find_idx_val(cur->key.at(i), idx_pos, table, umap) > target)
        break;
    cur = cur->ch[i];
  }
  auto tmp = cur->binary_search(idx_pos, table, target, umap);
  if (tmp == 0 ||
      find_idx_val(cur->key.at(tmp - 1), idx_pos, table, umap) != target) {
    throw sql_exception(300, "index manager", "element not found");
  }
  // throw std::exception("Fix Me: Deletion Not Implemented");
  auto suc = cur->key.at(tmp - 1);
  if (tmp == cur->key.size()) {
    if (cur->nxt != nullptr)
      suc = cur->nxt->key.at(0);
  } else
    suc = cur->key.at(tmp);

  cur->key.erase(cur->key.begin() + tmp - 1);
  cur->ch.erase(cur->ch.begin() + tmp - 1);

  while (true) {
    for (auto i = 0U; i < cur->key.size(); ++i)
      if (find_idx_val(cur->key.at(i), idx_pos, table, umap) == target) {
        cur->key.at(i) = suc;
        break;
      }
    if (cur == root || cur->ch.size() >= degree / 2) {
      if (root->key.empty()) {
        delete_node(root);
        root = nullptr;
      }
      break;
    }

    auto get_wh = cur->fa->binary_search(idx_pos, table, target, umap);
    BPTreeNode *lsib, *rsib;
    bool is_leaf = cur->ch.size() == cur->key.size();

    if (get_wh > 0)
      lsib = cur->fa->ch.at(get_wh - 1);
    else
      lsib = nullptr;
    if (get_wh < cur->fa->ch.size() - 1)
      rsib = cur->fa->ch.at(get_wh + 1);
    else
      rsib = nullptr;

    if (lsib != nullptr && lsib->key.size() > degree / 2) {  // if left enough
      // rrotate
      auto pivot_key = lsib->key.at(lsib->key.size() - 1);
      auto pivot_ch = lsib->ch.at(lsib->ch.size() - 1);

      if (is_leaf)
        cur->key.insert(cur->key.begin(), pivot_key);
      else
        cur->key.insert(cur->key.begin(), cur->fa->key.at(get_wh - 1));
      cur->ch.insert(cur->ch.begin(), pivot_ch);
      cur->fa->key.at(get_wh - 1) = pivot_key;
      lsib->key.pop_back();
      lsib->ch.pop_back();
    } else if (rsib != nullptr &&
               rsib->key.size() > degree / 2) {  // if right enough
                                                 // lrotate
      auto pivot_key = rsib->key.at(0);
      auto pivot_ch = rsib->ch.at(0);

      if (is_leaf)
        cur->key.push_back(pivot_key);
      else
        cur->key.push_back(cur->fa->key.at(get_wh));
      cur->ch.push_back(pivot_ch);
      cur->fa->key.at(get_wh) = rsib->key.at(1);
      rsib->key.erase(rsib->key.begin());
      rsib->ch.erase(rsib->ch.begin());
    } else {  // neither is enough
      if (lsib == nullptr) {
        lsib = cur;
        cur = rsib;  // lmerge rsib as rmerge
        ++get_wh;
      }
      // lmerge
      if (!is_leaf)
        cur->key.insert(cur->key.begin(), cur->fa->key.at(get_wh - 1));
      cur->fa->key.erase(cur->fa->key.begin() + get_wh - 1);
      cur->fa->ch.erase(cur->fa->ch.begin() + get_wh);
      for (auto& i : cur->key)
        lsib->key.push_back(i);
      for (auto& i : cur->ch) {
        lsib->ch.push_back(i);
        if (i != nullptr)
          i->fa = lsib;
      }
      delete_node(cur);
      cur = lsib;
    }

    if (root->key.empty()) {  // if root is empty
      delete_node(root);
      root = cur;
      return;
    }
    cur = cur->fa;
  }
}

void BPTree::write_file(string filename, string treename) {
  std::ofstream f(filename, std::ios::app);
  f << deleted_id.size() << " ";
  while (!deleted_id.empty()) {
    f << deleted_id.front() << " ";
    deleted_id.pop();
  }
  f << size << " " << degree << " ";
  for (int i = 0; i < size; ++i) {
    BPTreeNode* node = pool + i;
    f << i << " ";
    if (node->fa != nullptr)
      f << node->fa - pool;
    else
      f << -1;
    f << " " << node->key.size() << " " << node->ch.size() << " ";
    if (node->nxt != nullptr)
      f << node->nxt - pool << " ";
    else
      f << "-1 ";
    for (auto& j : node->key)
      f << j << " ";
    for (auto j : node->ch)
      f << (j == nullptr ? -1 : j - pool) << " ";
  }
  f << (root == nullptr ? -1 : root - pool);
  f << " " << treename << std::endl;
  f.close();
}

ELEMENTTYPE BPTree::searchHead() const {
  BPTreeNode* cur = root;
  if (cur->ch.empty()) {
    throw sql_exception(301, "index manager", "tree is empty");
  }
  while (cur->ch.at(0) != nullptr)
    cur = cur->ch.at(0);
  return cur->key.at(0);
}

ELEMENTTYPE BPTree::searchNext(uint32_t idx_pos,
                               const macro::table& table,
                               const sql_value& target) const {
  BPTreeNode* cur = root;
  if (cur == nullptr) {
    throw sql_exception(300, "index manager", "element not found");
  }
  while (cur->ch.at(0) != nullptr) {
    auto i = 0U;
    for (; i < cur->key.size(); ++i)
      if (find_idx_val(cur->key.at(i), idx_pos, table) > target)
        break;
    cur = cur->ch[i];
  }
  auto tmp = cur->binary_search(idx_pos, table, target);
  if (tmp == 0 ||
      find_idx_val(cur->key.at(tmp - 1), idx_pos, table) != target) {
    throw sql_exception(300, "index manager", "element not found");
  }
  if (tmp == cur->key.size()) {
    if (cur->nxt == nullptr)
      throw sql_exception(302, "index manager", "no next element");
    return cur->nxt->key.at(0);
  }
  return cur->key.at(tmp);
}

BPTree::BPTree(uint32_t max_size, uint32_t deg) {
  degree = deg;
  pool = new BPTreeNode[max_size];
  size = 0;
}

BPTree::~BPTree() {
  delete[] pool;
}

BPTree::BPTree(string treestr, uint32_t max_size, string& treename) {
  std::istringstream stream(treestr);
  int qsize;
  stream >> qsize;
  for (int i = 0; i < qsize; ++i) {
    ELEMENTTYPE qnum;
    stream >> qnum;
    deleted_id.push(qnum);
  }
  int tsize, deg;
  stream >> tsize >> deg;
  size = tsize;
  degree = deg;
  pool = new BPTreeNode[max_size];
  for (int i = 0; i < size; ++i) {
    BPTreeNode* node = pool + i;
    int id, fa_id, key_sz, ch_sz, nxt_id;
    stream >> id >> fa_id >> key_sz >> ch_sz >> nxt_id;
    node->fa = fa_id >= 0 ? pool + fa_id : nullptr;
    node->nxt = nxt_id >= 0 ? pool + nxt_id : nullptr;
    for (int j = 0; j < key_sz; ++j) {
      int key_num;
      stream >> key_num;
      node->key.push_back(key_num);
    }
    for (int j = 0; j < ch_sz; ++j) {
      int ch_num;
      stream >> ch_num;
      if (ch_num >= 0)
        node->ch.push_back(pool + ch_num);
      else
        node->ch.push_back(nullptr);
    }
  }
  int rt;
  stream >> rt;
  if (rt != -1)
    root = pool + rt;
  else
    root = nullptr;
  stream >> treename;
}

BPTreeNode* BPTree::new_node() {
  if (!deleted_id.empty()) {
    auto id = deleted_id.front();
    deleted_id.pop();
    ++size;
    return pool + id;
  }
  BPTreeNode* p = pool + (size++);
  p->fa = p->nxt = nullptr;
  return p;
}

void BPTree::delete_node(BPTreeNode* node) {
  deleted_id.push(node - pool);
}
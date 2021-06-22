#pragma once

#include "BPInclude.h"
#include "BPTreeNode.h"
#include "../utils/exception.h"

#include <fstream>
#include <sstream>

using std::string;

extern RecordManager rec_mgt;

class BPTree {
public:
    void insert(uint32_t idx_pos, ELEMENTTYPE rec_id, const sql_value&target);

    ELEMENTTYPE search(uint32_t idx_pos, const sql_value& target) const;

    void remove(uint32_t idx_pos, ELEMENTTYPE rec_id, const sql_value&target);

    void write_file(string filename, string treename);

    ELEMENTTYPE searchHead() const;

    ELEMENTTYPE searchNext(const sql_value& target) const;

    BPTree() = default;

    ~BPTree();

    BPTree(uint32_t max_size, uint32_t deg);

    BPTree(string treestr, uint32_t max_size, string &treename);

private:
    BPTreeNode *root;
    uint32_t degree;
    uint32_t size;

    BPTreeNode *pool;
    queue<uint32_t> deleted_id;

    BPTreeNode *new_node();

	sql_value find_idx_val(ELEMENTTYPE rec_id, uint32_t idx_pos);

	void delete_node(BPTreeNode* node);
};



template<typename T>
ELEMENTTYPE BPTree<T>::search(const vector<T> &tab, const T &target) const {
    BPTreeNode<T> *cur = root;
    if (cur == nullptr) {
        throw sql_exception(300, "index manager", "element not found");
    }
    while (cur->ch.at(0) != nullptr) {
        auto i = 0U;
        for (; i < cur->key.size(); ++i)
            if (tab.at(cur->key.at(i)) > target)
                break;
        cur = cur->ch[i];
    }
    auto tmp = cur->binary_search(tab, target);
    if (tmp == 0 || tab.at(cur->key.at(tmp - 1)) != target) {
        throw sql_exception(300, "index manager", "element not found");
    }
    return cur->key.at(tmp - 1);
}

template<typename T>
void BPTree<T>::remove(vector<T> &tab,
                       queue<ELEMENTTYPE> &que,
                       const T &target) {
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

    BPTreeNode<T> *cur = root;
    if (cur == nullptr) {
        throw sql_exception(300, "index manager", "element not found");
    }
    while (cur->ch.at(0) != nullptr) {
        auto i = 0U;
        for (; i < cur->key.size(); ++i)
            if (tab.at(cur->key.at(i)) > target)
                break;
        cur = cur->ch[i];
    }
    auto tmp = cur->binary_search(tab, target);
    if (tmp == 0 || tab.at(cur->key.at(tmp - 1)) != target) {
        throw sql_exception(300, "index manager", "element not found");
    }
    que.push(cur->key.at(tmp - 1));
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
            if (tab.at(cur->key.at(i)) == target) {
                cur->key.at(i) = suc;
                break;
            }
        if (cur == root || cur->ch.size() >= degree / 2)
            break;

        auto get_wh = cur->fa->binary_search(tab, target);
        BPTreeNode<T> *lsib, *rsib;
        bool is_leaf = cur->ch.size() == cur->key.size();

        if (get_wh > 0)
            lsib = cur->fa->ch.at(get_wh - 1);
        else
            lsib = nullptr;
        if (get_wh < cur->fa->ch.size() - 1)
            rsib = cur->fa->ch.at(get_wh + 1);
        else
            rsib = nullptr;

        if (lsib != nullptr &&
            lsib->ch.size() > (degree + is_leaf - 1) / 2) {  // if left enough
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
        } else if (rsib != nullptr && rsib->ch.size() > (degree + is_leaf - 1) /
                                                        2) {  // if right enough
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
            for (auto &i : cur->key)
                lsib->key.push_back(i);
            for (auto &i : cur->ch)
                lsib->ch.push_back(i);
            delete_node(cur);
            cur = lsib;
        }

        if (root->key.size() < 1) {  // if root is empty
            delete_node(root);
            root = cur;
            return;
        }
        cur = cur->fa;
    }
}

template<typename T>
inline void BPTree<T>::write_file(string filename, string treename) {
    std::ofstream f(filename, std::ios::app);
    f << deleted_id.size() << " ";
    while (!deleted_id.empty()) {
        f << deleted_id.front() << " ";
        deleted_id.pop();
    }
    f << size << " " << degree << " ";
    for (int i = 0; i < size; ++i) {
        BPTreeNode<T> *node = pool + i;
        f << i << " ";
        if (node->fa != nullptr)
            f << node->fa - pool;
        else
            f << -1;
        f << " " << node->key.size() << " " << node->ch.size() << " ";
        if (node->nxt != nullptr)
            f << node->nxt - pool << " ";
        else f << "-1 ";
        for (auto &j : node->key)
            f << j << " ";
        for (auto j : node->ch)
            f << (j == nullptr ? -1 : j - pool) << " ";
    }
    f << (root == nullptr ? -1 : root - pool);
    f << " " << treename << std::endl;
    f.close();
}

template<typename T>
inline ELEMENTTYPE BPTree<T>::searchHead() const {
    BPTreeNode<T> *cur = root;
    if (cur->ch.empty()) {
        throw sql_exception(301, "index manager", "tree is empty");
    }
    while (cur->ch.at(0) != nullptr)
        cur = cur->ch.at(0);
    return cur->key.at(0);
}

template<typename T>
inline ELEMENTTYPE BPTree<T>::searchNext(const vector<T> &tab, const T &target) const {
    BPTreeNode<T> *cur = root;
    if (cur == nullptr) {
        throw sql_exception(300, "index manager", "element not found");
    }
    while (cur->ch.at(0) != nullptr) {
        auto i = 0U;
        for (; i < cur->key.size(); ++i)
            if (tab.at(cur->key.at(i)) > target)
                break;
        cur = cur->ch[i];
    }
    auto tmp = cur->binary_search(tab, target);
    if (tmp == 0 || tab.at(cur->key.at(tmp - 1)) != target) {
        throw sql_exception(300, "index manager", "element not found");
    }
    if (tmp == cur->key.size()) {
        if (cur->nxt == nullptr)
            throw sql_exception(302, "index manager", "no next element");
        return cur->nxt->key.at(0);
    }
    return cur->key.at(tmp);
}

template<typename T>
inline BPTree<T>::~BPTree() {
    delete pool;
}

template<typename T>
BPTree<T>::BPTree(uint32_t max_size, uint32_t deg) {
    degree = deg;
    pool = new BPTreeNode<T>[max_size];
    size = 0;
}

template<typename T>
inline BPTree<T>::BPTree(string treestr, uint32_t max_size, string &treename) {
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
    pool = new BPTreeNode<T>[max_size];
    for (int i = 0; i < size; ++i) {
        BPTreeNode<T> *node = pool + i;
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
            else node->ch.push_back(nullptr);
        }
    }
    int rt;
    stream >> rt;
    if (rt != -1) root = pool + rt;
    else root = nullptr;
    stream >> treename;
}

template<typename T>
BPTreeNode<T> *BPTree<T>::new_node() {
    if (!deleted_id.empty()) {
        auto id = deleted_id.front();
        deleted_id.pop();
        ++size;
        return pool + id;
    }
    BPTreeNode<T> *p = pool + (size++);
    p->fa = p->nxt = nullptr;
    return p;
}

template<typename T>
inline void BPTree<T>::delete_node(BPTreeNode<T> *node) {
    deleted_id.push(node - pool);
}


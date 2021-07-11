# IndexManager

As what is just mentioned in its name, the module `IndexManager` is a class that works for the management of the index.

The index manager supports the following functions, which are listed as `public` member methods:

- create
  The `create` method supports two types of index creation:
  1. Create an empty index on a column of table, for example when called at `create table`
  2. Create an index on existing values
- drop
  Remove an index from index manager
- insert
  Insert a single element to a certain index
- search
  Search for a single element in a certain index, the return value is position of the target row in table (in `int`)
- remove
  Remove a single element from a certain index
- save
  Save all the indexes to local index file, should be called when quitting the process
- load
  Load the indexes from local file to memory, should be called when the process starts

Each of the index is individually managed by a customize B+ tree, and we use an unordered_map (the only private member in the class) to relate the index name with its content.

In order to save the space in B+ tree storage, the keys stored in B+ tree is actually the row number. And we use an external function provided by `RecordManager` to get the real value for comparing when operating the B+ tree.

The B+ tree currently has three main functions: insert, delete and search. Some other values and method are marked public, but they are _unused_ and _left for future accomplishment_.

The details of implementation will be illustrate in following chapters.

## Interface

The module `IndexManager` contains two parts: the manager layer and the B+ tree layer.

The manager layer's interfaces are:

- constructor: nothing else other than default constructor is needed

  ```cpp
  IndexManager() = default;
  ```

- destructor: nothing extra needs to be destroyed

  ```cpp
  ~IndexManager();
  ```

- create index: there are two types:

  1.  build an empty index by name

      ```cpp
      void create(const string& treename);
      ```

  2.  build an index on existing elements

      ```cpp
      void create(const string& treename,
        const vector<std::pair<uint32_t, sql_value>>& indexs,
        uint32_t idx_pos,
        const macro::table table);
      ```

      Explanation:

      - the vector `indexs` stores those exising value
      - `idx_pos` marks which column is the index in the table (the same below)

- drop index: just drop by name

  ```cpp
  void drop(const string& treename);
  ```

- search: search the position of value `val`

  ```cpp
  search(const string& treename,
    const sql_value& val,
    uint32_t idx_pos,
    const macro::table& table) const;
  ```

- insert: insert a value `val` at `new_key`

  ```cpp
  void insert(const string& treename,
    const sql_value& val,
    uint32_t idx_pos,
    const macro::table& table,
    ELEMENTTYPE new_key);
  ```

- remove: delete a value `val`

  ```cpp
  void remove(const string& treename,
    const sql_value& val,
    uint32_t idx_pos,
    const macro::table& table,
    std::unordered_map<uint32_t, sql_tuple> umap);
  ```

- save & load: file operation

  ```cpp
  void save();
  void load();
  ```

- `searchNext` and `searchHead` are not currently used, so we won't give the definition here

The B+ tree layer's interfaces are:

- constructor:

  1. The default constructor

     ```cpp
     BPTree() = default;
     ```

  2. Init an empty tree by max size and degree

     ```cpp
     BPTree(uint32_t max_size, uint32_t deg);
     ```

  3. Init a tree from a string (should be read from file)

  ```cpp
  BPTree(string treestr, uint32_t max_size, string& treename);
  ```

- destructor: we just need to delete the pool we apply

  ```cpp
  ~BPTree();
  ```

- insert

  ```cpp
  void insert(uint32_t idx_pos,
            const macro::table& table,
            ELEMENTTYPE rec_id,
            const sql_value& target);
  ```

- search

  ```cpp
  ELEMENTTYPE search(uint32_t idx_pos,
                     const macro::table& table,
                     const sql_value& target) const;
  ```

- remove

  ```
    void remove(uint32_t idx_pos,
              const macro::table& table,
              const sql_value& target,
              std::unordered_map<uint32_t, sql_tuple> umap);
  ```

- write a tree named `treename` to file named `filename`

  ```cpp
  void write_file(string filename, string treename);
  ```

## Implementation Detail

For index manager, it is almost a wrapper for B+ tree

We just use a map to relate the B+ tree with the index name

And when we do operations on index manager, we actually call the api provided by B+ trees

## B+ tree details

### B+ tree nodes

For each node in B+ trees, we use a class `BPTreeNode` to represent it

```cpp
class BPTreeNode {
 public:
  BPTreeNode *fa, *nxt;
  vector<BPTreeNode*> ch;
  vector<ELEMENTTYPE> key;
  uint32_t binary_search(uint32_t idx_pos,
                         const macro::table& table,
                         const sql_value& target) const;
  uint32_t binary_search(
      uint32_t idx_pos,
      const macro::table& table,
      const sql_value& target,
      std::unordered_map<uint32_t, sql_tuple> umap) const;
};
```

The members are:

- `fa`: the father of the node
- `ch`: the children of the node
- `nxt`: the next node in the bottom layer, should remain `nullptr` for upper ones
- `key`: the marks of the node

For those leaf nodes, `ch` are always `nullptr`, and the number is the same as `key`.
For those non-leaf nodes, the `ch` of number should be one more than `key`.

The `binary_search` method is use to find a target's position, for leaf node is the key, for non-leaf node is the children

#### B+ tree searching

Searching from the root, and traverse the node, until find a bigger element, then go into this child

Find until leaf, run `binary_search` and return the `key` found

#### B+ tree insertion

First find the position to insert

Then there are two types of insertion:

1. If the node is not full, then we can directly insert into the leaf node
2. If the node is full, we need to split. Do the following procedure recursively:

- Split at the middle of the current node
- Apply a new node as sibling
- Move the second half to the sibling
- If a new `fa` node is needed, apply a new node
- Rearrange the pointers' relationship

#### B+ tree deletion

First find the position to delete

Then there are steps to adjust the tree:

1. We need to replace the internal marks with the node's successor

2. If the node's size is more than a half or it is root then we can delete directly.
   On other cases, we need toIf the node's size is less than a half, we should try to rotate with a neighbour, either the previous one or the next one

3. If no neighbour is able to rotate here, we need to merge the node with the neighbour

4. If the only key is deleted from the root, we need to decrease the height



All the above operations' detail is illustrated in the comment among the codes.

Since it is too long and takes too much unnecessary space, here I just give the basic idea instead of implement details.
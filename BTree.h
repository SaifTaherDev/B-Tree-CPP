#pragma once

#include "BTreeNode.h"

class BTree
{
private:
	int t;
	int total_keys;
	int total_splits;
	BTreeNode* root;

	void split_child(BTreeNode* p, int i);
	BTreeNode* split_root();
	void insert_not_full(BTreeNode* u, int key);
	void delete_key(BTreeNode* u, int key);
	void merge_children(BTreeNode* p, int i);
	int sibling_with_at_least_t_keys(BTreeNode* p, int i);
	void restore(BTreeNode* p, const int n_children, const int children_level, stringstream* streams, const int& n_lines);

public:
	BTree(int _t);
	BTree(int _t, int* vals, int n);
	BTree(int _t, string snapshot_filename); // SAVE/RESTORE constructor

	BTreeNode* search(int key, BTreeNode* u = nullptr) const;

	void insert(int key);

	void level_order(ostream& output, BTreeNode** u = nullptr, int n = 1, int level = 1) const;

	bool delete_key_wrap(int key);

	int max_key(BTreeNode* u = nullptr) const;
	int min_key(BTreeNode* u = nullptr) const;

	int get_total_keys() const;
	int get_total_splits() const;
	void save(ostream& output, BTreeNode** u = nullptr, int n_level = 1) const;
};


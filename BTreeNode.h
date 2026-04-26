#pragma once

#include <stdexcept>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

class BTreeNode
{
private:
	int* keys;
	BTreeNode** children;
	int n;
	int t;
	bool leaf;

	friend class BTree;
public:
	BTreeNode(int t, bool leaf = true);
	void fill_keys(int* keys, int n);
};


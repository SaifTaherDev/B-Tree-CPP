#pragma once
class BTreeNode
{
public:
	int* keys;
	int t;
	BTreeNode** children;
	int n;
	bool leaf;
	BTreeNode(int _t, bool _leaf);
	void insertNonFull(int k);
	void splitChild(int i, BTreeNode* y);
};


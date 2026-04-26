#include "BTreeNode.h"

BTreeNode::BTreeNode(int _t, bool _leaf) : leaf(_leaf), t(_t)
{
	n = 0;

	keys = new int[2 * _t - 1];
	children = new BTreeNode* [2 * _t];
}

void BTreeNode::fill_keys(int* _keys, int n)
{
	for (int i = 0; i < min(n, 2*t - 1); i++)
	{
		keys[i] = _keys[i];
	}
}
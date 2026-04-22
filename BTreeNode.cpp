#include "BTreeNode.h"

BTreeNode::BTreeNode(int _t, bool _leaf) : t(_t), leaf(_leaf)
{
	n = 0;
	keys = new int[2 * _t - 1];
	children = new BTreeNode * [2 * _t];
}

void BTreeNode::insertNonFull(int k)
{

}

void BTreeNode::splitChild(int i, BTreeNode* y)
{

}

#include "BTree.h"

BTree::BTree(int _t) : t(_t)
{
	root = new BTreeNode(_t);
	total_keys = 0;
	total_splits = 0;
}

BTree::BTree(int _t, int* vals, int n) : t(_t)
{
	root = new BTreeNode(_t);
	total_keys = 0;
	total_splits = 0;

	for (int i = 0; i < n; i++)
		insert(*(vals + i));
}

BTree::BTree(int _t, string snapshot_filename) : t(_t) // RESTORE constructor
{
	total_keys = 0;
	total_splits = 0;

	ifstream snapshot(snapshot_filename);
	string curr_line;

	int n_lines = 0;
	while (getline(snapshot, curr_line))
		n_lines++;
	string* lines = new string[n_lines];

	snapshot = ifstream(snapshot_filename);
	n_lines = 0;
	while (getline(snapshot, curr_line))
		lines[n_lines++] = curr_line;

	stringstream* streams = new stringstream[n_lines];
	for (int i = 0; i < n_lines; i++)
	{
		streams[i] = stringstream(lines[i]);
	}

	int root_n_keys; streams[0] >> root_n_keys;
	int* root_keys = new int[root_n_keys];
	for (int i = 0; i < root_n_keys; i++)
		streams[0] >> root_keys[i];

	root = new BTreeNode(_t, false);
	root->fill_keys(root_keys, root_n_keys);
	root->n = root_n_keys;

	if (n_lines > 1)
		restore(root, root_n_keys + 1, 1, streams, n_lines);
}

void BTree::restore(BTreeNode* p, const int n_children, const int children_level, stringstream* streams, const int &n_lines)
{
	int n_keys;
	int* keys;
	BTreeNode* new_node;

	for (int i = 0; i < n_children; i++)
	{
		streams[children_level] >> n_keys;
		keys = new int[n_keys];

		for (int j = 0; j < n_keys; j++)
			streams[children_level] >> keys[j];

		new_node = new BTreeNode(t, children_level == (n_lines - 1));
		new_node->fill_keys(keys, n_keys);
		new_node->n = n_keys;
		p->children[i] = new_node;

		if (children_level != n_lines - 1)
			restore(p->children[i], n_keys + 1, children_level + 1, streams, n_lines);
	}
}

BTreeNode* BTree::search(int key, BTreeNode* u) const
{
	if (u == nullptr)
		u = root;

	int i = 0;

	while (i < u->n && u->keys[i] < key) i++;

	if (i < u->n && u->keys[i] == key) return u;

	if (u->leaf) return nullptr;

	return search(key, u->children[i]);
}

void BTree::split_child(BTreeNode* p, int i)
{
	BTreeNode* full_node = p->children[i];
	BTreeNode* new_node = new BTreeNode(t, full_node->leaf);

	for (int j = t; j < 2 * t - 1; j++)
		new_node->keys[j - t] = full_node->keys[j];

	if (!full_node->leaf)
		for (int j = t; j < 2 * t; j++)
			new_node->children[j - t] = full_node->children[j];

	new_node->n = t - 1;

	for (int j = p->n - 1; j >= i; j--)
		p->keys[j + 1] = p->keys[j];

	p->keys[i] = full_node->keys[t - 1];
	p->n++;

	for (int j = p->n; j >= i + 1; j--)
		p->children[j + 1] = p->children[j];

	p->children[i + 1] = new_node;

	full_node->n = t - 1;

	total_splits++;
}

BTreeNode* BTree::split_root()
{
	BTreeNode* new_root = new BTreeNode(t, false);
	new_root->children[0] = root;
	root = new_root;
	split_child(new_root, 0);

	return new_root;
}

void BTree::insert_not_full(BTreeNode* u, int key)
{
	int i = 0; 
	while (i < u->n and key > u->keys[i]) i++;

	if (u->leaf)
	{
		for (int j = u->n - 1; j >= i; j--)
			u->keys[j + 1] = u->keys[j];
		u->keys[i] = key;
		u->n++;
	}
	else
	{
		if (u->children[i]->n == 2*t - 1)
		{
			split_child(u, i);
			if (key > u->keys[i]) i++;
		}

		insert_not_full(u->children[i], key);
	}
}

void BTree::insert(int key)
{
	if (root->n == 2 * t - 1)
		split_root();
		
	insert_not_full(root, key);
	total_keys++;
}

void BTree::level_order(ostream& output, BTreeNode** u, int n_level, int level) const
{
	if (u == nullptr)
	{
		u = new BTreeNode*[1];
		u[0] = root;
	}

	bool is_last = u[0]->leaf;
	BTreeNode** next_level = nullptr;
	int curr_index;

	if (!is_last)
	{
		next_level = new BTreeNode * [n_level * (2 * t - 1)];
		curr_index = 0;
	}

	output << "Level " << level << ": ";

	for (int i = 0; i < n_level; i++)
	{
		if (u[i]->n == 0) continue;
		output << "[ ";

		for (int j = 0; j < u[i]->n; j++)
		{
			output << u[i]->keys[j] << " ";
			if (!is_last && u[i]->children[j]->n != 0)
				next_level[curr_index++] = u[i]->children[j];
		}
		if (!is_last && u[i]->children[u[i]->n]->n != 0)
			next_level[curr_index++] = u[i]->children[u[i]->n];

		output << "]";

		if (i != n_level - 1) output << ", ";
		else output << '\n';
	}

	if (!is_last) level_order(output, next_level, curr_index, level + 1);
}

void BTree::delete_key(BTreeNode* u, int key)
{
	if (u->n < t && u != root) throw runtime_error("The node must contain at least t keys or be itself the root.");

	// We locate the key 
	int i = 0;
	while (i < u->n && key > u->keys[i]) i++;

	// Now we handle the 3 cases

	if (u->leaf)
	{
		// Case 1: 
		if (i < u->n && key == u->keys[i])
		{
			for (int j = i; j < u->n - 1; j++)
				u->keys[j] = u->keys[j + 1];

			u->n--;
			return;
		}
		else
		{
			throw runtime_error("The key " + to_string(key) + " was not found in the tree.");
		}
	}

	// If u is not a leaf	

	if (i < u->n && key == u->keys[i])
	{
		// Case 2
		if (u->children[i]->n >= t)
		{
			// Case 2a
			int pred_key = max_key(u->children[i]);
			delete_key(u->children[i], pred_key);
			u->keys[i] = pred_key;
		}
		else if (u->children[i + 1]->n >= t)
		{
			// Case 2b
			int succ_key = min_key(u->children[i + 1]);
			delete_key(u->children[i + 1], succ_key);
			u->keys[i] = succ_key;
		}
		else
		{
			// Case 2c: both children have t-1 keys or fewer
			merge_children(u, i);
			if (u == root && u->n == 0)
			{
				root = u->children[0];
			}

			delete_key(u->children[i], key);
		}
	}
	else
	{
		// Case 3 (key not in u)
		if (u->children[i]->n >= t)
			delete_key(u->children[i], key); // Recursive
		else if (sibling_with_at_least_t_keys(u, i) != -1)
		{
			// Case 3a
			int sibling_at_least_t = sibling_with_at_least_t_keys(u, i);
			if (sibling_at_least_t == i + 1)
			{
				// Right sibling has at least t keys
				u->children[i]->keys[u->children[i]->n++] = u->keys[i]; // 1. We move the parent key down to the child

				u->keys[i] = u->children[sibling_at_least_t]->keys[0]; // 2. We move the sibling's first key up to the parent

				for (int j = 1; j < u->children[sibling_at_least_t]->n; j++)
					u->children[sibling_at_least_t]->keys[j - 1] = u->children[sibling_at_least_t]->keys[j]; // Shift the keys of the sibling one place to the left

				if (!u->children[sibling_at_least_t]->leaf)
				{
					u->children[i]->children[u->children[i]->n] = u->children[sibling_at_least_t]->children[0]; // 3. Move the leftmost child of the sibling to node i in order to maintain tree structure


					for (int j = 1; j <= u->children[sibling_at_least_t]->n; j++)
						u->children[sibling_at_least_t]->children[j - 1] = u->children[sibling_at_least_t]->children[j]; // Shift the children of the sibling one place to the left
				}

				u->children[sibling_at_least_t]->n--;
			}
			else
			{
				// Left sibling has at least t keys
				for (int j = u->children[i]->n - 1; j >= 0; j--)
					u->children[i]->keys[j + 1] = u->children[i]->keys[j];
				u->children[i]->keys[0] = u->keys[sibling_at_least_t];

				u->keys[sibling_at_least_t] = u->children[sibling_at_least_t]->keys[u->children[sibling_at_least_t]->n - 1];

				if (!u->children[sibling_at_least_t]->leaf)
				{
					for (int j = u->children[i]->n; j >= 0; j--)
						u->children[i]->children[j + 1] = u->children[i]->children[j];

					u->children[i]->children[0] = u->children[sibling_at_least_t]->children[u->children[sibling_at_least_t]->n]; // 3. Move the rightmost child of the sibling to node i in order to maintain tree structure
				}

				u->children[i]->n++;
				u->children[sibling_at_least_t]->n--;
			}
			delete_key(u->children[i], key); // Recursive
		}
		else
		{
			// u is not a leaf and both siblings have t-1 keys
			if (i > 0)
			{
				// we merge with left sibling
				merge_children(u, i - 1);
				i -= 1; // we now have one less child, so we shift over

				level_order(cout);
				cout << "\n\n";
			}	
			else
			{
				// we merge with right sibling
				merge_children(u, i);
			}

			if (u == root && u->n == 0)
			{
				// reduce height of the tree
				root = u->children[0];
			}
			
			delete_key(u->children[i], key);
		}
	}
}

bool BTree::delete_key_wrap(int key)
{
	try
	{
		delete_key(root, key);
		total_keys--;

		return true;
	}
	catch (exception e)
	{
		return false;
	}
	
}

int BTree::max_key(BTreeNode* u) const
{
	if (u == nullptr) u = root;

	while (!u->leaf)
	{
		u = u->children[u->n];
	}
	
	return u->keys[u->n - 1];
}
int BTree::min_key(BTreeNode* u) const
{
	if (u == nullptr) u = root;

	while (!u->leaf)
	{
		u = u->children[0];
	}

	return u->keys[0];
}
void BTree::merge_children(BTreeNode* p, int i)
{
	int median_key = p->keys[i];
	p->children[i]->keys[p->children[i]->n++] = median_key;

	int new_children_first_index = p->children[i]->n;
	
	for (int j = 0; j < p->children[i + 1]->n; j++)
	{
		p->children[i]->keys[p->children[i]->n++] = p->children[i + 1]->keys[j];
	}

	if (!p->children[i]->leaf)
	{
		for (int j = 0; j <= p->children[i + 1]->n; j++)
		{
			p->children[i]->children[new_children_first_index + j] = p->children[i + 1]->children[j];
		}
	}

	// Shift parent's keys to fill the gap left by the median key
	for (int j = i + 1; j < p->n; j++) {
		p->keys[j - 1] = p->keys[j];
	}

	// Shift parent's children to fill the gap left by the deleted right child
	for (int j = i + 2; j <= p->n; j++) {
		p->children[j - 1] = p->children[j];
	}

	p->n--; // Update the parent's key count
	

	if (p == root && p->n == 0)
	{
		root = p->children[i];
	}
}

int BTree::sibling_with_at_least_t_keys(BTreeNode* p, int i)
{
	if (i > 0 && p->children[i - 1]->n >= t)
		return i - 1;
	if (i < p->n && p->children[i + 1]->n >= t)
		return i + 1;

	return -1;
}

int BTree::get_total_keys() const
{
	return total_keys;
}

int BTree::get_total_splits() const
{
	return total_splits;
}

void BTree::save(ostream& output, BTreeNode** u, int n_level) const
{
	if (u == nullptr)
	{
		u = new BTreeNode * [1];
		u[0] = root;
	}

	bool is_last = u[0]->leaf;
	BTreeNode** next_level = nullptr;
	int curr_index;

	if (!is_last)
	{
		next_level = new BTreeNode * [n_level * (2 * t - 1)];
		curr_index = 0;
	}

	for (int i = 0; i < n_level; i++)
	{
		output << u[i]->n << " ";
		for (int j = 0; j < u[i]->n; j++)
		{
			output << u[i]->keys[j] << " ";
			if (!is_last && u[i]->children[j]->n != 0)
				next_level[curr_index++] = u[i]->children[j];
		}
		if (!is_last && u[i]->children[u[i]->n]->n != 0)
			next_level[curr_index++] = u[i]->children[u[i]->n];

		if (i == n_level - 1) output << '\n';
	}

	if (!is_last) save(output, next_level, curr_index);
}
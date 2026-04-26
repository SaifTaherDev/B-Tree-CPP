#include "BTree.h"

void read_input(BTree& my_tree, string input_name, string output_name, string log_name)
{
	ifstream input(input_name);
	ofstream output(output_name), log(log_name);

	BTreeNode* search_result;
	bool deletion_succ;

	char op;
	int n;

	while (input >> op)
	{
		input >> n;

		switch (op)
		{
		case 'I':
			my_tree.insert(n);
			log << "Inserting " << n << ":\n";
			my_tree.level_order(log);
			log << "\n" << "total # of splits so far: " << my_tree.get_total_splits() << "\n\n";
			break;
		case 'S':
			search_result = my_tree.search(n);
			log << "Searching for " << n << ": ";
			if (search_result == nullptr) log << "not found\n\n";
			else log << "found!\n\n";
			break;
		case 'D':
			log << "Deleting " << n << ":\n";
			deletion_succ = my_tree.delete_key_wrap(n);
			my_tree.level_order(log);
			log << "\n";
			if (deletion_succ) log << n << " was deleted successfully.\n\n";
			else log << n << " was not found in the tree.\n\n";
		}
	}

	my_tree.level_order(output);
}

int main()
{
	BTree my_tree(2, "snapshot.dat");
	my_tree.level_order(cout);
	ofstream output("snapshot2.dat");
	my_tree.save(output);
	//read_input(my_tree, "input.txt", "output.txt", "log.txt");

	return 0;
}
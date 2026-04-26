## NOTE: The logic for this implementation of B-Trees followed the CLRS design paradigm.

# Answers to the conceptual questions in the assignment:

## Part 7: Edge Cases

*Case 1*: 10 10 10 10

Questions:

• **What happens?**

  – My implementation allows the insertion of duplicates.

• **Is the tree still balanced?**

  – Yes.

*Case 2*: 10 20 30 40

**Question: What happens when inserting into a full root?**

  – The root is split using the `BTree::split_root()` function.
___

## Part 8: Reverse Reconstruction
  – The height, structure, and number of splits completely changes if the order of insertions was reversed; the exact changes depend entirely on whether the data was sorted or not. If the data was sorted, then the height and number of splits will remain identical when the order of insertion is reversed, and the structure of the tree will be mirrored. If it was not sorted but rather arbitrary, the height, structure, and number of splits will change unpredictably.
___
## Part 10: Concept Questions

**1. Why is a 2-3-4 Tree a special case of B-Tree?**

  – Because it is simply a B-tree whose `t` value is equal to 2; i.e., it is a B-tree that allows a maximum of 3 keys and 4 children per node.
  
**2. Maximum number of children?**

  – The maximum number of children for any given B-tree is equal to `2 * t`.
  
**3. Explain split operation**

  – The split operation occurs when the number of keys exceeds `2 * t - 1` in a given node. The node is split into 3 nodes: the median keys moves into the parent node (if there was no parent node, a new parent is generated), the keys to the left of the median key move down to the new left child, and the keys to the right of the median key move down to the new right child.

**4. Complexity of insertion?**

  – Insertion is Big-O of log(n) for B-trees.
___
## README Questions (SAVE/RESTORE part):

1. **Why is saving only keys not enough?**

  – Because it would not be clear where nodes start and end at each level; therefore, we would need to perform many comparisons between the parent keys and the child keys to figure out where children nodes end, which would be very inefficient.

2. **How do you distinguish node types in serialization?**

– I simply add the number of keys in each node at the beginning, and from that I can figure out the number of children for each node (it's simply the number of keys + 1). I then generate the snapshot of the tree in a depth-first manner. When I am restoring the tree from the snapshot, I do it in a breadth-first manner, similar to how a level-order traversal is performed.
  

## Project Phase 3

## Functionalities:

### Part a:  B+ Tree Insertion

- B+ Tree insertion required to complete implementation of 2 functions : insertion in leaf node and internal node
- Recursive function for insertion is used
- Insertion in Leaf Node:
    - The keys and record pointers are inserted in sorted order. Map data structure is used to store <key, record pointer>
    - A leaf node can have maximum FANOUT number of <key, record pointer> pairs. 
    - In case of overflow, a new Leaf Node is created, ceil(FANOUT / 2 ) <key, record pointer> pairs is kept in the current leaf node and the rest are inserted in the newly created leaf node.
    - The tree pointer of the newly created node is returned in case of split, else null tree pointer is returned 
- Insertion in Internal Node
    - Recursive approach is used
    - In general, first the <key, record pointer> is inserted into the leaf node, maximum key from the corresponding sub-tree is inserted in the internal node if required
    - If key needs to be inserted, 2 cases can occur: 
    - case 1: There is space available in the internal node for a new key
        - The max key of corresponding subtree and the tree pointer is inserted in the correct position (lower_bound function is used to find index), the size is updated and null tree pointer is returned as no new internal node is created
    - case 2: Space is not available in internal node
        - The key and tree pointer are inserted in correct positions
        - A new internal node is created
        - First ceil(FANOUT / 2) keys and tree pointers are kept in current internal node and remaining are inserted in the split internal node
        - Address of the split internal node is returned
         
### Part b:  B+ Tree Deletion

### Part c: Analysis 

## Contribution

### Madhusree Bera
- B+ Tree inserion in 
    - leaf node
    - internal node
- report


### Sanya Gandhi
- B+ Tree deletion in
    - leaf node
    - internal node
- report

### Anurag Gupta
- RANGE command analysis
- report
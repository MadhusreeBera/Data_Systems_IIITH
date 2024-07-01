#include "InternalNode.hpp"

// creates internal node pointed to by tree_ptr or creates a new one
InternalNode::InternalNode(const TreePtr &tree_ptr) : TreeNode(INTERNAL, tree_ptr)
{
    this->keys.clear();
    this->tree_pointers.clear();
    if (!is_null(tree_ptr))
        this->load();
}

// max element from tree rooted at this node
Key InternalNode::max()
{
    Key max_key = DELETE_MARKER;
    TreeNode *last_tree_node = TreeNode::tree_node_factory(this->tree_pointers[this->size - 1]);
    max_key = last_tree_node->max();
    delete last_tree_node;
    return max_key;
}

// if internal node contains a single child, it is returned
TreePtr InternalNode::single_child_ptr()
{
    if (this->size == 1)
        return this->tree_pointers[0];
    return NULL_PTR;
}

// inserts <key, record_ptr> into subtree rooted at this node.
// returns pointer to split node if exists
// TODO: InternalNode::insert_key to be implemented
TreePtr InternalNode::insert_key(const Key &key, const RecordPtr &record_ptr)
{
    TreePtr new_tree_ptr = NULL_PTR;
    // search for the node
    int search_index = lower_bound(keys.begin(), keys.end(), key) - keys.begin();
    TreeNode *left_node = tree_node_factory(tree_pointers[search_index]);
    TreePtr right_node = left_node->insert_key(key, record_ptr);
    
    //if split node is there
    if (!is_null(right_node))
    {
        // key to be inserted in internal node
        Key internal_key = left_node->max();

        // check if space is available
        if (keys.size() < FANOUT - 1)
        {
            // find the index to insert
            int inserting_index = lower_bound(keys.begin(), keys.end(), internal_key) - keys.begin();

            // inserting in keys
            keys.insert(keys.begin() + inserting_index, internal_key);

            // inserting the TreePtr
            tree_pointers.insert(tree_pointers.begin() + inserting_index + 1, right_node);
            size++;
        }
        else
        {
            // cout << "Internal node split" << endl;

            // find the index to insert
            int inserting_index = lower_bound(keys.begin(), keys.end(), internal_key) - keys.begin();

            // inserting in keys
            keys.insert(keys.begin() + inserting_index, internal_key);

            // inserting the TreePtr
            tree_pointers.insert(tree_pointers.begin() + inserting_index + 1, right_node);

            // now the keys are overflowing, so we need to insert the extra in a new internal node
            InternalNode *new_internal_node = new InternalNode();
            new_internal_node->keys.resize(keys.size() - MIN_OCCUPANCY);

            // keeping the first ceil(f/2) keys in first internal node
            int j = 0;
            for (int i = MIN_OCCUPANCY; i < keys.size(); i++)
            {
                new_internal_node->keys[j++] = keys[i];
            }

            keys.resize(MIN_OCCUPANCY);
            // keeping the first ceil(f/2) pointers in the first internal node
            new_internal_node->tree_pointers.resize(tree_pointers.size() - MIN_OCCUPANCY);

            j = 0;
            for (int i = MIN_OCCUPANCY; i < tree_pointers.size(); i++)
            {
                new_internal_node->tree_pointers[j++] = tree_pointers[i];
            }
            tree_pointers.resize(MIN_OCCUPANCY);

            new_tree_ptr = new_internal_node->tree_ptr;

            size = tree_pointers.size();
            new_internal_node->size = new_internal_node->tree_pointers.size();
            new_internal_node->dump();
        }
    }
    this->dump();
    return new_tree_ptr;
}

// deletes key from subtree rooted at this if exists
// TODO: InternalNode::delete_key to be implemented
void InternalNode::delete_key(const Key &key)
{
    TreePtr new_tree_ptr = NULL_PTR;
    cout << "InternalNode::delete_key not implemented" << endl;
    this->dump();
}

// runs range query on subtree rooted at this node
void InternalNode::range(ostream &os, const Key &min_key, const Key &max_key) const
{
    BLOCK_ACCESSES++;
    for (int i = 0; i < this->size - 1; i++)
    {
        if (min_key <= this->keys[i])
        {
            auto *child_node = TreeNode::tree_node_factory(this->tree_pointers[i]);
            child_node->range(os, min_key, max_key);
            delete child_node;
            return;
        }
    }
    auto *child_node = TreeNode::tree_node_factory(this->tree_pointers[this->size - 1]);
    child_node->range(os, min_key, max_key);
    delete child_node;
}

// exports node - used for grading
void InternalNode::export_node(ostream &os)
{
    TreeNode::export_node(os);
    for (int i = 0; i < this->size - 1; i++)
        os << this->keys[i] << " ";
    os << endl;
    for (int i = 0; i < this->size; i++)
    {
        auto child_node = TreeNode::tree_node_factory(this->tree_pointers[i]);
        child_node->export_node(os);
        delete child_node;
    }
}

// writes subtree rooted at this node as a mermaid chart
void InternalNode::chart(ostream &os)
{
    string chart_node = this->tree_ptr + "[" + this->tree_ptr + BREAK;
    chart_node += "size: " + to_string(this->size) + BREAK;
    chart_node += "]";
    os << chart_node << endl;

    for (int i = 0; i < this->size; i++)
    {
        auto tree_node = TreeNode::tree_node_factory(this->tree_pointers[i]);
        tree_node->chart(os);
        delete tree_node;
        string link = this->tree_ptr + "-->|";

        if (i == 0)
            link += "x <= " + to_string(this->keys[i]);
        else if (i == this->size - 1)
        {
            link += to_string(this->keys[i - 1]) + " < x";
        }
        else
        {
            link += to_string(this->keys[i - 1]) + " < x <= " + to_string(this->keys[i]);
        }
        link += "|" + this->tree_pointers[i];
        os << link << endl;
    }
}

ostream &InternalNode::write(ostream &os) const
{
    TreeNode::write(os);
    for (int i = 0; i < this->size - 1; i++)
    {
        if (&os == &cout)
            os << "\nP" << i + 1 << ": ";
        os << this->tree_pointers[i] << " ";
        if (&os == &cout)
            os << "\nK" << i + 1 << ": ";
        os << this->keys[i] << " ";
    }
    if (&os == &cout)
        os << "\nP" << this->size << ": ";
    os << this->tree_pointers[this->size - 1];
    return os;
}

istream &InternalNode::read(istream &is)
{
    TreeNode::read(is);
    this->keys.assign(this->size - 1, DELETE_MARKER);
    this->tree_pointers.assign(this->size, NULL_PTR);
    for (int i = 0; i < this->size - 1; i++)
    {
        if (&is == &cin)
            cout << "P" << i + 1 << ": ";
        is >> this->tree_pointers[i];
        if (&is == &cin)
            cout << "K" << i + 1 << ": ";
        is >> this->keys[i];
    }
    if (&is == &cin)
        cout << "P" << this->size;
    is >> this->tree_pointers[this->size - 1];
    return is;
}

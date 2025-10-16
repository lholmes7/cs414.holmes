// Landon Holmes
// Assignment05.cpp

#include <variant>
#include <memory>
#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

struct Node;

using Tree = variant<monostate, shared_ptr<Node>>; //empty or part of a tree (https://devblogs.microsoft.com/oldnewthing/20240708-00/?p=109959)

struct Node {
    int value;
    Tree left;
    Tree right;
    explicit Node(int v) : value(v), left(monostate{}), right(monostate{}) {}
};

inline bool tree_is_empty(const Tree& t) {
    return holds_alternative<monostate>(t);
}

// Return shared_ptr<Node> 
inline shared_ptr<Node> get_node_ptr(const Tree& t) {
    return get<shared_ptr<Node>>(t);
}

// mutation
void insert(Tree& t, int v) {
    if (tree_is_empty(t)) {
        t = make_shared<Node>(v);
        return;
    }
    auto n = get_node_ptr(t);
    if (v < n->value) {
        insert(n->left, v);
    }
    else if (v > n->value) {
        insert(n->right, v);
    }
    else {
    }
}

// Inorder
void inorder_rec(const Tree& t, vector<int>& out) {
    if (tree_is_empty(t)) return;
    auto n = get_node_ptr(t);
    inorder_rec(n->left, out);
    out.push_back(n->value);
    inorder_rec(n->right, out);
}
vector<int> inorder(const Tree& t) {
    vector<int> out;
    inorder_rec(t, out);
    return out;
}

// Preorder
void preorder_rec(const Tree& t, vector<int>& out) {
    if (tree_is_empty(t)) return;
    auto n = get_node_ptr(t);
    out.push_back(n->value);
    preorder_rec(n->left, out);
    preorder_rec(n->right, out);
}
vector<int> preorder(const Tree& t) {
    vector<int> out;
    preorder_rec(t, out);
    return out;
}

// Postorder
void postorder_rec(const Tree& t, vector<int>& out) {
    if (tree_is_empty(t)) return;
    auto n = get_node_ptr(t);
    postorder_rec(n->left, out);
    postorder_rec(n->right, out);
    out.push_back(n->value);
}
vector<int> postorder(const Tree& t) {
    vector<int> out;
    postorder_rec(t, out);
    return out;
}

// print utility
void print_vec(const vector<int>& v) {
    for (size_t i = 0; i < v.size(); ++i) {
        if (i) cout << ", ";
        cout << v[i];
    }
    cout << "\n";
}

// test main
int main() {
    Tree t = monostate{};

    int vals[] = { 10, 5, 15, 3, 7, 12, 20 };
    for (int v : vals) insert(t, v);

    cout << "Inorder (unsorted): ";
    print_vec(inorder(t));

    cout << "Preorder: ";
    print_vec(preorder(t));

    cout << "Postorder: ";
    print_vec(postorder(t));

    return 0;
}

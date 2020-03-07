#include <algorithm>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

struct AVLTreeNode {
    AVLTreeNode* left, * right;
    int value, height, size;
    AVLTreeNode(int val) : left(NULL), right(NULL), value(val), height(1), size(1) {}
};

class AVLTree {
    AVLTreeNode* root;

    int getSubtreeSize(AVLTreeNode*& node) {
        return (node == NULL) ? 0 : node->size;
    }

    int getNodeHeight(AVLTreeNode*& node) {
        return (node == NULL) ? 0 : node->height;
    }

    AVLTreeNode* leftRotation(AVLTreeNode*& p) { // returns highest node after rotation
        AVLTreeNode* res = p->right;

        p->right = p->right->left;
        res->left = p;

        res->left->height = max(getNodeHeight(res->left->left), getNodeHeight(res->left->right)) + 1;
        res->height = max(getNodeHeight(res->left), getNodeHeight(res->right)) + 1;

        res->left->size = getSubtreeSize(res->left->left) + getSubtreeSize(res->left->right) + 1;
        res->size = getSubtreeSize(res->left) + getSubtreeSize(res->right) + 1;

        return res;
    }

    AVLTreeNode* rightRotation(AVLTreeNode*& p) {
        AVLTreeNode* res = p->left;

        p->left = p->left->right;
        res->right = p;

        res->right->height = max(getNodeHeight(res->right->left), getNodeHeight(res->right->right)) + 1;
        res->height = max(getNodeHeight(res->left), getNodeHeight(res->right)) + 1;

        res->right->size = getSubtreeSize(res->right->left) + getSubtreeSize(res->right->right) + 1;
        res->size = getSubtreeSize(res->left) + getSubtreeSize(res->right) + 1;

        return res;
    }

    AVLTreeNode* getMinValueNode(AVLTreeNode*& n) {
        AVLTreeNode* c = n;
        while (c && c->left != NULL) {
            c = c->left;
        }
        return c;
    }

    AVLTreeNode* insertHandler(int value, AVLTreeNode*& current) {
        if (current == NULL) {
            return new AVLTreeNode(value);
        } else if (value < current->value) {
            current->left = insertHandler(value, current->left);
        } else if (value > current->value) {
            current->right = insertHandler(value, current->right);
        }

        current->height = max(getNodeHeight(current->left), getNodeHeight(current->right)) + 1;
        current->size = getSubtreeSize(current->left) + getSubtreeSize(current->right) + 1;

        if (getNodeHeight(current->left) - getNodeHeight(current->right) > 1) { // left subtree is longer
            if (value < current->left->value) {
                return rightRotation(current);
            }
            else {
                current->left = leftRotation(current->left);
                return rightRotation(current);
            }
        }
        else if (getNodeHeight(current->left) - getNodeHeight(current->right) < -1) { // right subtree is longer
            if (value > current->right->value) {
                return leftRotation(current);
            }
            else {
                current->right = rightRotation(current->right);
                return leftRotation(current);
            }
        }

        return current;
    }

    int selectHandler(int i, AVLTreeNode*& node) {
        int skipped = getSubtreeSize(node->left);
        if (skipped == i) {
            return node->value;
        } else if (skipped > i) {
            return selectHandler(i, node->left);
        } else { // skipped < i
            return selectHandler(i - skipped - 1, node->right);
        }
    }

    AVLTreeNode* removeHandler(int value, AVLTreeNode*& n) {
        if (n == NULL) {
            return n;
        }

        if (value < n->value) {
            n->left = removeHandler(value, n->left);
        } else if (value > n->value) {
            n->right = removeHandler(value, n->right);
        } else {
            if (n->left == NULL) {
                AVLTreeNode* tmp = n->right;
                delete n;
                return tmp;
            } else if (n->right == NULL) {
                AVLTreeNode* tmp = n->left;
                delete n;
                return tmp;
            }
            AVLTreeNode* m = getMinValueNode(n->right);

            n->value = m->value;

            n->right = removeHandler(m->value, n->right);
        }

        n->height = max(getNodeHeight(n->left), getNodeHeight(n->right)) + 1;
        n->size = getSubtreeSize(n->left) + getSubtreeSize(n->right) + 1;

        if (getNodeHeight(n->left) - getNodeHeight(n->right) > 1) { // left subtree is longer
            if (getNodeHeight(n->left->left) > getNodeHeight(n->left->right)) {
                return rightRotation(n);
            }
            else {
                n->left = leftRotation(n->left);
                return rightRotation(n);
            }
        }
        else if (getNodeHeight(n->left) - getNodeHeight(n->right) < -1) { // right subtree is longer
            if (getNodeHeight(n->right->right) > getNodeHeight(n->right->left)) {
                return leftRotation(n);
            }
            else {
                n->right = rightRotation(n->right);
                return leftRotation(n);
            }
        }

        return n;
    }
public:
    AVLTree() {
        root = NULL;
    }

    void insert(int value) {
        if (this->contains(value)) {
            return;
        }
        root = insertHandler(value, root);
    }

    void remove(int value) {
        root = removeHandler(value, root);
    }

    bool contains(int value) {
        AVLTreeNode* current = root;

        while (current != NULL && current->value != value) {
            current = (value < current->value) ? current->left : current->right;
        }
        return current != NULL;
    }

    int select(int i) {
        return selectHandler(i, root);
    }

    bool empty() {
        return root == NULL;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);

    int n, k;

    cin >> n >> k;

    AVLTree* t = new AVLTree();

    for (int i = 1; i <= n; i++) {
        t->insert(i);
    }

    int index = k - 1, element, size = n;
    while (size > 0) {
        element = t->select(index);
        cout << element << " ";
        t->remove(element);
        size--;
        index = (index + k - 1) % max(size, 1);
    }
}
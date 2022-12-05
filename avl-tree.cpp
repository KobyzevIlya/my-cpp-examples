#include <cstdio>
#include <algorithm>

struct Node {
    Node* parent;
    Node* left;
    Node* right;

    int value;
    int height;

    Node(int x) {
        value = x;
        height = 1;
        left = right = parent = nullptr;
    }
};

class AVLTree {
public:
    AVLTree();

    int getHeight();

    void insert(int value);

    void erase(int value);

    int* find(int value);

    int* traversal();

    int* lowerBound(int value);

    bool empty();

    Node* getRoot();

    int getSize();

    ~AVLTree();

private:
    Node* empty_node_ = nullptr;
    size_t size_ = 0;

    Node* root_ = nullptr;
    void setEmptyChildren(Node* node);

    void checkBalance(Node* node);
    void setNodeHeight(Node* node);

    Node* leftTurn(Node* left_node, Node* right_node);
    Node* smallLeftTurn(Node* left_node, Node* right_node);
    Node* bigLeftTurn(Node* left_node, Node* right_node);

    Node* rightTurn(Node* left_node, Node* right_node);
    Node* smallRightTurn(Node* left_node, Node* right_node);
    Node* bigRightTurn(Node* left_node, Node* right_node);

    void clear(Node* root);
    Node* myFind(int value);
    void symmetric(Node* node, int* i, int* array);
    Node* myErase(Node* node);
};

AVLTree::AVLTree() {
    empty_node_ = new Node(0);
    empty_node_->height = 0;
}

AVLTree::~AVLTree() {
    clear(root_);
    delete empty_node_;
}

bool AVLTree::empty() {
    return size_ == 0;
}

int AVLTree::getSize() {
    return size_;
}

Node* AVLTree::getRoot() {
    return root_;
}

void AVLTree::insert(int value) {
    if (root_ == nullptr) {
        root_ = new Node(value);
        setEmptyChildren(root_);

        size_ = 1;
    } else {
        Node* current = root_;
        while (value != current->value) {
            if (value < current->value) {
                if (current->left == empty_node_) {
                    current->left = new Node(value);
                    current->left->parent = current;
                    setEmptyChildren(current->left);

                    checkBalance(current);
                    ++size_;
                    return;
                } else {
                    current = current->left;
                }
            } else if (value > current->value) {
                if (current->right == empty_node_) {
                    current->right = new Node(value);
                    current->right->parent = current;
                    setEmptyChildren(current->right);

                    checkBalance(current);
                    ++size_;
                    return;
                } else {
                    current = current->right;
                }
            }
        }
    }
}

void AVLTree::checkBalance(Node* node) {
    if (node->right->height - node->left->height > 1) {
        node = leftTurn(node, node->right);
    } else if (node->left->height - node->right->height > 1) {
        node = rightTurn(node->left, node);
    }

    setNodeHeight(node);

    if (node->parent != nullptr) {
        checkBalance(node->parent);
    }
}

Node* AVLTree::leftTurn(Node* left_node, Node* right_node) {
    if (right_node->left->height > right_node->right->height) {
        return bigLeftTurn(left_node, right_node);
    } else {
        return smallLeftTurn(left_node, right_node);
    }
}

Node* AVLTree::smallLeftTurn(Node* left_node, Node* right_node) {
    if (left_node == root_) {
        root_ = right_node;
    }

    right_node->parent = left_node->parent;
    left_node->parent = right_node;
    left_node->right = right_node->left;
    right_node->left->parent = left_node;
    right_node->left = left_node;

    if (right_node->parent && right_node->parent->value < right_node->value) {
        right_node->parent->right = right_node;
    } else if (right_node->parent) {
        right_node->parent->left = right_node;
    }

    setNodeHeight(left_node);
    setNodeHeight(right_node);

    return right_node;
}

Node* AVLTree::bigLeftTurn(Node* left_node, Node* right_node) {
    Node* middle_node = right_node->left;
    middle_node->parent = left_node->parent;

    if (left_node == root_) {
        root_ = middle_node;
    }

    left_node->parent = middle_node;
    left_node->right = middle_node->left;

    right_node->parent = middle_node;
    right_node->left = middle_node->right;

    middle_node->left = left_node;
    middle_node->right = right_node;

    if (middle_node->parent && middle_node->parent->value < middle_node->value) {
        middle_node->parent->right = middle_node;
    } else if (middle_node->parent) {
        middle_node->parent->left = middle_node;
    }

    setNodeHeight(left_node);
    setNodeHeight(right_node);
    setNodeHeight(middle_node);

    return middle_node;
}

Node* AVLTree::rightTurn(Node* left_node, Node* right_node) {
    if (left_node->right->height > left_node->left->height) {
        return bigRightTurn(left_node, right_node);
    } else {
        return smallRightTurn(left_node, right_node);
    }
}

Node* AVLTree::smallRightTurn(Node* left_node, Node* right_node) {
    if (right_node == root_) {
        root_ = left_node;
    }

    left_node->parent = right_node->parent;
    right_node->parent = left_node;
    right_node->left = left_node->right;
    left_node->right->parent = right_node->left;
    left_node->right = right_node;

    if (left_node->parent && left_node->parent->value < left_node->value) {
        left_node->parent->right = left_node;
    } else if (left_node->parent) {
        left_node->parent->left = left_node;
    }

    setNodeHeight(left_node);
    setNodeHeight(right_node);

    return left_node;
}

Node* AVLTree::bigRightTurn(Node* left_node, Node* right_node) {
    Node* middle_node = left_node->right;
    middle_node->parent = right_node->parent;

    if (right_node == root_) {
        root_ = middle_node;
    }

    right_node->parent = middle_node;
    right_node->left = middle_node->right;

    left_node->parent = middle_node;
    left_node->right = middle_node->left;

    middle_node->left = left_node;
    middle_node->right = right_node;

    if (middle_node->parent && middle_node->parent->value < middle_node->value) {
        middle_node->parent->right = middle_node;
    } else if (middle_node->parent) {
        middle_node->parent->left = middle_node;
    }

    setNodeHeight(left_node);
    setNodeHeight(right_node);
    setNodeHeight(middle_node);

    return middle_node;
}

void AVLTree::setEmptyChildren(Node* node) {
    node->left = empty_node_;
    node->right = empty_node_;
}

void AVLTree::setNodeHeight(Node* node) {
    node->height = std::max(node->left->height, node->right->height) + 1;
}

void AVLTree::clear(Node* root) {
    if (root != nullptr && root != empty_node_) {
        clear(root->left);
        clear(root->right);
        delete root;
    }
}

int AVLTree::getHeight() {
    if (root_ == nullptr) {
        return 0;
    }
    return root_->height;
}

void AVLTree::erase(int value) {
    Node* to_delete = myFind(value);
    if (to_delete == nullptr) {
        return;
    }

    Node* to_balance = myErase(to_delete);
    if (to_balance) {
        checkBalance(to_balance);
    }
    --size_;
}

Node* AVLTree::myErase(Node* node) {
    if (node->left == empty_node_ && node->right == empty_node_) {
        if (node->parent && node->parent->left == node) {
            node->parent->left = empty_node_;
        } else if (node->parent) {
            node->parent->right = empty_node_;
        }

        if (root_ == node) {
            root_ = nullptr;
        }

        Node* to_return = node->parent;
        delete node;
        return to_return;
    } else if (node->left != empty_node_ && node->right == empty_node_) {
        Node* left_node = node->left;
        if (node->parent && node->parent->left == node) {
            node->parent->left = left_node;
        } else if (node->parent) {
            node->parent->right = left_node;
        }
        if (node->parent) {
            left_node->parent = node->parent;
        } else {
            left_node->parent = nullptr;
        }

        if (root_ == node) {
            root_ = left_node;
        }

        delete node;
        return left_node;
    } else if (node->left == empty_node_ && node->right != empty_node_) {
        Node* right_node = node->right;
        if (node->parent && node->parent->left == node) {
            node->parent->left = right_node;
        } else if (node->parent) {
            node->parent->right = right_node;
        }
        if (node->parent) {
            right_node->parent = node->parent;
        } else {
            right_node->parent = nullptr;
        }

        if (root_ == node) {
            root_ = right_node;
        }

        delete node;
        return right_node;
    } else {
        Node* to_replace = node;
        if (to_replace->left != empty_node_) {
            to_replace = to_replace->left;
        }
        while (to_replace->right != empty_node_) {
            to_replace = to_replace->right;
        }
        Node* to_return = to_replace;

        if (to_replace->parent && to_replace->parent != node) {
            to_replace->parent->right = to_replace->left;
            to_replace->left->parent = to_replace->parent;
            to_return = to_replace->parent;
        }

        to_replace->parent = node->parent;
        if (node->parent && node->parent->right == node) {
            node->parent->right = to_replace;
        } else if (node->parent) {
            node->parent->left = to_replace;
        }
        if (to_replace != node->left) {
            to_replace->left = node->left;
        }
        if (to_replace != node->left && node->left != empty_node_) {
            node->left->parent = to_replace;
        }
        to_replace->right = node->right;
        if (node->right != empty_node_) {
            node->right->parent = to_replace;
        }

        if (root_ == node) {
            root_ = to_replace;
        }

        delete node;
        return to_return;
    }
}

int* AVLTree::find(int value) {
    Node* found = myFind(value);
    if (found) {
        return &found->value;
    }
    return nullptr;
}

Node* AVLTree::myFind(int value) {
    if (root_ == nullptr) {
        return nullptr;
    }
    Node* current = root_;
    while (current != empty_node_ && current->value != value) {
        if (value > current->value) {
            current = current->right;
        } else {
            current = current->left;
        }
    }

    if (current == empty_node_ || (current == root_ && root_->value != value)) {
        return nullptr;
    }

    return current;
}

int* AVLTree::traversal() {
    int* array = new int[size_];
    int i = 0;
    symmetric(root_, &i, array);

    return array;
}

void AVLTree::symmetric(Node* node, int* i, int* array) {
    if (node == empty_node_ || node == nullptr) {
        return;
    }
    symmetric(node->left, i, array);
    array[*i] = node->value;
    ++*i;
    symmetric(node->right, i, array);
}

int* AVLTree::lowerBound(int value) {
    Node* current = root_;
    while (current != nullptr && current->value != value) {
        if (value > current->value) {
            if (current->right == empty_node_) {
                break;
            }
            current = current->right;
        } else {
            if (current->left == empty_node_) {
                break;
            }
            current = current->left;
        }
    }

    if (current == nullptr) {
        return nullptr;
    }

    if (current->value == value) {
        return &current->value;
    }

    if (current->value > value) {
        return &current->value;
    } else {
        while (current->parent) {
            if (current->parent->value > value) {
                return &current->parent->value;
            }
            current = current->parent;
        }
        return nullptr;
    }
}

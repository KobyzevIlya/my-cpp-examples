#include <initializer_list>
#include <algorithm>

enum class Color { RED, BLACK };

template <typename T>
struct Node {
    T key;
    Node<T>* left = nullptr;
    Node<T>* right = nullptr;
    Node<T>* parent = nullptr;
    Color color;

    int height;

    Node(){};
    Node(T x) : key(x), height(1){};
};

template <typename T>
class RBTree {
public:
    RBTree();
    RBTree(std::initializer_list<T> list);
    ~RBTree();

    void insert(T key);

    int size() const;

    bool empty() const;

    T* lowerBound(T key) const;

    T* find(T key) const;

    Node<T>* root = nullptr;

private:
    Node<T>* leftTurn(Node<T>* left_node, Node<T>* right_node);
    Node<T>* smallLeftTurn(Node<T>* left_node, Node<T>* right_node);
    Node<T>* bigLeftTurn(Node<T>* left_node, Node<T>* right_node);

    Node<T>* rightTurn(Node<T>* left_node, Node<T>* right_node);
    Node<T>* smallRightTurn(Node<T>* left_node, Node<T>* right_node);
    Node<T>* bigRightTurn(Node<T>* left_node, Node<T>* right_node);

    void setEmptyChildren(Node<T>* node);
    void rebuildRBT(Node<T>* node);
    void clear(Node<T>* node);
    void setNodeHeight(Node<T>* node);
    void setAllParentsHeight(Node<T>* node);
    Node<T>* myFind(T key) const;

    std::size_t size_ = 0;
};

template <typename T>
RBTree<T>::RBTree() {
}

template <typename T>
RBTree<T>::RBTree(std::initializer_list<T> list) {
    for (auto key : list) {
        insert(key);
    }
}

template <typename T>
RBTree<T>::~RBTree() {
    clear(root);
}

template <typename T>
int RBTree<T>::size() const {
    return size_;
}

template <typename T>
bool RBTree<T>::empty() const {
    return root == nullptr;
}

template <typename T>
T* RBTree<T>::lowerBound(T key) const {
    Node<T>* current = root;
    while (current != nullptr && current->key != key) {
        if (key > current->key) {
            if (!current->right) {
                break;
            }
            current = current->right;
        } else {
            if (!current->left) {
                break;
            }
            current = current->left;
        }
    }

    if (current == nullptr) {
        return nullptr;
    }

    if (current->key == key) {
        return &current->key;
    }

    if (current->key > key) {
        return &current->key;
    } else {
        while (current->parent) {
            if (current->parent->key > key) {
                return &current->parent->key;
            }
            current = current->parent;
        }
        return nullptr;
    }
}

template <typename T>
T* RBTree<T>::find(T key) const {
    Node<T>* found = myFind(key);
    if (found) {
        return &found->key;
    }
    return nullptr;
}

template <typename T>
Node<T>* RBTree<T>::myFind(T key) const {
    if (root == nullptr) {
        return nullptr;
    }
    Node<T>* current = root;
    while (current && current->key != key) {
        if (key > current->key) {
            current = current->right;
        } else {
            current = current->left;
        }
    }

    if (!current || (current == root && root->key != key)) {
        return nullptr;
    }

    return current;
}

template <typename T>
void RBTree<T>::insert(T key) {
    if (root == nullptr) {
        root = new Node<T>(key);

        root->color = Color::BLACK;
        setEmptyChildren(root);
        size_ = 1;
    } else {
        Node<T>* current = root;
        while (key != current->key) {
            if (key < current->key) {
                if (!current->left) {
                    current->left = new Node<T>(key);
                    current->left->parent = current;
                    setEmptyChildren(current->left);

                    current->left->color = Color::RED;
                    rebuildRBT(current->left);

                    ++size_;
                    return;
                } else {
                    current = current->left;
                }
            } else if (key > current->key) {
                if (!current->right) {
                    current->right = new Node<T>(key);
                    current->right->parent = current;
                    setEmptyChildren(current->right);

                    current->right->color = Color::RED;
                    rebuildRBT(current->right);

                    ++size_;
                    return;
                } else {
                    current = current->right;
                }
            }
        }
    }
}

template <typename T>
void RBTree<T>::rebuildRBT(Node<T>* node) {
    if (!node->parent) {
        node->color = Color::BLACK;
        setNodeHeight(node);
        if (node->left) {
            setNodeHeight(node->left);
        }
        if (node->right) {
            setNodeHeight(node->right);
        }
    }

    if (!(node->color == Color::RED && node->parent->color == Color::RED)) {
        setAllParentsHeight(node);
        return;
    }

    // если есть дед
    if (node->parent->parent) {
        // если отец - левый потомок
        if (node->parent->parent->left == node->parent) {
            // если дядя красный
            if (node->parent->parent->right && node->parent->parent->right->color == Color::RED) {
                node->parent->parent->color = Color::RED;
                node->parent->parent->left->color = Color::BLACK;
                node->parent->parent->right->color = Color::BLACK;

                setNodeHeight(node->parent);
                setNodeHeight(node->parent->parent);
                rebuildRBT(node->parent->parent);
                return;
            }
            // если дядя - черный или его нет
            if (!node->parent->parent->right ||
                node->parent->parent->right->color == Color::BLACK) {
                setNodeHeight(node->parent);
                setNodeHeight(node->parent->parent);

                Node<T>* grandfather = rightTurn(node->parent, node->parent->parent);
                grandfather->color = Color::BLACK;
                grandfather->right->color = Color::RED;
                return;
            }
        }
        // если отец - правый потомок
        if (node->parent->parent->right == node->parent) {
            // если дядя красный
            if (node->parent->parent->left && node->parent->parent->left->color == Color::RED) {
                node->parent->parent->color = Color::RED;
                node->parent->parent->left->color = Color::BLACK;
                node->parent->parent->right->color = Color::BLACK;

                setNodeHeight(node->parent);
                setNodeHeight(node->parent->parent);
                rebuildRBT(node->parent->parent);
                return;
            }
            // если дядя - черный или его нет
            if (!node->parent->parent->left || node->parent->parent->left->color == Color::BLACK) {
                setNodeHeight(node->parent);
                setNodeHeight(node->parent->parent);

                Node<T>* grandfather = leftTurn(node->parent->parent, node->parent);
                grandfather->color = Color::BLACK;
                grandfather->left->color = Color::RED;
                return;
            }
        }
    } else {
        rebuildRBT(node->parent);
    }
}

template <typename T>
Node<T>* RBTree<T>::leftTurn(Node<T>* left_node, Node<T>* right_node) {
    int left_height = 0;
    int right_height = 0;

    if (right_node->left) {
        left_height = right_node->left->height;
    }
    if (right_node->right) {
        right_height = right_node->right->height;
    }

    if (left_height > right_height) {
        return bigLeftTurn(left_node, right_node);
    } else {
        return smallLeftTurn(left_node, right_node);
    }
}

template <typename T>
Node<T>* RBTree<T>::smallLeftTurn(Node<T>* left_node, Node<T>* right_node) {
    if (left_node == root) {
        root = right_node;
    }

    right_node->parent = left_node->parent;
    left_node->parent = right_node;
    left_node->right = right_node->left;
    if (right_node->left) {
        right_node->left->parent = left_node;
    }
    right_node->left = left_node;

    if (right_node->parent && right_node->parent->key < right_node->key) {
        right_node->parent->right = right_node;
    } else if (right_node->parent) {
        right_node->parent->left = right_node;
    }

    setNodeHeight(left_node);
    setNodeHeight(right_node);

    return right_node;
}

template <typename T>
Node<T>* RBTree<T>::bigLeftTurn(Node<T>* left_node, Node<T>* right_node) {
    Node<T>* middle_node = right_node->left;
    middle_node->parent = left_node->parent;

    if (left_node == root) {
        root = middle_node;
    }

    left_node->parent = middle_node;
    left_node->right = middle_node->left;
    if (middle_node->left) {
        middle_node->left->parent = left_node;
    }

    right_node->parent = middle_node;
    right_node->left = middle_node->right;
    if (middle_node->right) {
        middle_node->right->parent = right_node;
    }

    middle_node->left = left_node;
    middle_node->right = right_node;

    if (middle_node->parent && middle_node->parent->key < middle_node->key) {
        middle_node->parent->right = middle_node;
    } else if (middle_node->parent) {
        middle_node->parent->left = middle_node;
    }

    setNodeHeight(left_node);
    setNodeHeight(right_node);
    setNodeHeight(middle_node);

    return middle_node;
}

template <typename T>
Node<T>* RBTree<T>::rightTurn(Node<T>* left_node, Node<T>* right_node) {
    int left_height = 0;
    int right_height = 0;

    if (left_node->left) {
        left_height = left_node->left->height;
    }
    if (left_node->right) {
        right_height = left_node->right->height;
    }

    if (right_height > left_height) {
        return bigRightTurn(left_node, right_node);
    } else {
        return smallRightTurn(left_node, right_node);
    }
}

template <typename T>
Node<T>* RBTree<T>::smallRightTurn(Node<T>* left_node, Node<T>* right_node) {
    if (right_node == root) {
        root = left_node;
    }

    left_node->parent = right_node->parent;
    right_node->parent = left_node;
    right_node->left = left_node->right;
    if (left_node->right) {
        left_node->right->parent = right_node;
    }
    left_node->right = right_node;

    if (left_node->parent && left_node->parent->key < left_node->key) {
        left_node->parent->right = left_node;
    } else if (left_node->parent) {
        left_node->parent->left = left_node;
    }

    setNodeHeight(right_node);
    setNodeHeight(left_node);

    return left_node;
}

template <typename T>
Node<T>* RBTree<T>::bigRightTurn(Node<T>* left_node, Node<T>* right_node) {
    Node<T>* middle_node = left_node->right;
    middle_node->parent = right_node->parent;

    if (right_node == root) {
        root = middle_node;
    }

    right_node->parent = middle_node;
    right_node->left = middle_node->right;
    if (middle_node->left) {
        middle_node->right->parent = right_node;
    }

    left_node->parent = middle_node;
    left_node->right = middle_node->left;
    if (middle_node->left) {
        middle_node->left->parent = left_node;
    }

    middle_node->left = left_node;
    middle_node->right = right_node;

    if (middle_node->parent && middle_node->parent->key < middle_node->key) {
        middle_node->parent->right = middle_node;
    } else if (middle_node->parent) {
        middle_node->parent->left = middle_node;
    }

    setNodeHeight(left_node);
    setNodeHeight(right_node);
    setNodeHeight(middle_node);

    return middle_node;
}

template <typename T>
void RBTree<T>::setEmptyChildren(Node<T>* node) {
    node->left = nullptr;
    node->right = nullptr;
}

template <typename T>
void RBTree<T>::setNodeHeight(Node<T>* node) {
    if (node->left && node->right) {
        node->height = std::max(node->left->height, node->right->height) + 1;
    } else if (node->left) {
        node->height = node->left->height + 1;
    } else if (node->right) {
        node->height = node->right->height + 1;
    } else {
        node->height = 1;
    }
}

template <typename T>
void RBTree<T>::setAllParentsHeight(Node<T>* node) {
    Node<T>* current = node;
    while (current) {
        if (current->left && current->right) {
            current->height = std::max(current->left->height, current->right->height) + 1;
        } else if (current->left) {
            current->height = current->left->height + 1;
        } else if (current->right) {
            current->height = current->right->height + 1;
        } else {
            current->height = 1;
        }
        current = current->parent;
    }
}

template <typename T>
void RBTree<T>::clear(Node<T>* node) {
    if (node != nullptr) {
        clear(node->left);
        clear(node->right);
        delete node;
    }
}

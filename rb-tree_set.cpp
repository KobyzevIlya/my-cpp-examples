#include <algorithm>
#include <utility>

enum class Color { RED, BLACK, DOUBLEBLACK };

template <typename T>
struct Node {
    T value;

    Node<T>* left = nullptr;
    Node<T>* right = nullptr;
    Node<T>* parent = nullptr;

    int height = 1;

    Color color;

    Node(){};
    explicit Node(const T& x) : value(x){};

    Node<T>* copy();
};

template <typename T>
Node<T>* Node<T>::copy() {
    Node* copy = new Node<T>;

    copy->color = color;
    copy->value = value;
    copy->height = height;

    return copy;
}

template <typename ValueType>
class RBTree {
public:
    struct Iterator {
        friend Iterator RBTree::begin() const;
        friend Iterator RBTree::end() const;

        Iterator(){};
        explicit Iterator(Node<ValueType>* node);

        const ValueType& operator*() const;
        const ValueType* operator->() const;

        Iterator& operator++();
        Iterator operator++(int);

        Iterator& operator--();
        Iterator operator--(int);

        bool operator==(const Iterator& other) const;
        bool operator!=(const Iterator& other) const;

    private:
        Node<ValueType>* node_ptr_ = nullptr;
        Node<ValueType>* root_parent_ptr_ = nullptr;

        Node<ValueType>* goNext(Node<ValueType>* node);
        Node<ValueType>* goPrevious(Node<ValueType>* node);
    };

    RBTree(){};
    RBTree(std::initializer_list<ValueType> list);
    RBTree(const RBTree& other);

    RBTree<ValueType>& operator=(const RBTree& other);

    ~RBTree();

    void insert(const ValueType& value);
    void erase(const ValueType& value);

    size_t size() const;
    bool empty() const;

    Iterator lowerBound(const ValueType& value) const;
    Iterator find(const ValueType& value) const;

    Iterator begin() const;
    Iterator end() const;

    Node<ValueType>* root = nullptr;

private:
    size_t size_ = 0;
    Node<ValueType>* root_parent_ = nullptr;

    Node<ValueType>* leftTurn(Node<ValueType>* left_node, Node<ValueType>* right_node);
    Node<ValueType>* smallLeftTurn(Node<ValueType>* left_node, Node<ValueType>* right_node);
    Node<ValueType>* bigLeftTurn(Node<ValueType>* left_node, Node<ValueType>* right_node);

    Node<ValueType>* rightTurn(Node<ValueType>* left_node, Node<ValueType>* right_node);
    Node<ValueType>* smallRightTurn(Node<ValueType>* left_node, Node<ValueType>* right_node);
    Node<ValueType>* bigRightTurn(Node<ValueType>* left_node, Node<ValueType>* right_node);

    void setEmptyChildren(Node<ValueType>* node);
    void insertRebuildRBT(Node<ValueType>* node);
    void eraseRebuildRBT(Node<ValueType>* node);
    void clear(Node<ValueType>* node);
    void setNodeHeight(Node<ValueType>* node);
    void setAllParentsHeight(Node<ValueType>* node);
    Node<ValueType>* treeCopyHelper(Node<ValueType>* parent, Node<ValueType>* to_copy);
    Node<ValueType>* findNode(const ValueType value) const;
};

template <typename ValueType>
RBTree<ValueType>::RBTree(const RBTree& other) {
    root = other.root->copy();
    root_parent_ = other.root_parent_->copy();
    root_parent_->left = root;
    size_ = other.size_;

    root->left = treeCopyHelper(root, other.root->left);
    root->right = treeCopyHelper(root, other.root->right);
}

template <typename ValueType>
RBTree<ValueType>::RBTree(std::initializer_list<ValueType> list) {
    for (auto x : list) {
        insert(x);
    }
}

template <typename ValueType>
RBTree<ValueType>::~RBTree() {
    clear(root);
    if (root_parent_) {
        delete root_parent_;
    }
}

template <typename ValueType>
RBTree<ValueType>& RBTree<ValueType>::operator=(const RBTree& other) {
    RBTree<ValueType> copy(other);
    std::swap(size_, copy.size_);
    std::swap(root, copy.root);
    std::swap(root_parent_, copy.root_parent_);

    return *this;
}

template <typename ValueType>
Node<ValueType>* RBTree<ValueType>::treeCopyHelper(Node<ValueType>* parent,
                                                   Node<ValueType>* to_copy) {
    if (!to_copy) {
        return nullptr;
    }

    Node<ValueType>* copy = to_copy->copy();

    copy->parent = parent;
    copy->left = treeCopyHelper(copy, to_copy->left);
    copy->right = treeCopyHelper(copy, to_copy->right);

    return copy;
}

template <typename ValueType>
size_t RBTree<ValueType>::size() const {
    return size_;
}

template <typename ValueType>
bool RBTree<ValueType>::empty() const {
    return size_ == 0;
}

template <typename ValueType>
typename RBTree<ValueType>::Iterator RBTree<ValueType>::lowerBound(const ValueType& value) const {
    Node<ValueType>* current = findNode(value);

    if (!current) {
        return Iterator(root_parent_);
    }

    if (/*current->value == value*/ !(current->value < value) && !(value < current->value)) {
        return Iterator(current);
    }

    if (value < current->value) {
        return Iterator(current);
    } else {
        while (current->parent) {
            if (value < current->parent->value) {
                return Iterator(current->parent);
            }
            current = current->parent;
        }
        return Iterator(root_parent_);
    }
}

template <typename ValueType>
typename RBTree<ValueType>::Iterator RBTree<ValueType>::find(const ValueType& value) const {
    Node<ValueType>* current = findNode(value);

    if (!current) {
        return Iterator(root_parent_);
    }

    if (/*current->value == value*/ !(current->value < value) && !(value < current->value)) {
        return Iterator(current);
    } else {
        return Iterator(root_parent_);
    }
}

template <typename ValueType>
Node<ValueType>* RBTree<ValueType>::findNode(const ValueType value) const {
    Node<ValueType>* current = root;
    if (!root) {
        return nullptr;
    }
    while (current != nullptr &&
           /*current->value != value*/ (current->value < value || value < current->value)) {
        if (current->value < value) {
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
    if (!(current->value < value) && !(value < current->value)) {
        return current;
    }
    return nullptr;
}

template <typename ValueType>
void RBTree<ValueType>::insert(const ValueType& value) {
    if (!root) {
        root = new Node<ValueType>(value);

        root_parent_ = new Node<ValueType>;
        root_parent_->height = -1;
        root_parent_->left = root;

        root->color = Color::BLACK;
        setEmptyChildren(root);
        size_ = 1;
    } else {
        Node<ValueType>* current = root;
        while (/*current->value != value*/ (current->value < value || value < current->value)) {
            if (value < current->value) {
                if (!current->left) {
                    current->left = new Node<ValueType>(value);
                    current->left->parent = current;
                    setEmptyChildren(current->left);

                    current->left->color = Color::RED;
                    insertRebuildRBT(current->left);

                    ++size_;
                    return;
                } else {
                    current = current->left;
                }
            } else if (current->value < value) {
                if (!current->right) {
                    current->right = new Node<ValueType>(value);
                    current->right->parent = current;
                    setEmptyChildren(current->right);

                    current->right->color = Color::RED;
                    insertRebuildRBT(current->right);

                    ++size_;
                    return;
                } else {
                    current = current->right;
                }
            }
        }
    }
}

template <typename ValueType>
void RBTree<ValueType>::insertRebuildRBT(Node<ValueType>* node) {
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
                insertRebuildRBT(node->parent->parent);
                return;
            }
            // если дядя - черный или его нет
            if (!node->parent->parent->right ||
                node->parent->parent->right->color == Color::BLACK) {
                setNodeHeight(node->parent);
                setNodeHeight(node->parent->parent);

                Node<ValueType>* grandfather = rightTurn(node->parent, node->parent->parent);
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
                insertRebuildRBT(node->parent->parent);
                return;
            }
            // если дядя - черный или его нет
            if (!node->parent->parent->left || node->parent->parent->left->color == Color::BLACK) {
                setNodeHeight(node->parent);
                setNodeHeight(node->parent->parent);

                Node<ValueType>* grandfather = leftTurn(node->parent->parent, node->parent);
                grandfather->color = Color::BLACK;
                grandfather->left->color = Color::RED;
                return;
            }
        }
    } else {
        insertRebuildRBT(node->parent);
    }
}

template <typename ValueType>
Node<ValueType>* RBTree<ValueType>::leftTurn(Node<ValueType>* left_node,
                                             Node<ValueType>* right_node) {
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

template <typename ValueType>
Node<ValueType>* RBTree<ValueType>::smallLeftTurn(Node<ValueType>* left_node,
                                                  Node<ValueType>* right_node) {
    if (left_node == root) {
        root = right_node;

        root_parent_->left = right_node;
    }

    right_node->parent = left_node->parent;
    left_node->parent = right_node;
    left_node->right = right_node->left;
    if (right_node->left) {
        right_node->left->parent = left_node;
    }
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

template <typename ValueType>
Node<ValueType>* RBTree<ValueType>::bigLeftTurn(Node<ValueType>* left_node,
                                                Node<ValueType>* right_node) {
    Node<ValueType>* middle_node = right_node->left;
    middle_node->parent = left_node->parent;

    if (left_node == root) {
        root = middle_node;

        root_parent_->left = middle_node;
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

template <typename ValueType>
Node<ValueType>* RBTree<ValueType>::rightTurn(Node<ValueType>* left_node,
                                              Node<ValueType>* right_node) {
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

template <typename ValueType>
Node<ValueType>* RBTree<ValueType>::smallRightTurn(Node<ValueType>* left_node,
                                                   Node<ValueType>* right_node) {
    if (right_node == root) {
        root = left_node;

        root_parent_->left = left_node;
    }

    left_node->parent = right_node->parent;
    right_node->parent = left_node;
    right_node->left = left_node->right;
    if (left_node->right) {
        left_node->right->parent = right_node;
    }
    left_node->right = right_node;

    if (left_node->parent && left_node->parent->value < left_node->value) {
        left_node->parent->right = left_node;
    } else if (left_node->parent) {
        left_node->parent->left = left_node;
    }

    setNodeHeight(right_node);
    setNodeHeight(left_node);

    return left_node;
}

template <typename ValueType>
Node<ValueType>* RBTree<ValueType>::bigRightTurn(Node<ValueType>* left_node,
                                                 Node<ValueType>* right_node) {
    Node<ValueType>* middle_node = left_node->right;
    middle_node->parent = right_node->parent;

    if (right_node == root) {
        root = middle_node;

        root_parent_->left = middle_node;
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

template <typename ValueType>
void RBTree<ValueType>::setEmptyChildren(Node<ValueType>* node) {
    node->left = nullptr;
    node->right = nullptr;
}

template <typename ValueType>
void RBTree<ValueType>::setNodeHeight(Node<ValueType>* node) {
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

template <typename ValueType>
void RBTree<ValueType>::setAllParentsHeight(Node<ValueType>* node) {
    Node<ValueType>* current = node;
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

template <typename ValueType>
void RBTree<ValueType>::erase(const ValueType& value) {
    Node<ValueType>* current = findNode(value);

    if (!current || current == root_parent_) {
        return;
    }

    // если есть оба потомка
    if (current->left && current->right) {
        Node<ValueType>* saved = current;

        current = current->right;
        while (current->left) {
            current = current->left;
        }

        saved->value = current->value;
    }

    // если нет потомков
    if (!current->left && !current->right) {
        // удаление красной
        if (current->color == Color::RED) {
            if (current->parent->left == current) {
                current->parent->left = nullptr;
            } else if (current->parent->right == current) {
                current->parent->right = nullptr;
            }
            setAllParentsHeight(current->parent);

            --size_;
            delete current;
            return;
        }

        // удаление корня
        if (!current->parent) {
            --size_;
            delete current;
            root = nullptr;
            return;
        }

        // удаление черной без потомков
        if (current->color == Color::BLACK) {
            current->color = Color::DOUBLEBLACK;
            eraseRebuildRBT(current);
            if (current->parent) {
                if (current->parent->left == current) {
                    current->parent->left = nullptr;
                } else if (current->parent->right == current) {
                    current->parent->right = nullptr;
                }
            }
            --size_;
            delete current;
            return;
        }
    } else if (!current->right && current->left) {
        // удаление черной с одним левым потомком
        if (current->color == Color::BLACK) {
            Node<ValueType>* to_delete = current->left;

            current->value = current->left->value;
            current->left = current->left->left;
            if (current->right) {
                current->right = current->left->right;
            }

            setAllParentsHeight(current);
            --size_;
            delete to_delete;
            return;
        }
    } else if (!current->left && current->right) {
        // удаление черной с одним правым потомком
        if (current->color == Color::BLACK) {
            Node<ValueType>* to_delete = current->right;

            current->value = current->right->value;
            current->right = current->right->right;
            if (current->left) {
                current->left = current->right->left;
            }

            setAllParentsHeight(current);
            --size_;
            delete to_delete;
            return;
        }
    }
}

template <typename ValueType>
void RBTree<ValueType>::eraseRebuildRBT(Node<ValueType>* node) {
    if (!node->parent) {
        node->color = Color::BLACK;
        return;
    }

    // если вершина - левый потомок
    if (node->parent->left == node) {
        // если брат - красный
        if (node->parent->right && node->parent->right->color == Color::RED) {
            smallLeftTurn(node->parent, node->parent->right);
            node->parent->parent->color = Color::BLACK;
            node->parent->color = Color::RED;
            eraseRebuildRBT(node);
        } else if (!node->parent->right || node->parent->right->color == Color::BLACK) {
            // если брат - черный
            // если дети брата черные или брата нет
            if (!node->parent->right ||
                ((!node->parent->right->left || node->parent->right->left->color == Color::BLACK) &&
                 (!node->parent->right->right ||
                  node->parent->right->right->color == Color::BLACK))) {
                if (node->parent->right) {
                    node->parent->right->color = Color::RED;
                }
                if (node->parent && node->parent->color == Color::RED) {
                    node->parent->color = Color::BLACK;
                    node->color = Color::BLACK;
                    return;
                } else {
                    node->parent->color = Color::DOUBLEBLACK;
                    node->color = Color::BLACK;
                    eraseRebuildRBT(node->parent);
                }
            } else if (node->parent->right->right &&
                       node->parent->right->right->color == Color::RED) {
                // если у правого брата правый ребенок - красный
                smallLeftTurn(node->parent, node->parent->right);
                node->parent->parent->color = node->parent->color;
                node->parent->color = Color::BLACK;
                node->color = Color::BLACK;
                if (node->parent->parent && node->parent->parent->right) {
                    node->parent->parent->right->color = Color::BLACK;
                }
                return;
            } else if (node->parent->right->left->color == Color::RED &&
                       (!node->parent->right->right ||
                        node->parent->right->right->color == Color::BLACK)) {
                // если у правого брата левый ребенок - красный, а правый - черный
                smallRightTurn(node->parent->right->left, node->parent->right);
                if (node->parent->right) {
                    node->parent->right->color = Color::BLACK;
                    if (node->parent->right->right) {
                        node->parent->right->right->color = Color::RED;
                    }
                }
                eraseRebuildRBT(node);
            }
        }
    } else
        // если вершина - правый потомок
        if (node->parent->right == node) {
            // если брат - красный
            if (node->parent->left && node->parent->left->color == Color::RED) {
                smallRightTurn(node->parent->left, node->parent);
                node->parent->parent->color = Color::BLACK;
                node->parent->color = Color::RED;
                eraseRebuildRBT(node);
            } else if (!node->parent->left || node->parent->left->color == Color::BLACK) {
                // если брат - черный
                // если дети брата черные или брата нет
                if (!node->parent->left || ((!node->parent->left->left ||
                                             node->parent->left->left->color == Color::BLACK) &&
                                            (!node->parent->left->right ||
                                             node->parent->left->right->color == Color::BLACK))) {
                    if (node->parent->left) {
                        node->parent->left->color = Color::RED;
                    }
                    if (node->parent && node->parent->color == Color::RED) {
                        node->parent->color = Color::BLACK;
                        node->color = Color::BLACK;
                        return;
                    } else {
                        node->parent->color = Color::DOUBLEBLACK;
                        node->color = Color::BLACK;
                        eraseRebuildRBT(node->parent);
                    }
                } else if (node->parent->left->left &&
                           node->parent->left->left->color == Color::RED) {
                    // если у левого брата левый ребенок - красный
                    smallRightTurn(node->parent->left, node->parent);
                    node->parent->parent->color = node->parent->color;
                    node->parent->color = Color::BLACK;
                    node->color = Color::BLACK;
                    if (node->parent->parent && node->parent->parent->left) {
                        node->parent->parent->left->color = Color::BLACK;
                    }
                    return;
                } else if (node->parent->left->right->color == Color::RED &&
                           (!node->parent->left->left ||
                            node->parent->left->left->color == Color::BLACK)) {
                    // если у левого брата правый ребенок - красный, а левый - черный
                    smallLeftTurn(node->parent->left, node->parent->left->right);
                    if (node->parent->left) {
                        node->parent->left->color = Color::BLACK;
                        if (node->parent->left->left) {
                            node->parent->left->left->color = Color::RED;
                        }
                    }
                    eraseRebuildRBT(node);
                }
            }
        }
}

template <typename ValueType>
void RBTree<ValueType>::clear(Node<ValueType>* node) {
    if (node != nullptr) {
        clear(node->left);
        clear(node->right);
        delete node;
    }
}

template <typename ValueType>
typename RBTree<ValueType>::Iterator RBTree<ValueType>::begin() const {
    if (!root) {
        return Iterator(root_parent_);
    }

    RBTree<ValueType>::Iterator begin_iterator(root_parent_);
    while (begin_iterator.node_ptr_->left) {
        begin_iterator.node_ptr_ = begin_iterator.node_ptr_->left;
    }
    return begin_iterator;
}
template <typename ValueType>
typename RBTree<ValueType>::Iterator RBTree<ValueType>::end() const {
    return RBTree::Iterator(root_parent_);
}

// Iterator
template <typename ValueType>
RBTree<ValueType>::Iterator::Iterator(Node<ValueType>* node) {
    if (node->height == -1) {
        root_parent_ptr_ = node;
    }
    node_ptr_ = node;
}

template <typename ValueType>
const ValueType& RBTree<ValueType>::Iterator::operator*() const {
    return node_ptr_->value;
}

template <typename ValueType>
const ValueType* RBTree<ValueType>::Iterator::operator->() const {
    return &node_ptr_->value;
}

template <typename ValueType>
typename RBTree<ValueType>::Iterator& RBTree<ValueType>::Iterator::operator++() {
    Node<ValueType>* current = goNext(node_ptr_);
    if (!current) {
        node_ptr_ = root_parent_ptr_;
    } else {
        node_ptr_ = current;
    }

    return *this;
}

template <typename ValueType>
typename RBTree<ValueType>::Iterator RBTree<ValueType>::Iterator::operator++(int) {
    Iterator copy = Iterator(node_ptr_);
    Node<ValueType>* current = goNext(node_ptr_);
    if (!current) {
        node_ptr_ = root_parent_ptr_;
    } else {
        node_ptr_ = current;
    }
    return copy;
}

template <typename ValueType>
typename RBTree<ValueType>::Iterator& RBTree<ValueType>::Iterator::operator--() {
    node_ptr_ = goPrevious(node_ptr_);

    return *this;
}

template <typename ValueType>
typename RBTree<ValueType>::Iterator RBTree<ValueType>::Iterator::operator--(int) {
    Iterator copy = Iterator(node_ptr_);
    node_ptr_ = goPrevious(node_ptr_);
    return copy;
}

template <typename ValueType>
bool RBTree<ValueType>::Iterator::operator==(const RBTree::Iterator& other) const {
    return node_ptr_ == other.node_ptr_;
}

template <typename ValueType>
bool RBTree<ValueType>::Iterator::operator!=(const RBTree::Iterator& other) const {
    return node_ptr_ != other.node_ptr_;
}

template <typename ValueType>
Node<ValueType>* RBTree<ValueType>::Iterator::goNext(Node<ValueType>* node) {
    if (!node || node == root_parent_ptr_) {
        return node;
    }
    Node<ValueType>* current = node;
    if (current->right) {
        current = current->right;
        while (current->left) {
            current = current->left;
        }
        return current;
    } else {
        while (current->parent && current->parent->value < current->value) {
            current = current->parent;
        }
        return current->parent;
    }
}

template <typename ValueType>
Node<ValueType>* RBTree<ValueType>::Iterator::goPrevious(Node<ValueType>* node) {
    if (!node || node == root_parent_ptr_) {
        return node;
    }
    Node<ValueType>* current = node;
    if (current->left) {
        current = current->left;
        while (current->right) {
            current = current->right;
        }
        return current;
    } else {
        while (current->parent && current->value < current->parent->value) {
            current = current->parent;
        }
        return current->parent;
    }
}

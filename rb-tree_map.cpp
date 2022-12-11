#include <algorithm>
#include <utility>

enum class Color { RED, BLACK, DOUBLEBLACK };

template <typename K, typename V>
struct Node {
    K key;
    V value;
    Node<K, V>* left = nullptr;
    Node<K, V>* right = nullptr;
    Node<K, V>* parent = nullptr;
    Color color;

    Node(){};
    Node(const K& key, const V& value) : key(key), value(value){};

    Node<K, V>* copy();
};

template <typename K, typename V>
Node<K, V>* Node<K, V>::copy() {
    Node* copy = new Node<K, V>;

    copy->color = color;
    copy->key = key;
    copy->value = value;

    return copy;
}

template <typename K, typename V>
class Map {
public:
    struct Iterator {
        friend Iterator Map::begin() const;
        friend Iterator Map::end() const;

        Iterator(){};
        explicit Iterator(Node<K, V>* node);

        const std::pair<K, V>& operator*() const;
        const std::pair<K, V>* operator->() const;

        Iterator& operator++();
        Iterator operator++(int);

        Iterator& operator--();
        Iterator operator--(int);

        bool operator==(const Iterator& other) const;
        bool operator!=(const Iterator& other) const;

    private:
        Node<K, V>* node_ptr_ = nullptr;
        Node<K, V>* root_parent_ptr_ = nullptr;

        Node<K, V>* goNext(Node<K, V>* node);
        Node<K, V>* goPrevious(Node<K, V>* node);

        std::pair<K, V> values_;
    };

    Map(){};
    Map(std::initializer_list<std::pair<K, V>> list);
    Map(const Map& other);

    Map<K, V>& operator=(const Map& other);

    ~Map();

    void insert(const K& key, const V& value);
    void erase(const K& key);

    size_t size() const;
    bool empty() const;

    Iterator lowerBound(const K& key) const;
    Iterator find(const K& key) const;

    Iterator begin() const;
    Iterator end() const;

    Node<K, V>* root = nullptr;

private:
    size_t size_ = 0;
    Node<K, V>* root_parent_ = nullptr;

    Node<K, V>* smallLeftTurn(Node<K, V>* left_node, Node<K, V>* right_node);
    Node<K, V>* smallRightTurn(Node<K, V>* left_node, Node<K, V>* right_node);

    void setEmptyChildren(Node<K, V>* node);
    void insertRebuildRBT(Node<K, V>* node);
    void eraseRebuildRBT(Node<K, V>* node);
    void clear(Node<K, V>* node);
    Node<K, V>* treeCopyHelper(Node<K, V>* parent, Node<K, V>* to_copy);
    Node<K, V>* findNode(const K& key) const;
};

template <typename K, typename V>
Map<K, V>::Map(std::initializer_list<std::pair<K, V>> list) {
    for (auto x : list) {
        insert(x.first, x.second);
    }
}

template <typename K, typename V>
Map<K, V>::Map(const Map& other) {
    root = other.root->copy();
    root_parent_ = other.root_parent_->copy();
    root_parent_->left = root;
    size_ = other.size_;

    root->left = treeCopyHelper(root, other.root->left);
    root->right = treeCopyHelper(root, other.root->right);
}

template <typename K, typename V>
Node<K, V>* Map<K, V>::treeCopyHelper(Node<K, V>* parent, Node<K, V>* to_copy) {
    if (!to_copy) {
        return nullptr;
    }

    Node<K, V>* copy = to_copy->copy();

    copy->parent = parent;
    copy->left = treeCopyHelper(copy, to_copy->left);
    copy->right = treeCopyHelper(copy, to_copy->right);

    return copy;
}

template <typename K, typename V>
Map<K, V>::~Map() {
    clear(root);
    if (root_parent_) {
        delete root_parent_;
    }
}

template <typename K, typename V>
void Map<K, V>::clear(Node<K, V>* node) {
    if (node != nullptr) {
        clear(node->left);
        clear(node->right);
        delete node;
    }
}

template <typename K, typename V>
size_t Map<K, V>::size() const {
    return size_;
}

template <typename K, typename V>
bool Map<K, V>::empty() const {
    return size_ == 0;
}

template <typename K, typename V>
Map<K, V>& Map<K, V>::operator=(const Map& other) {
    Map<K, V> copy(other);
    std::swap(size_, copy.size_);
    std::swap(root, copy.root);
    std::swap(root_parent_, copy.root_parent_);

    return *this;
}

template <typename K, typename V>
void Map<K, V>::insert(const K& key, const V& value) {
    if (!root) {
        root = new Node<K, V>(key, value);

        root_parent_ = new Node<K, V>;
        root_parent_->color = Color::DOUBLEBLACK;
        root_parent_->left = root;

        root->color = Color::BLACK;
        setEmptyChildren(root);
        size_ = 1;
    } else {
        Node<K, V>* current = root;
        while (/*current->key != key*/ (current->key < key || key < current->key)) {
            if (key < current->key) {
                if (!current->left) {
                    current->left = new Node<K, V>(key, value);
                    current->left->parent = current;
                    setEmptyChildren(current->left);

                    current->left->color = Color::RED;
                    insertRebuildRBT(current->left);

                    ++size_;
                    return;
                } else {
                    current = current->left;
                }
            } else if (current->key < key) {
                if (!current->right) {
                    current->right = new Node<K, V>(key, value);
                    current->right->parent = current;
                    setEmptyChildren(current->right);

                    current->right->color = Color::RED;
                    insertRebuildRBT(current->right);

                    ++size_;
                    return;
                } else {
                    current = current->right;
                }
            } else if (/*current->key == key*/ !(current->key < key) && !(key < current->key)) {
                current->value = value;
                return;
            }
        }
    }
}

template <typename K, typename V>
void Map<K, V>::insertRebuildRBT(Node<K, V>* node) {
    if (!node->parent) {
        node->color = Color::BLACK;
    }

    if (!(node->color == Color::RED && node->parent->color == Color::RED)) {
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

                insertRebuildRBT(node->parent->parent);
                return;
            }
            // если дядя - черный или его нет
            if (!node->parent->parent->right ||
                node->parent->parent->right->color == Color::BLACK) {

                // если текущий узел - правый потомок
                if (node->parent->right == node) {
                    smallLeftTurn(node->parent, node);
                    smallRightTurn(node, node->parent);
                    node->color = Color::BLACK;
                    node->right->color = Color::RED;
                    return;
                }

                smallRightTurn(node->parent, node->parent->parent);
                node->parent->color = Color::BLACK;
                node->parent->right->color = Color::RED;
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

                insertRebuildRBT(node->parent->parent);
                return;
            }
            // если дядя - черный или его нет
            if (!node->parent->parent->left || node->parent->parent->left->color == Color::BLACK) {

                // если текущий узел - левый потомок
                if (node->parent->left == node) {
                    smallRightTurn(node, node->parent);
                    smallLeftTurn(node->parent, node);
                    node->color = Color::BLACK;
                    node->left->color = Color::RED;
                    return;
                }

                smallLeftTurn(node->parent->parent, node->parent);
                node->parent->color = Color::BLACK;
                node->parent->left->color = Color::RED;
                return;
            }
        }
    } else {
        insertRebuildRBT(node->parent);
    }
}

template <typename K, typename V>
Node<K, V>* Map<K, V>::smallLeftTurn(Node<K, V>* left_node, Node<K, V>* right_node) {
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

    if (right_node->parent && right_node->parent->key < right_node->key) {
        right_node->parent->right = right_node;
    } else if (right_node->parent) {
        right_node->parent->left = right_node;
    }

    return right_node;
}

template <typename K, typename V>
Node<K, V>* Map<K, V>::smallRightTurn(Node<K, V>* left_node, Node<K, V>* right_node) {
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

    if (left_node->parent && left_node->parent->key < left_node->key) {
        left_node->parent->right = left_node;
    } else if (left_node->parent) {
        left_node->parent->left = left_node;
    }

    return left_node;
}

template <typename K, typename V>
void Map<K, V>::erase(const K& key) {
    Node<K, V>* current = findNode(key);

    if (!current || current == root_parent_) {
        return;
    }

    // если есть оба потомка
    if (current->left && current->right) {
        Node<K, V>* saved = current;

        current = current->right;
        while (current->left) {
            current = current->left;
        }

        saved->key = current->key;
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
            Node<K, V>* to_delete = current->left;

            current->key = current->left->key;
            current->left = current->left->left;
            if (current->right) {
                current->right = current->left->right;
            }

            --size_;
            delete to_delete;
            return;
        }
    } else if (!current->left && current->right) {
        // удаление черной с одним правым потомком
        if (current->color == Color::BLACK) {
            Node<K, V>* to_delete = current->right;

            current->key = current->right->key;
            current->right = current->right->right;
            if (current->left) {
                current->left = current->right->left;
            }

            --size_;
            delete to_delete;
            return;
        }
    }
}

template <typename K, typename V>
void Map<K, V>::eraseRebuildRBT(Node<K, V>* node) {
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
    } else if (node->parent->right == node) {
        // если вершина - правый потомок
        // если брат - красный
        if (node->parent->left && node->parent->left->color == Color::RED) {
            smallRightTurn(node->parent->left, node->parent);
            node->parent->parent->color = Color::BLACK;
            node->parent->color = Color::RED;
            eraseRebuildRBT(node);
        } else if (!node->parent->left || node->parent->left->color == Color::BLACK) {
            // если брат - черный
            // если дети брата черные или брата нет
            if (!node->parent->left ||
                ((!node->parent->left->left || node->parent->left->left->color == Color::BLACK) &&
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
            } else if (node->parent->left->left && node->parent->left->left->color == Color::RED) {
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

template <typename K, typename V>
void Map<K, V>::setEmptyChildren(Node<K, V>* node) {
    node->left = nullptr;
    node->right = nullptr;
}

template <typename K, typename V>
typename Map<K, V>::Iterator Map<K, V>::lowerBound(const K& key) const {
    Node<K, V>* current = root;
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
        return Iterator(root_parent_);
    }

    if (current->key == key) {
        return Iterator(current);
    }

    if (current->key > key) {
        return Iterator(current);
    } else {
        while (current->parent) {
            if (current->parent->key > key) {
                return Iterator(current->parent);
            }
            current = current->parent;
        }
        return Iterator(root_parent_);
    }
}

template <typename K, typename V>
typename Map<K, V>::Iterator Map<K, V>::find(const K& key) const {
    Node<K, V>* current = findNode(key);

    if (!current) {
        return Iterator(root_parent_);
    }

    if (/*current->key == key*/ !(current->key < key) && !(key < current->key)) {
        return Iterator(current);
    } else {
        return Iterator(root_parent_);
    }
}

template <typename K, typename V>
Node<K, V>* Map<K, V>::findNode(const K& key) const {
    Node<K, V>* current = root;
    if (!root) {
        return nullptr;
    }
    while (current != nullptr &&
           /*current->key != key*/ (current->key < key || key < current->key)) {
        if (current->key < key) {
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
    if (!(current->key < key) && !(key < current->key)) {
        return current;
    }
    return nullptr;
}

template <typename K, typename V>
typename Map<K, V>::Iterator Map<K, V>::begin() const {
    if (!root) {
        return Iterator(root_parent_);
    }

    Map<K, V>::Iterator begin_iterator(root_parent_);
    while (begin_iterator.node_ptr_->left) {
        begin_iterator.values_ = {begin_iterator.node_ptr_->left->key,
                                  begin_iterator.node_ptr_->left->value};
        begin_iterator.node_ptr_ = begin_iterator.node_ptr_->left;
    }
    return begin_iterator;
}

template <typename K, typename V>
typename Map<K, V>::Iterator Map<K, V>::end() const {
    return Map<K, V>::Iterator(root_parent_);
}

// Iterator
template <typename K, typename V>
Map<K, V>::Iterator::Iterator(Node<K, V>* node) {
    if (node->color == Color::DOUBLEBLACK) {
        root_parent_ptr_ = node;
    }
    node_ptr_ = node;

    if (node_ptr_) {
        values_.first = node_ptr_->key;
        values_.second = node_ptr_->value;
    }
}

template <typename K, typename V>
const std::pair<K, V>& Map<K, V>::Iterator::operator*() const {
    return values_;
}

template <typename K, typename V>
const std::pair<K, V>* Map<K, V>::Iterator::operator->() const {
    return &values_;
}

template <typename K, typename V>
typename Map<K, V>::Iterator& Map<K, V>::Iterator::operator++() {
    Node<K, V>* current = goNext(node_ptr_);
    if (!current) {
        node_ptr_ = root_parent_ptr_;
    } else {
        node_ptr_ = current;
        values_.first = node_ptr_->key;
        values_.second = node_ptr_->value;
    }

    return *this;
}

template <typename K, typename V>
typename Map<K, V>::Iterator Map<K, V>::Iterator::operator++(int) {
    Iterator copy = Iterator(node_ptr_);
    Node<K, V>* current = goNext(node_ptr_);
    if (!current) {
        node_ptr_ = root_parent_ptr_;
    } else {
        node_ptr_ = current;
        values_.first = node_ptr_->key;
        values_.second = node_ptr_->value;
    }
    return copy;
}

template <typename K, typename V>
typename Map<K, V>::Iterator& Map<K, V>::Iterator::operator--() {
    node_ptr_ = goPrevious(node_ptr_);

    if (node_ptr_) {
        values_.first = node_ptr_->key;
        values_.second = node_ptr_->value;
    }

    return *this;
}

template <typename K, typename V>
typename Map<K, V>::Iterator Map<K, V>::Iterator::operator--(int) {
    Iterator copy = Iterator(node_ptr_);
    node_ptr_ = goPrevious(node_ptr_);

    if (node_ptr_) {
        values_.first = node_ptr_->key;
        values_.second = node_ptr_->value;
    }

    return copy;
}

template <typename K, typename V>
bool Map<K, V>::Iterator::operator==(const Map::Iterator& other) const {
    return node_ptr_ == other.node_ptr_;
    ;
}

template <typename K, typename V>
bool Map<K, V>::Iterator::operator!=(const Map::Iterator& other) const {
    return node_ptr_ != other.node_ptr_;
}

template <typename K, typename V>
Node<K, V>* Map<K, V>::Iterator::goNext(Node<K, V>* node) {
    if (!node || node == root_parent_ptr_) {
        return node;
    }
    Node<K, V>* current = node;
    if (current->right) {
        current = current->right;
        while (current->left) {
            current = current->left;
        }
        return current;
    } else {
        while (current->parent && current->parent->key < current->key) {
            current = current->parent;
        }
        return current->parent;
    }
}

template <typename K, typename V>
Node<K, V>* Map<K, V>::Iterator::goPrevious(Node<K, V>* node) {
    if (!node || node == root_parent_ptr_) {
        return node;
    }
    if (node == root_parent_ptr_) {
        node_ptr_ = root_parent_ptr_->left;
        while (node_ptr_->right) {
            node_ptr_ = node_ptr_->right;
        }
    }
    Node<K, V>* current = node;
    if (current->left) {
        current = current->left;
        while (current->right) {
            current = current->right;
        }
        return current;
    } else {
        while (current->parent && current->key < current->parent->key) {
            current = current->parent;
        }
        return current->parent;
    }
}

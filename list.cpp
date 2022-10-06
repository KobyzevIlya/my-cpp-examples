#include <iostream>

class Node {
public:
    int data = 0;
    Node* next = nullptr;
    Node* previous = nullptr;

    Node(){};

    Node(const int data) {
        Node::data = data;
    }

    ~Node(){};
};

class List {
private:
    size_t size_ = 0;

public:
    List() {
        head = nullptr;
        tail = head;
    }

    List(int* values, size_t size) {
        for (int i = 0; i < static_cast<int>(size); ++i) {
            if (values[i] >= 2 * 1e9 || values[i] <= -2 * 1e9) {
                for (int j = 0; j < i; ++j) {
                    this->pop();
                }

                throw std::runtime_error("Wrong Value!");
            }

            this->pushBack(values[i]);
        }
    }

    ~List() {
        while (size_ != 0) {
            this->pop();
        }
    }

    int size() {
        return static_cast<int>(size_);
    }

    void pushBack(int value) {
        if (value > 2 * 1e9 || value < -2 * 1e9) {
            throw std::runtime_error("Wrong Value!");
        }

        Node* new_node = new Node(value);

        if (!head) {
            head = new_node;
            tail = head;

            tail->next = head;
            head->previous = tail;
        } else {
            if (tail == head) {
                tail = new_node;
                head->next = tail;
                tail->previous = head;

                head->previous = tail;
                tail->next = head;
            } else {
                new_node->previous = tail;
                tail->next = new_node;
                tail = tail->next;

                head->previous = tail;
                tail->next = head;
            }
        }
        ++size_;
    }

    void pushFront(int value) {
        if (value > 2 * 1e9 || value < -2 * 1e9) {
            throw std::runtime_error("Wrong Value!");
        }

        Node* new_node = new Node(value);

        if (!head) {
            head = new_node;
            tail = head;

            tail->next = head;
            head->previous = tail;
        } else {
            if (head == tail) {
                head = new_node;
                head->next = tail;
                tail->previous = head;

                head->previous = tail;
                tail->next = head;
            } else {
                head->previous = new_node;
                new_node->next = head;
                head = head->previous;

                head->previous = tail;
                tail->next = head;
            }
        }
        ++size_;
    }

    int pop() {
        if (!head) {
            throw std::runtime_error("Can not pop such element!");
        }

        int value_to_return = head->data;

        if (head == tail) {
            delete head;

            head = tail = nullptr;

            --size_;

            return value_to_return;
        }

        Node* to_delete = head;

        head = head->next;
        head->previous = tail;
        tail->next = head;

        --size_;

        delete to_delete;

        return value_to_return;
    }

    int pop(size_t position) {
        if (position + 1 >= size_) {
            throw std::runtime_error("Wrong Position!");
        }
        --size_;

        Node* current = head;

        for (int i = 0; i <= static_cast<int>(position); ++i) {
            current = current->next;
        }

        int value_to_return = current->data;

        if (current->next == head) {
            tail = tail->previous;
            tail->next = head;
            head->previous = tail;

            delete current;

            return value_to_return;
        }

        current->previous->next = current->next;
        current->next->previous = current->previous;

        delete current;

        return value_to_return;
    }

    void push(int value, size_t position) {
        if (position >= size_) {
            throw std::runtime_error("Wrong Position!");
        }
        if (value > 2 * 1e9 || value < -2 * 1e9) {
            throw std::runtime_error("Wrong Value!");
        }

        ++size_;

        Node* current = head;
        Node* new_node = new Node(value);

        for (int i = 0; i < static_cast<int>(position); ++i) {
            current = current->next;
        }

        if (head == tail) {
            tail = new_node;
            tail->previous = head;
            tail->next = head;
            head->previous = tail;
            head->next = tail;
            return;
        }

        if (current->previous == tail) {
            new_node->next = head->next;
            new_node->previous = head;
            head->next = new_node;
            return;
        }

        if (current->next == head) {
            tail->next = new_node;
            new_node->previous = tail;
            new_node->next = head;
            head->previous = new_node;
            tail = new_node;
            return;
        }

        new_node->next = current->next;
        new_node->previous = current;
        current->next = new_node;
    }

    Node* head{};
    Node* tail{};
};

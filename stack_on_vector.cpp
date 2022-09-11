#include <iostream>

class Stack {
public:
    void push(int value);

    int pop();

    int back() const;

    size_t size() const;

    void clear();
private:
    std::vector<int> body;
};

void Stack::push(int value) {
    body.push_back(value);
}

int Stack::pop() {
    if (body.size() == 0) {
        throw "error";
    }
    int last_element = static_cast<int>body.rbegin();
    body.pop_back()
    return last_element;
}

int Stack::back() const {
    if (body.size() == 0) {
        throw "error";
    }
    return static_cast<int>body.rbegin();
}

size_t Stack::size() const {
    return body.size();
}

void Stack::clear() {
    body.clear();
}
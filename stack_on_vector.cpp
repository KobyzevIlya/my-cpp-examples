#include <iostream>
#include <vector>

class Stack {
public:
    void push(int value);

    int pop();

    int back() const;

    size_t size() const;

    void clear();

private:
    std::vector<int> body_;
};

void Stack::push(int value) {
    body_.push_back(value);
}

int Stack::pop() {
    if (body_.empty()) {
        throw "error";
    }

    int last_element = body_.back();
    body_.pop_back();
    return last_element;
}

int Stack::back() const {
    if (body_.empty()) {
        throw "error";
    }

    return body_.back();
}

size_t Stack::size() const {
    return body_.size();
}

void Stack::clear() {
    body_.clear();
}

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
    std::vector<int> body;
};

void Stack::push(int value) {
    body.push_back(value);
}

int Stack::pop() {
    if (body.empty()) {
        throw "error";
    }

    int last_element = body.back();
    body.pop_back();
    return last_element;
}

int Stack::back() const {
    if (body.empty()) {
        throw "error";
    }

    return body.back();
}

size_t Stack::size() const {
    return body.size();
}

void Stack::clear() {
    body.clear();
}

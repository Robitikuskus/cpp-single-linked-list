#pragma once

#include <algorithm>
#include <cassert>
#include <iterator>
#include <iostream>
#include <vector>

template <typename Type>
class SingleLinkedList {
    struct Node {
        Node() = default;
        Node(const Type& val, Node* next_node)
            : value(val)
            , next(next_node) {
        }
        Type value;
        Node* next = nullptr;
    };

    template <typename ValueType>
    class BasicIterator {
        template <typename T>
        friend class SingleLinkedList;

        explicit BasicIterator(Node* node)
        : node_(node) {}

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Type;
        using difference_type = std::ptrdiff_t;
        using pointer = ValueType*;
        using reference = ValueType&;

        BasicIterator() = default;

        BasicIterator(const BasicIterator<Type>& other) noexcept {
            node_ = other.node_;
        }

        BasicIterator& operator=(const BasicIterator& rhs) = default;

        bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
            return node_ == rhs.node_;
        }

        bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
            return !(node_ == rhs.node_);
        }

        bool operator==(const BasicIterator<Type>& rhs) const noexcept {
            return node_ == rhs.node_;
        }

        bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
            return !(node_ == rhs.node_);
        }

        BasicIterator& operator++() noexcept {
            assert(node_ != nullptr);
            node_ = node_->next;
            return *this;
        }

        BasicIterator operator++(int) noexcept {
            auto prev_value(*this);
            ++(*this);
            return prev_value;
        }

        reference operator*() const noexcept {
            assert(node_ != nullptr);
            return node_->value;
        }

        pointer operator->() const noexcept {
            assert(node_ != nullptr);
            return &node_->value;
        }

    private:
        Node* node_ = nullptr;
    };

public:
    SingleLinkedList()
    : size_(0) {
        head_.next = nullptr;
    };

    template <typename Iterator>
    SingleLinkedList(Iterator begin, Iterator end) {
        Node* current = &head_;
        try {
            for (auto it = begin; it != end; ++it) {
                current->next = new Node(*it, nullptr);
                current = current->next;
                ++size_;
            }
        } catch (...) {
            Clear();
            throw;
        }
    }

    SingleLinkedList(std::initializer_list<Type> values)
    : SingleLinkedList(values.begin(), values.end()) {
    }

    SingleLinkedList(const SingleLinkedList& other)
    : SingleLinkedList(other.begin(), other.end()) {
    }

    SingleLinkedList& operator=(const SingleLinkedList& rhs) {
        if (head_.next != rhs.head_.next) {
            SingleLinkedList t(rhs.begin(), rhs.end());
            swap(t);
            size_ = rhs.size_;
        }
        return *this;
    }

    void swap(SingleLinkedList& other) noexcept {
        std::swap(head_.next, other.head_.next);
        std::swap(size_, other.size_);
    }

    ~SingleLinkedList() {
        Clear();
    }

    using value_type = Type;
    using reference = value_type&;
    using const_reference = const value_type&;

    using Iterator = BasicIterator<Type>;
    using ConstIterator = BasicIterator<const Type>;

    Iterator begin() noexcept {
        return Iterator{head_.next};
    }

    Iterator end() noexcept {
        return Iterator(nullptr);
    }

    ConstIterator begin() const noexcept {
        return ConstIterator{head_.next};
    }

    ConstIterator end() const noexcept {
        return ConstIterator{nullptr};
    }

    ConstIterator cbegin() const noexcept {
        return begin();
    }

    ConstIterator cend() const noexcept {
        return end();
    }

    size_t GetSize() const noexcept {
        return size_;
    }

    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    void PushFront(const Type& value) {
        head_.next = new Node(value, head_.next);
        ++size_;
    }

    void Clear() {
        while (head_.next != nullptr) {
            Node* next = head_.next;
            head_.next = next->next;
            delete next;
        }
        size_ = 0;
    }

    Iterator before_begin() noexcept {
        return Iterator(&head_);
    }

    ConstIterator cbefore_begin() const noexcept {
        return ConstIterator(const_cast<Node*>(&head_));
    }

    ConstIterator before_begin() const noexcept {
        return cbefore_begin();
    }

    Iterator InsertAfter(ConstIterator pos, const Type& value) {
        if (pos.node_ == nullptr) {
            return Iterator(nullptr);
        }

        try {
            Node* new_node = new Node(value, pos.node_->next);
            pos.node_->next = new_node;
            ++size_;

            return Iterator(new_node);
        } catch (std::bad_alloc&) {
            throw;
        }
    }

    void PopFront() noexcept {
        if (!IsEmpty()) {
            Node* t = head_.next->next;
            delete head_.next;
            head_.next = t;

            --size_;
        }
    }

    Iterator EraseAfter(ConstIterator pos) noexcept {
        if (pos.node_ == nullptr || pos.node_->next == nullptr) {
            return Iterator(nullptr);
        }

        Node* target = pos.node_->next;
        pos.node_->next = target->next;
        delete target;
        --size_;

        return Iterator(pos.node_->next);
    }

private:
    Node head_;
    size_t size_ = 0;
};

template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
    lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return lhs < rhs || lhs == rhs;
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs <= rhs);
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(rhs < lhs);
} 
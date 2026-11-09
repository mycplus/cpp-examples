// binary_search_tree.hpp - a binary search tree of std::unique_ptr-owned
// nodes. Keys are unique; Compare must be a strict weak ordering, like the
// comparator of std::set.
#ifndef MYCPLUS_BINARY_SEARCH_TREE_HPP
#define MYCPLUS_BINARY_SEARCH_TREE_HPP

#include <cstddef>
#include <functional>
#include <memory>
#include <queue>
#include <utility>
#include <vector>

template <typename T, typename Compare = std::less<T>>
class BinarySearchTree {
    struct Node {
        explicit Node(T k) : key(std::move(k)) {}
        T key;
        std::unique_ptr<Node> left;    // keys that compare before key
        std::unique_ptr<Node> right;   // keys that compare after key
    };

public:
    BinarySearchTree() = default;
    explicit BinarySearchTree(Compare comp) : comp_(std::move(comp)) {}
    ~BinarySearchTree() { clear(); }

    BinarySearchTree(const BinarySearchTree&) = delete;
    BinarySearchTree& operator=(const BinarySearchTree&) = delete;

    BinarySearchTree(BinarySearchTree&& other) noexcept
        : root_(std::move(other.root_)), size_(std::exchange(other.size_, 0)),
          comp_(std::move(other.comp_)) {}

    BinarySearchTree& operator=(BinarySearchTree&& other) noexcept
    {
        if (this != &other) {
            clear();                   // not root_ = ...: that would recurse
            root_ = std::move(other.root_);
            size_ = std::exchange(other.size_, 0);
            comp_ = std::move(other.comp_);
        }
        return *this;
    }

    // Returns false if an equivalent key was already present.
    bool insert(T key)
    {
        std::unique_ptr<Node>* link = &root_;
        while (*link) {
            if (comp_(key, (*link)->key))
                link = &(*link)->left;
            else if (comp_((*link)->key, key))
                link = &(*link)->right;
            else
                return false;          // neither is before the other: equivalent
        }
        *link = std::make_unique<Node>(std::move(key));
        ++size_;
        return true;
    }

    [[nodiscard]] bool contains(const T& key) const
    {
        const Node* n = root_.get();
        while (n) {
            if (comp_(key, n->key))
                n = n->left.get();
            else if (comp_(n->key, key))
                n = n->right.get();
            else
                return true;
        }
        return false;
    }

    // Returns false if the key was not present.
    bool erase(const T& key)
    {
        std::unique_ptr<Node>* link = &root_;
        while (*link && (comp_(key, (*link)->key) || comp_((*link)->key, key)))
            link = comp_(key, (*link)->key) ? &(*link)->left : &(*link)->right;
        if (!*link)
            return false;

        Node& n = **link;
        if (!n.left || !n.right) {
            // Take the child out first: assigning *link from a member of the
            // node it owns would destroy that node mid-assignment.
            std::unique_ptr<Node> child = std::move(n.left ? n.left : n.right);
            *link = std::move(child);
        } else {
            // Two children: move the successor's key in, then unlink the
            // successor, which has no left child.
            std::unique_ptr<Node>* succ = &n.right;
            while ((*succ)->left)
                succ = &(*succ)->left;
            n.key = std::move((*succ)->key);
            std::unique_ptr<Node> right = std::move((*succ)->right);
            *succ = std::move(right);
        }
        --size_;
        return true;
    }

    // In-order with an explicit stack: keys in sorted order, no recursion.
    template <typename F>
    void for_each_inorder(F visit) const
    {
        std::vector<const Node*> stack;
        const Node* n = root_.get();
        while (n || !stack.empty()) {
            if (n) {
                stack.push_back(n);
                n = n->left.get();
            } else {
                n = stack.back();
                stack.pop_back();
                visit(n->key);
                n = n->right.get();
            }
        }
    }

    template <typename F>
    void for_each_preorder(F visit) const
    {
        std::vector<const Node*> stack;
        if (root_)
            stack.push_back(root_.get());
        while (!stack.empty()) {
            const Node* n = stack.back();
            stack.pop_back();
            visit(n->key);
            if (n->right)                  // pushed first, so visited second
                stack.push_back(n->right.get());
            if (n->left)
                stack.push_back(n->left.get());
        }
    }

    // Postorder is preorder with the children swapped, reversed.
    template <typename F>
    void for_each_postorder(F visit) const
    {
        std::vector<const Node*> stack, out;
        if (root_)
            stack.push_back(root_.get());
        while (!stack.empty()) {
            const Node* n = stack.back();
            stack.pop_back();
            out.push_back(n);
            if (n->left)
                stack.push_back(n->left.get());
            if (n->right)
                stack.push_back(n->right.get());
        }
        for (auto it = out.rbegin(); it != out.rend(); ++it)
            visit((*it)->key);
    }

    template <typename F>
    void for_each_level_order(F visit) const
    {
        std::queue<const Node*> queue;
        if (root_)
            queue.push(root_.get());
        while (!queue.empty()) {
            const Node* n = queue.front();
            queue.pop();
            visit(n->key);
            if (n->left)
                queue.push(n->left.get());
            if (n->right)
                queue.push(n->right.get());
        }
    }

    // Height in edges: -1 for an empty tree. Counts levels; no recursion.
    [[nodiscard]] long height() const
    {
        std::vector<const Node*> level, next;
        if (root_)
            level.push_back(root_.get());
        long h = -1;
        while (!level.empty()) {
            ++h;
            next.clear();
            for (const Node* n : level) {
                if (n->left)
                    next.push_back(n->left.get());
                if (n->right)
                    next.push_back(n->right.get());
            }
            level.swap(next);
        }
        return h;
    }

    [[nodiscard]] std::size_t size() const noexcept { return size_; }
    [[nodiscard]] bool empty() const noexcept { return size_ == 0; }

    // Rotates left children up until the root has none, then drops the
    // root, so destroying even a chain of a million nodes does not recurse.
    void clear() noexcept
    {
        while (root_) {
            if (root_->left) {
                std::unique_ptr<Node> l = std::move(root_->left);
                root_->left = std::move(l->right);
                l->right = std::move(root_);
                root_ = std::move(l);
            } else {
                std::unique_ptr<Node> right = std::move(root_->right);
                root_ = std::move(right);      // frees the old root, which has no children
            }
        }
        size_ = 0;
    }

private:
    std::unique_ptr<Node> root_;
    std::size_t size_ = 0;
    Compare comp_;
};

#endif

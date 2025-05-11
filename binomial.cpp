#include <iostream>
#include <fstream>
#include <limits>

template <typename T>
class Binomial {
private:
    struct Node {
        T key;
        int degree;
        Node* parent;
        Node* child;
        Node* sibling;

        Node(const T& val) : key(val), degree(0), parent(nullptr), child(nullptr), sibling(nullptr) {}
    };

    Node* head;

    Node* merge_root_lists(Node* h1, Node* h2) {
        if (!h1) return h2;
        if (!h2) return h1;

        Node* result;
        if (h1->degree <= h2->degree) {
            result = h1;
            h1 = h1->sibling;
        } else {
            result = h2;
            h2 = h2->sibling;
        }

        Node* tail = result;

        while (h1 && h2) {
            if (h1->degree <= h2->degree) {
                tail->sibling = h1;
                h1 = h1->sibling;
            } else {
                tail->sibling = h2;
                h2 = h2->sibling;
            }
            tail = tail->sibling;
        }

        tail->sibling = (h1 ? h1 : h2);
        return result;
    }

    void link_trees(Node* y, Node* z) {
        y->parent = z;
        y->sibling = z->child;
        z->child = y;
        z->degree++;
    }

    Node* union_heaps(Node* h1, Node* h2) {
        Node* new_head = merge_root_lists(h1, h2);
        if (!new_head) return nullptr;

        Node* prev = nullptr;
        Node* curr = new_head;
        Node* next = curr->sibling;

        while (next) {
            if ((curr->degree != next->degree) ||
                (next->sibling && next->sibling->degree == curr->degree)) {
                prev = curr;
                curr = next;
            } else {
                if (curr->key <= next->key) {
                    curr->sibling = next->sibling;
                    link_trees(next, curr);
                } else {
                    if (!prev) new_head = next;
                    else prev->sibling = next;
                    link_trees(curr, next);
                    curr = next;
                }
            }
            next = curr->sibling;
        }

        return new_head;
    }

    Node* extract_min_node(Node*& min_prev) const {
        if (!head) return nullptr;

        Node* min_node = head;
        Node* prev_min = nullptr;
        Node* prev = head;
        Node* curr = head->sibling;

        while (curr) {
            if (curr->key < min_node->key) {
                min_node = curr;
                min_prev = prev;
            }
            prev = curr;
            curr = curr->sibling;
        }

        return min_node;
    }

    void delete_tree(Node* node) {
        if (!node) return;
        delete_tree(node->child);
        delete_tree(node->sibling);
        delete node;
    }

public:
    Binomial() : head(nullptr) {}

    ~Binomial() {
        delete_tree(head);
    }

    bool insert(const T& value) {
        Binomial<T> tempHeap;
        tempHeap.head = new Node(value);
        head = union_heaps(head, tempHeap.head);
        tempHeap.head = nullptr;
        return true;
    }

    T get_min() const {
        if (!head) return std::numeric_limits<T>::max();

        Node* curr = head;
        Node* min_node = head;
        while (curr) {
            if (curr->key < min_node->key)
                min_node = curr;
            curr = curr->sibling;
        }

        return min_node->key;
    }

    T delete_min() {
        if (!head) return std::numeric_limits<T>::max();

        Node* min_prev = nullptr;
        Node* min_node = extract_min_node(min_prev);

        if (min_prev) min_prev->sibling = min_node->sibling;
        else head = min_node->sibling;

        Node* child = min_node->child;
        Node* reversed_child = nullptr;
        while (child) {
            Node* next = child->sibling;
            child->sibling = reversed_child;
            child->parent = nullptr;
            reversed_child = child;
            child = next;
        }

        head = union_heaps(head, reversed_child);

        T min_value = min_node->key;
        delete min_node;
        return min_value;
    }

    void reunion(Binomial<T>& other) {
        if (head == other.head) return;
        head = union_heaps(head, other.head);
        other.head = nullptr; // clear moved-from heap
    }
};

int main(int argc, char *argv[])
{
	if (argc < 2) {
		std::cout << "INSERT FILE NAME\n";
		return 0;
	}

	std::ifstream fin(argv[1]);

	int nrheaps, ops, op, number, number2, heap, heap2;
	fin >> nrheaps >> ops;
	Binomial<int> heaps[nrheaps];
	for (int i = 0; i < ops; i++) {
		fin >> op;
		std::cout << i << ": ";
		switch (op) {
			case 1:
				fin >> number >> heap;
				std::cout << "insert: " << heaps[heap].insert(number) << '\n';
				break;
			case 2:
				fin >> heap;
				std::cout << "get: " << heaps[heap].get_min() << '\n';
				break;
			case 3:
				fin >> heap;
				std::cout << "delete: " << heaps[heap].delete_min()<< '\n';
				break;
			case 4:
				fin >> heap >> heap2;
				std::cout << "reunion! " << heap << ' ' << heap2 << '\n';
				heaps[heap].reunion(heaps[heap2]);
				break;
		}
	}
	return 0;
}

#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>

template <typename T>
class Fibonacci { // i know the pieces fit
private:
    struct Node {
        T key;
        int degree;
        bool mark;
        Node* parent;
        Node* child;
        Node* left;
        Node* right;

        Node(const T& val)
            : key(val), degree(0), mark(false), parent(nullptr),
              child(nullptr), left(this), right(this) {}
    };

    Node* minNode;
    int n;

    void link(Node* y, Node* x) {
        y->left->right = y->right;
        y->right->left = y->left;


        y->parent = x;
        y->left = y->right = y;
        if (!x->child)
            x->child = y;
        else {
            y->left = x->child;
            y->right = x->child->right;
            x->child->right->left = y;
            x->child->right = y;
        }

        x->degree++; // isi da grade, isi permite
        y->mark = false;
    }

    void consolidate() {
        int D = static_cast<int>(std::log2(n)) + 2; // hmmmm
        std::vector<Node*> A(D, nullptr);

        std::vector<Node*> rootList;
        Node* w = minNode;
        if (w) {
            do {
                rootList.push_back(w);
                w = w->right;
            } while (w != minNode);
        }

        for (Node* w : rootList) {
            Node* x = w;
            int d = x->degree;
            while (A[d]) {
                Node* y = A[d];
                if (x->key > y->key)
                    std::swap(x, y);
                link(y, x);
                A[d] = nullptr;
                d++;
            }
            A[d] = x;
        }

        minNode = nullptr;
        for (Node* a : A) {
            if (a) {
                if (!minNode) {
                    a->left = a->right = a;
                    minNode = a;
                } else {
                    a->left = minNode;
                    a->right = minNode->right;
                    minNode->right->left = a;
                    minNode->right = a;
                    if (a->key < minNode->key)
                        minNode = a;
                }
            }
        }
    }

    void delete_all(Node* node) {
        if (!node) return;
        Node* curr = node;
        do {
            Node* next = curr->right;
            if (curr->child)
                delete_all(curr->child);
            delete curr;
            curr = next;
        } while (curr != node);
    }

    void insert_node(Node* node) {
        if (!minNode) {
            minNode = node;
        } else {
            node->left = minNode;
            node->right = minNode->right;
            minNode->right->left = node;
            minNode->right = node;
            if (node->key < minNode->key)
                minNode = node;
        }
    }

public:
    Fibonacci() : minNode(nullptr), n(0) {}
    ~Fibonacci() {
        delete_all(minNode);
    }

    bool insert(const T& value) {
        Node* node = new Node(value);
        insert_node(node);
        ++n;
        return true;
    }

    T get_min() const {
        if (!minNode) return std::numeric_limits<T>::max(); // cei mai mici vor fi cei mai mari
        return minNode->key;
    }

    T delete_min() {
        if (!minNode) return std::numeric_limits<T>::max();
        Node* z = minNode;
        if (z->child) {
            Node* x = z->child;
            do {
                Node* next = x->right;
                insert_node(x);
                x->parent = nullptr;
                x = next;
            } while (x != z->child);
        }



        z->left->right = z->right;
        z->right->left = z->left;
        if (z == z->right) {
            minNode = nullptr;
        } else {
            minNode = z->right;
            consolidate();
        }
        T minValue = z->key;
        delete z;
        --n;
        return minValue;
    }

    void reunion(Fibonacci<T>& other) {
        if (!other.minNode) return;
        if (!minNode) {
            minNode = other.minNode;
            n = other.n;
        } else {
            Node* a = minNode->right;
            Node* b = other.minNode->right;
            minNode->right = b;
            b->left = minNode;
            other.minNode->right = a;
            a->left = other.minNode;
            if (other.minNode->key < minNode->key)
                minNode = other.minNode;
            n += other.n;
        }
        other.minNode = nullptr;
        other.n = 0;
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
	Fibonacci<int> heaps[nrheaps];
	for (int i = 0; i < ops; i++) {
		fin >> op;
		std::cout << i << ": ";
		switch (op) {
			case 1:
				fin >> number >> heap;
				std::cout << "insert: " << number << '\n';
				heaps[heap].insert(number);
				break;
			case 2:
				fin >> heap;
				std::cout << "get: " << heaps[heap].get_min() << '\n';
				break;
			case 3:
				fin >> heap;
				std::cout << "delete: " << heaps[heap].delete_min();
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

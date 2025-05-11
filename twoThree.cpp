#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <limits>

template<typename data_type>
class TwoThree {
private:
    struct node {
        std::vector<data_type> keys;
        std::vector<node*> kids;
        node* parent;

        node(data_type val) : keys{val}, parent(nullptr) {}

        bool isLeaf() const { return kids.empty(); }
        bool isFull() const { return keys.size() == 2; }

        void insertKey(data_type val) {
            keys.push_back(val);
            sort(keys.begin(), keys.end());
        }
    };

    node* root = nullptr;
    int nodeCount = 0;

    node* findMinNode(node* nd, data_type& result) const {
        if (!nd || nd->keys.empty()) return nullptr;

        node* minNode = nd;
        result = nd->keys.front();

        for (const auto& k : nd->keys)
            if (k < result) result = k;

        for (auto* child : nd->kids) {
            data_type childMin;
            node* temp = findMinNode(child, childMin);
            if (temp && childMin < result) {
                result = childMin;
                minNode = temp;
            }
        }

        return minNode;
    }

    node* findNode(node* nd, const data_type& val) const {
        if (!nd) return nullptr;

        if (find(nd->keys.begin(), nd->keys.end(), val) != nd->keys.end()) return nd;
        if (nd->isLeaf()) return nullptr;

        if (val < nd->keys[0]) return findNode(nd->kids[0], val);
        if (nd->keys.size() == 1 || val < nd->keys[1]) return findNode(nd->kids[1], val);
        return findNode(nd->kids[2], val);
    }

    void removeFromNode(node* nd, data_type val) {
        auto it = find(nd->keys.begin(), nd->keys.end(), val);
        if (it != nd->keys.end()) nd->keys.erase(it);
    }

    void destroy(node* nd) {
        if (!nd) return;
        for (auto* kid : nd->kids) destroy(kid); // ceva de genul ca disown child pe linux
        delete nd;
    }

public:
    TwoThree() = default;
    ~TwoThree() { clear(); }

    bool insert(const data_type& value) {
        if (!root) {
            root = new node(value);
            nodeCount++;
            return true;
        }

        node* leaf = findLeaf(root, value);
        leaf->insertKey(value);
        nodeCount++;
        fixOverflow(leaf);
        return true;
    }

    data_type get_min() const {
        if (!root) return std::numeric_limits<data_type>::max();

        data_type minVal;
        findMinNode(root, minVal);
        return minVal;
    }

    data_type delete_min() {
        if (!root) return std::numeric_limits<data_type>::max();

        data_type minVal;
        findMinNode(root, minVal);
        node* nd = findNode(root, minVal);
        if (nd) {
            removeFromNode(nd, minVal);
            nodeCount--;
        }
        return minVal;
    }

    void reunion(TwoThree<data_type>& other) {
        std::vector<data_type> merged;
        exportHeap(merged);
        other.exportHeap(merged);

        clear();
        for (const data_type& val : merged) insert(val);

        other.clear();
    }

    void clear() {
        destroy(root);
        root = nullptr;
        nodeCount = 0;
    }

    void exportHeap(std::vector<data_type>& out) const {
        inOrder(root, out);
    }

private:
    node* findLeaf(node* nd, data_type val) const {
        if (nd->isLeaf()) return nd;

        if (val < nd->keys[0]) return findLeaf(nd->kids[0], val);
        if (nd->keys.size() == 1 || val < nd->keys[1]) return findLeaf(nd->kids[1], val);
        return findLeaf(nd->kids[2], val);
    }

    void inOrder(node* nd, std::vector<data_type>& out) const {
        if (!nd) return;

        for (size_t i = 0; i < nd->keys.size(); ++i) {
            if (i < nd->kids.size()) inOrder(nd->kids[i], out);
            out.push_back(nd->keys[i]);
        }

        if (!nd->kids.empty() && nd->kids.size() > nd->keys.size())
            inOrder(nd->kids.back(), out);
    }

    void fixOverflow(node* nd) {
        while (nd && nd->keys.size() > 2) {
            if (!nd->parent) {
                node* left = new node(nd->keys[0]);
                node* right = new node(nd->keys[2]);

                if (!nd->isLeaf() && nd->kids.size() >= 4) {
                    left->kids = { nd->kids[0], nd->kids[1] };
                    right->kids = { nd->kids[2], nd->kids[3] };

                    for (auto* c : left->kids) c->parent = left;
                    for (auto* c : right->kids) c->parent = right;
                }

                root = new node(nd->keys[1]);
                root->kids = { left, right };
                left->parent = right->parent = root;

                delete nd;
                return;
                }

            node* parent = nd->parent;
            data_type mid = nd->keys[1];

            node* left = new node(nd->keys[0]);
            node* right = new node(nd->keys[2]);

            if (!nd->isLeaf() && nd->kids.size() >= 4) {
                left->kids = { nd->kids[0], nd->kids[1] };
                right->kids = { nd->kids[2], nd->kids[3] };

                for (auto* c : left->kids) c->parent = left;
                for (auto* c : right->kids) c->parent = right;
            }

            size_t idx = 0;
            while (idx < parent->kids.size() && parent->kids[idx] != nd) idx++;

            if (idx <= parent->keys.size()) {
                parent->keys.insert(parent->keys.begin() + idx, mid);

                parent->kids.erase(parent->kids.begin() + idx);
                parent->kids.insert(parent->kids.begin() + idx, right);
                parent->kids.insert(parent->kids.begin() + idx, left);
            }

            left->parent = right->parent = parent;

            delete nd;
            nd = parent;
        }
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
	TwoThree<int> heaps[nrheaps];
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

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

template<typename data_type>
class TwoThreeHeap {
private:
    struct node {
        vector<data_type> keys;
        vector<node*> kids;
        node* parent;

        node(data_type val) : keys{val}, parent(nullptr) {}

        bool isLeaf() const {
            return kids.empty();
        }

        bool isFull() const {
            return keys.size() == 2;
        }

        void insertKey(data_type val) {
            keys.push_back(val);
            sort(keys.begin(), keys.end());
        }
    };

    node* root = nullptr;
    int nodeCount = 0;

    node* findLeaf(node* nd, data_type val) const {
        if (nd->isLeaf()) return nd;

        if (val < nd->keys[0]) return findLeaf(nd->kids[0], val);
        if (nd->keys.size() == 1 || val < nd->keys[1]) return findLeaf(nd->kids[1], val);

        return findLeaf(nd->kids[2], val);
    }

    void fixOverflow(node* nd) {
        while (nd && nd->keys.size() > 2) {
            if (!nd->parent) {
                node* left = new node(nd->keys[0]);
                node* right = new node(nd->keys[2]);

                if (!nd->isLeaf()) {
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

            if (!nd->isLeaf()) {
                left->kids = { nd->kids[0], nd->kids[1] };
                right->kids = { nd->kids[2], nd->kids[3] };

                for (auto* c : left->kids) c->parent = left;
                for (auto* c : right->kids) c->parent = right;
            }

            size_t idx = 0;
            while (parent->kids[idx] != nd) idx++;

            parent->keys.insert(parent->keys.begin() + idx, mid);
            parent->kids.erase(parent->kids.begin() + idx);
            parent->kids.insert(parent->kids.begin() + idx, right);
            parent->kids.insert(parent->kids.begin() + idx, left);

            left->parent = right->parent = parent;

            delete nd;
            nd = parent;
        }
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

    void findMinDFS(node* nd, const data_type& atLeast, data_type& res, bool& found) const {
        if (!nd) return;

        for (const data_type& k : nd->keys) {
            if (k >= atLeast && (!found || k < res)) {
                res = k;
                found = true;
            }
        }

        for (auto* kid : nd->kids) {
            findMinDFS(kid, atLeast, res, found);
        }
    }

    void findMaxDFS(node* nd, const data_type& atMost, data_type& res, bool& found) const {
        if (!nd) return;

        for (const data_type& k : nd->keys) {
            if (k <= atMost && (!found || k > res)) {
                res = k;
                found = true;
            }
        }

        for (auto* kid : nd->kids) {
            findMaxDFS(kid, atMost, res, found);
        }
    }

    void rangeDFS(node* nd, const data_type& a, const data_type& b, vector<data_type>& out) const {

        if (!nd) return;

        for (const data_type& k : nd->keys) {
            if (k >= a && k <= b) out.push_back(k);
        }

        for (auto* kid : nd->kids) rangeDFS(kid, a, b, out);
    }

    void inOrder(node* nd, vector<data_type>& out) const {

        if (!nd) return;

        for (size_t i = 0; i < nd->keys.size(); ++i) {
            if (i < nd->kids.size()) inOrder(nd->kids[i], out);
            out.push_back(nd->keys[i]);
        }

        if (!nd->kids.empty() && nd->kids.size() > nd->keys.size())
            inOrder(nd->kids.back(), out);
    }

    void destroy(node* nd) {
        if (!nd) return;

        for (auto* kid : nd->kids) destroy(kid);
        delete nd;
    }

public:
    TwoThreeHeap() = default;

    void insert(const data_type& value) {
        if (!root) {
            root = new node(value);
            nodeCount++;
            return;
        }

        node* leaf = findLeaf(root, value);
        leaf->insertKey(value);

        nodeCount++;

        fixOverflow(leaf);
    }

    void remove(const data_type& value) {
        node* nd = findNode(root, value);
        if (nd) {
            removeFromNode(nd, value);
            nodeCount--;
        }
    }

    data_type get_min(const data_type& atLeast) const {
        data_type res;
        bool found = false;

        findMinDFS(root, atLeast, res, found);
        return res;
    }

    /*data_type findmax(const data_type& atMost) const {
        data_type res;
        bool found = false;

        findMaxDFS(root, atMost, res, found);
        return res;
    }
*/
    bool get(const data_type& value) const {
        return findNode(root, value) != nullptr;
    }

    void range(const data_type& a, const data_type& b, vector<data_type>& out) const {
        rangeDFS(root, a, b, out);
    }

    void exportHeap(vector<data_type>& out) const {
        inOrder(root, out);
    }

    void clear() {
        destroy(root);
        root = nullptr;
        nodeCount = 0;
    }

    void reunion(TwoThreeHeap<data_type>& other) {
        vector<data_type> merged;

        this->exportHeap(merged);
        other.exportHeap(merged);

        sort(merged.begin(), merged.end());

        this->clear();

        for (const data_type& val : merged) insert(val);
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
	TwoThreeHeap<int> heaps[nrheaps];
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
				std::cout << "get: " << heaps[heap].get_min(3) << '\n';
				break;
			case 3:
				fin >> heap;
				std::cout << "delete: ";
				heaps[heap].remove(heaps[heap].get_min(3));
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

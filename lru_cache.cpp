#include <iostream>
#include <unordered_map>

// Node for doubly linked list (stores key-value)
struct Node {
    int key;
    int value;
    Node* prev;
    Node* next;

    Node(int k, int v) : key(k), value(v), prev(nullptr), next(nullptr) {}
};

class LRUCache {
private:
    int capacity;
    std::unordered_map<int, Node*> cacheMap;
    Node* head; // most recently used
    Node* tail; // least recently used

    void removeNode(Node* node) {
        if (!node) return;
        if (node->prev)
            node->prev->next = node->next;
        else
            head = node->next;
        if (node->next)
            node->next->prev = node->prev;
        else
            tail = node->prev;
    }

    void insertAtFront(Node* node) {
        node->prev = nullptr;
        node->next = head;
        if (head)
            head->prev = node;
        head = node;
        if (!tail)
            tail = head;
    }

public:
    LRUCache(int cap) {
        capacity = cap;
        head = nullptr;
        tail = nullptr;
    }

    int get(int key) {
        if (cacheMap.find(key) == cacheMap.end())
            return -1; // not found
        Node* node = cacheMap[key];
        removeNode(node);
        insertAtFront(node);
        return node->value;
    }

    void put(int key, int value) {
        if (cacheMap.find(key) != cacheMap.end()) {
            // Key exists, update value and move to front
            Node* node = cacheMap[key];
            node->value = value;
            removeNode(node);
            insertAtFront(node);
        } else {
            // Key doesn't exist, create new node
            Node* newNode = new Node(key, value);
            if ((int)cacheMap.size() == capacity) {
                // Remove least recently used
                Node* toDelete = tail;
                removeNode(toDelete);
                cacheMap.erase(toDelete->key);
                delete toDelete;
            }
            insertAtFront(newNode);
            cacheMap[key] = newNode;
        }
    }

    void display() {
        Node* curr = head;
        std::cout << "Cache [MRU -> LRU]: ";
        while (curr) {
            std::cout << "(" << curr->key << "," << curr->value << ") ";
            curr = curr->next;
        }
        std::cout << "\n";
    }

    ~LRUCache() {
        Node* curr = head;
        while (curr) {
            Node* nextNode = curr->next;
            delete curr;
            curr = nextNode;
        }
    }
};

// Function to run tests
void runTests() {
    std::cout << "\nRunning tests...\n";

    LRUCache cache(2);
    cache.put(1, 10);
    cache.put(2, 20);

    if (cache.get(1) != 10) {
        std::cout << "Test failed: Expected 10 for key 1\n";
    } else {
        std::cout << "Test passed: Key 1 has value 10\n";
    }

    cache.put(3, 30); // evict key 2

    if (cache.get(2) != -1) {
        std::cout << "Test failed: Expected -1 for evicted key 2\n";
    } else {
        std::cout << "Test passed: Key 2 correctly evicted\n";
    }

    if (cache.get(3) != 30) {
        std::cout << "Test failed: Expected 30 for key 3\n";
    } else {
        std::cout << "Test passed: Key 3 has value 30\n";
    }

    cache.put(4, 40); // evict key 1

    if (cache.get(1) != -1) {
        std::cout << "Test failed: Expected -1 for evicted key 1\n";
    } else {
        std::cout << "Test passed: Key 1 correctly evicted\n";
    }

    if (cache.get(4) != 40) {
        std::cout << "Test failed: Expected 40 for key 4\n";
    } else {
        std::cout << "Test passed: Key 4 has value 40\n";
    }

    std::cout << "All tests finished.\n";
}

int main() {
    int cap;
    std::cout << "Enter cache capacity: ";
    std::cin >> cap;

    LRUCache cache(cap);

    std::cout << "\nAvailable commands:\n";
    std::cout << "1 -> Put (add or update key-value)\n";
    std::cout << "2 -> Get value by key\n";
    std::cout << "3 -> Display cache\n";
    std::cout << "4 -> Exit\n";
    std::cout << "5 -> Run tests\n";

    while (true) {
        int cmd;
        std::cout << "\nEnter command number: ";
        std::cin >> cmd;

        if (cmd == 1) {
            int key, value;
            std::cout << "Enter key: ";
            std::cin >> key;
            std::cout << "Enter value: ";
            std::cin >> value;
            cache.put(key, value);
            std::cout << "Added/Updated (" << key << ", " << value << ").";
        } else if (cmd == 2) {
            int key;
            std::cout << "Enter key to get: ";
            std::cin >> key;
            int val = cache.get(key);
            if (val == -1) {
                std::cout << "Key not found.";
            } else {
                std::cout << "Value: " << val;
            }
        } else if (cmd == 3) {
            cache.display();
        } else if (cmd == 4) {
            std::cout << "Bye!\n";
            break;
        } else if (cmd == 5) {
            runTests();
        } else {
            std::cout << "Invalid command. Please enter 1, 2, 3, 4, or 5.";
        }
    }

    return 0;
}

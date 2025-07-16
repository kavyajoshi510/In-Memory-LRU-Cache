#include <iostream>
#include <unordered_map>
#include <mutex>
#include <chrono>
#include <thread>

using namespace std::chrono;

// Node for the doubly linked list
struct Node {
    int key;
    int value;
    Node* prev;
    Node* next;
    steady_clock::time_point expiry; // Expiration time for TTL

    Node(int k, int v, steady_clock::time_point exp) 
        : key(k), value(v), prev(nullptr), next(nullptr), expiry(exp) {}
};

class LRUCache {
private:
    int capacity;
    std::unordered_map<int, Node*> cacheMap;
    Node* head; // Most recently used
    Node* tail; // Least recently used
    std::mutex mtx;

    int hits;   // Count of cache hits
    int misses; // Count of cache misses

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

    bool isExpired(Node* node) {
        return node->expiry != steady_clock::time_point() &&
               steady_clock::now() > node->expiry;
    }

public:
    LRUCache(int cap) : capacity(cap), head(nullptr), tail(nullptr), hits(0), misses(0) {}

    int get(int key) {
        std::lock_guard<std::mutex> lock(mtx);

        if (cacheMap.find(key) == cacheMap.end()) {
            misses++;
            return -1;
        }

        Node* node = cacheMap[key];

        if (isExpired(node)) {
            removeNode(node);
            cacheMap.erase(key);
            delete node;
            misses++;
            return -1;
        }

        removeNode(node);
        insertAtFront(node);
        hits++;
        return node->value;
    }

    void put(int key, int value, int ttl_seconds = 0) {
        std::lock_guard<std::mutex> lock(mtx);

        if (cacheMap.find(key) != cacheMap.end()) {
            Node* node = cacheMap[key];
            node->value = value;
            node->expiry = ttl_seconds > 0 ? steady_clock::now() + seconds(ttl_seconds) : steady_clock::time_point();
            removeNode(node);
            insertAtFront(node);
        } else {
            Node* newNode = new Node(key, value, ttl_seconds > 0 ? steady_clock::now() + seconds(ttl_seconds) : steady_clock::time_point());

            if ((int)cacheMap.size() == capacity) {
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
        std::lock_guard<std::mutex> lock(mtx);
        Node* curr = head;
        std::cout << "Cache [MRU -> LRU]: ";
        while (curr) {
            std::cout << "(" << curr->key << "," << curr->value << ") ";
            curr = curr->next;
        }
        std::cout << "\n";
    }

    void showStats() {
        std::lock_guard<std::mutex> lock(mtx);
        std::cout << "Cache Hits: " << hits << ", Misses: " << misses << "\n";
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

void runTests() {
    std::cout << "\nRunning automated tests...\n";
    LRUCache cache(2);

    // Test 1: Insert and get
    cache.put(1, 10);
    cache.put(2, 20);

    if (cache.get(1) != 10) {
        std::cout << "Test 1 failed: Expected 10 for key 1\n";
    } else {
        std::cout << "Test 1 passed: Key 1 has value 10\n";
    }

    // Test 2: LRU eviction
    cache.put(3, 30); // should evict key 2

    if (cache.get(2) != -1) {
        std::cout << "Test 2 failed: Expected -1 for evicted key 2\n";
    } else {
        std::cout << "Test 2 passed: Key 2 correctly evicted\n";
    }

    // Test 3: TTL expiration
    cache.put(4, 40, 1); // TTL 1 second
    std::this_thread::sleep_for(std::chrono::seconds(2));

    if (cache.get(4) != -1) {
        std::cout << "Test 3 failed: Expected -1 for expired key 4\n";
    } else {
        std::cout << "Test 3 passed: Key 4 expired as expected\n";
    }

    // Test 4: Eviction after new puts
    cache.put(5, 50);
    cache.put(6, 60); // evicts key 1

    if (cache.get(1) != -1) {
        std::cout << "Test 4 failed: Expected -1 for evicted key 1\n";
    } else {
        std::cout << "Test 4 passed: Key 1 correctly evicted\n";
    }

    // Test 5: Show stats
    cache.showStats();

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
    std::cout << "4 -> Show stats\n";
    std::cout << "5 -> Run tests\n";
    std::cout << "6 -> Exit\n";

    while (true) {
        int cmd;
        std::cout << "\nEnter command number: ";
        std::cin >> cmd;

        if (cmd == 1) {
            int key, value, ttl;
            std::cout << "Enter key: ";
            std::cin >> key;
            std::cout << "Enter value: ";
            std::cin >> value;
            std::cout << "Enter TTL in seconds (0 for no expiry): ";
            std::cin >> ttl;

            cache.put(key, value, ttl);
            std::cout << "Added/Updated (" << key << ", " << value << ") with TTL " << ttl << " seconds.\n";

        } else if (cmd == 2) {
            int key;
            std::cout << "Enter key to get: ";
            std::cin >> key;
            int val = cache.get(key);
            if (val == -1) {
                std::cout << "Key not found or expired.\n";
            } else {
                std::cout << "Value: " << val << "\n";
            }

        } else if (cmd == 3) {
            cache.display();

        } else if (cmd == 4) {
            cache.showStats();

        } else if (cmd == 5) {
            runTests();

        } else if (cmd == 6) {
            std::cout << "Exiting. Goodbye!\n";
            break;

        } else {
            std::cout << "Invalid command. Please enter 1 to 6.\n";
        }
    }

    return 0;
}


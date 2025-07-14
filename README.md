# In-Memory LRU Cache (C++)

This project is a simple and easy-to-understand implementation of a Least Recently Used (LRU) cache in C++. It uses a doubly linked list and an unordered map to support **O(1)** time for both `get` and `put` operations.

When the cache reaches its capacity, it automatically removes (evicts) the least recently used item to make space for new ones. This is helpful when you have limited memory and want to keep only the most recently accessed data.

---

## How it works

- The cache stores key-value pairs in a doubly linked list to track which items were used recently.
- The most recently used (MRU) item is always at the front of the list, and the least recently used (LRU) item is at the end.
- An unordered map allows us to quickly find and update items in constant time.
- When you access or add a key, it moves to the front to mark it as most recently used.

---

## How to compile and run

```bash
g++ -std=c++11 lru_cache.cpp -o lru_cache
./lru_cache

```

## Testing

You can run built-in tests to check if everything works as expected.  
When you run the program, just choose **Option 5** (Run tests).

This will automatically test `put`, `get`, and eviction logic, and show if all cases pass or if something is wrong.



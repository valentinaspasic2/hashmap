#pragma once

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

using namespace std;

// allow the HashMap to use different key and value types
template <typename KeyT, typename ValT>

class HashMap {
 private:
 // store one key value pair in a bucket's linked list
  struct ChainNode {
    const KeyT key;
    ValT value;
    ChainNode* next;

    // node with no next node
    ChainNode(KeyT key, ValT value) : key(key), value(value), next(nullptr) {
    }

    // node that points to an existing next node
    ChainNode(KeyT key, ValT value, ChainNode* next)
        : key(key), value(value), next(next) {
    }
  };

  // array of bucket heads
  ChainNode** data;

  size_t sz;
  size_t capacity;

  // used to keep track of position
  ChainNode* curr;
  size_t curr_idx;

 public:
  // creates empty HashMap with 10 buckets
  HashMap() {
    capacity = 10;
    sz = 0;

    // create array of empty buckets
    data = new ChainNode*[capacity];

    // set all buckets to null
    for (size_t i = 0; i < capacity; i++) {
      data[i] = nullptr;
    }

    // iteration start
    curr = nullptr;

    // index start
    curr_idx = 0;
  }

  // creates an empty HashMap with `capacity` buckets.
  HashMap(size_t capacity) {
    // set bucket count from parameter
    this->capacity = capacity;

    // start empty
    sz = 0;

    // make array of pointers
    data = new ChainNode*[this->capacity];

    // set all buckets to null
    for (size_t i = 0; i < this->capacity; i++) {
      data[i] = nullptr;
    }

    // iteration start
    curr = nullptr;

    // index start
    curr_idx = 0;
  }

  // checks if the `HashMap` is empty. Runs in O(1)
  bool empty() const {
    return sz == 0;
  }

  // Returns the number of mappings in the `HashMap`. Runs in O(1)
  size_t size() const {
    return sz;
  }

  // adds a key-value pair and resizes the table when needed
  void insert(KeyT key, ValT value) {
    // find bucket for this key
    size_t idx = hash<KeyT>{}(key) % capacity;


    ChainNode* curr = data[idx];

    // check if key already exists
    while (curr != nullptr) {
      // if key found, do nothing
      if (curr->key == key) {
        return;
      }

      
      curr = curr->next;
    }

    // create new node at front
    ChainNode* newNode = new ChainNode(key, value, data[idx]);

    // update head and size
    data[idx] = newNode;
    sz++;

    // check load factor
    double load = (double)sz / (double)capacity;

    // if too full then resize
    if (load > 1.5) {

      // save old data and capacity
      ChainNode** oldData = data;
      size_t oldCap = capacity;

      // new capacity and array
      capacity *= 2;
      data = new ChainNode*[capacity];

      // set all to null
      for (size_t i = 0; i < capacity; i++) {
        data[i] = nullptr;
      }

      // rehash all nodes
      for (size_t i = 0; i < oldCap; i++) {

        // go through each chain
        ChainNode* node = oldData[i];

        while (node != nullptr) {
          // save next node
          ChainNode* nextNode = node->next;

          // get new index
          size_t newIdx = hash<KeyT>{}(node->key) % capacity;

          // insert at front of new chain
          node->next = data[newIdx];
          data[newIdx] = node;

          // move forward
          node = nextNode;
        }
      }

      // delete old array
      delete[] oldData;
    }
  }

  // returns the value for a given key
  ValT& at(const KeyT& key) const {
    // find the bucket for this key
    size_t idx = hash<KeyT>{}(key) % capacity;

    ChainNode* curr = data[idx];

    // search chain for the key
    while (curr != nullptr) {
      // if key matches
      if (curr->key == key) {
        return curr->value;
      }

      // move to next node
      curr = curr->next;
    }

    // key not found -> throw
    throw out_of_range("key not found");
  }

  // checks if a key exists in the map
  bool contains(const KeyT& key) const {
    // find bucket for this key
    size_t idx = hash<KeyT>{}(key) % capacity;

    ChainNode* curr = data[idx];

    // search chain for the key
    while (curr != nullptr) {
      // if key matches
      if (curr->key == key) {
        return true;
      }

      // move to next node
      curr = curr->next;
    }

    // not found
    return false;
  }

  // removes all key value pairs and frees their memory
  void clear() {
    // delete every node in each bucket
    for (size_t i = 0; i < capacity; i++) {
      ChainNode* curr = data[i];

      // delete all nodes
      while (curr != nullptr) {
        // save next node
        ChainNode* nextNode = curr->next;

        // delete current node & go next
        delete curr;
        curr = nextNode;
      }

      data[i] = nullptr;
    }

    // reset
    sz = 0;
    curr = nullptr;
    curr_idx = 0;
  }

  // frees all memory used by the HashMap
  ~HashMap() {
    clear();
    delete[] data;
  }

  // removes a key value pair and returns its value
  ValT erase(const KeyT& key) {
    size_t idx = hash<KeyT>{}(key) % capacity;
    ChainNode* curr = data[idx];
    ChainNode* prev = nullptr;

    // search the chain for the key
    while (curr != nullptr) {
      // if found
      if (curr->key == key) {
        ValT val = curr->value;

        // reconnect the chain around the removed node
        if (prev == nullptr) {
          data[idx] = curr->next;
        } else {
          prev->next = curr->next;
        }
        delete curr;
        sz--;
        return val;
      }

      prev = curr;
      curr = curr->next;
    }

    // key not found
    throw out_of_range("key not found");
  }

  // creates an independent copy of another HashMap
  HashMap(const HashMap& other) {
    capacity = other.capacity;
    sz = 0;

    // create array of empty buckets
    data = new ChainNode*[capacity];
    for (size_t i = 0; i < capacity; i++) {
      data[i] = nullptr;
    }

    // reset iterator
    curr = nullptr;
    curr_idx = 0;

    // copy all key value pairs from the other map
    for (size_t i = 0; i < other.capacity; i++) {
      ChainNode* node = other.data[i];

      // go through chain
      while (node != nullptr) {
        // insert key/value
        insert(node->key, node->value);

        node = node->next;
      }
    }
  }

  // replaces this HashMap with a copy of another HashMap
  HashMap& operator=(const HashMap& other) {
    // avoid copying the map onto itself
    if (this == &other) {
      return *this;
    }

    clear();
    delete[] data;

    capacity = other.capacity;
    sz = 0;

    // create array of empty buckets
    data = new ChainNode*[capacity];
    for (size_t i = 0; i < capacity; i++) {
      data[i] = nullptr;
    }

    // copy all key value pairs from the other map
    for (size_t i = 0; i < other.capacity; i++) {
      ChainNode* node = other.data[i];

      while (node != nullptr) {
        // insert key/value
        insert(node->key, node->value);

        // move forward
        node = node->next;
      }
    }

    // return this object
    return *this;
  }

  // checks if two HashMaps contain the same key value pairs
  bool operator==(const HashMap& other) const {
    // sizes must match
    if (sz != other.sz) {
      return false;
    }

    // compare each key value pair
    for (size_t i = 0; i < capacity; i++) {
      ChainNode* node = data[i];

      // go through chain
      while (node != nullptr) {
        // if no key
        if (!other.contains(node->key)) {
          return false;
        }

        // if value doesn't match
        if (other.at(node->key) != node->value) {
          return false;
        }

        // move forward
        node = node->next;
      }
    }

    // all match
    return true;
  }

  // prepares iteration starting at the first non empty bucket
  void begin() {
    curr = nullptr;
    curr_idx = 0;

    // find first non empty bucket
    while (curr_idx < capacity && data[curr_idx] == nullptr) {
      curr_idx++;
    }

    // set curr to first node
    if (curr_idx < capacity) {
      curr = data[curr_idx];
    }
  }

  // returns the current key value pair and moves to the next one
  bool next(KeyT& key, ValT& value) {
    // if no more elements
    if (curr == nullptr) {
      return false;
    }

    // set output values
    key = curr->key;
    value = curr->value;

    // continue through the current chain
    if (curr->next != nullptr) {
      curr = curr->next;
      return true;
    }

    // find next non empty bucket
    curr_idx++;
    while (curr_idx < capacity && data[curr_idx] == nullptr) {
      curr_idx++;
    }

    // if found new bucket
    if (curr_idx < capacity) {
      curr = data[curr_idx];
    } else {
      // no more elements
      curr = nullptr;
    }

    return true;
  }
};

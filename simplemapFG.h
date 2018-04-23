// Spring 2018
//
// Description: This file specifies a simple map implemented using two vectors.
// << use templates for creating a map of generic types >>
// One vector is used for storing keys. One vector is used for storing values.
// The expectation is that items in equal positions in the two vectors correlate.
// That is, for all values of i, keys[i] is the key for the item in values[i].
// Keys in the map are not necessarily ordered.

#include <vector>
#include <cassert>
#include <algorithm>
//#include <mutex>
//#insert "tests.h"

#define MAXTHREADS 8
#define TOTAL 1000
#define ACCNTS 1000


template <class K, class V>
class simplemap_t {

  // Define the two vectors of types K and V
  std::vector<K> *keys;
  std::vector<V> *values;

public:
  
  // The constructor should just initialize the vectors to be empty
  simplemap_t() {
    keys = new std::vector<K>();
    values = new std::vector<V>();
  }

  bool test_and_set(bool ref_lock){
    bool initial = ref_lock;
    ref_lock = true;
    return initial;
  }

 
  // Insert (key, val) if and only if the key is not currently present in
  // the map.  Returns true on success, false if the key was
  // already present.
  bool insert(K key, V val) {
    
    typename std::vector<K>::iterator it;
    it = find(keys->begin(), keys->end(), key);
    if(it != keys->end()) {
      return false; //key was found; no insertion
    }else{
      keys->push_back(key);
      values->push_back(val);
      return true; //key was inserted
    }
  }

  // If key is present in the data structure, replace its value with val
  // and return true; if key is not present in the data structure, return
  // false.
  bool update(K key, V val) {

    typename std::vector<K>::iterator itK;
    itK = find(keys->begin(), keys->end(), key);
    int i = itK - keys->begin(); //retrieving key index
    if(itK != keys->end()) {
      values->at(i) = val; //replacing value
      return true;
    }else{
      return false;
    }
  }
  
  // Remove the (key, val) pair if it is present in the data structure.
  // Returns true on success, false if the key was not already present.
  bool remove(K key) {
    for (auto i = keys->begin(), j = values->begin(); i != keys->end(), j != values->end(); ++i, ++j) {
      //use lookup bool to check if key pair is present
      if(*i == key) {
        keys->erase(i);
        values->erase(j);
	return true; //successfully deleted
      }else{
	return false; //key was not found
      }
    }
  }
  
  // If key is present in the map, return a pair consisting of
  // the corresponding value and true. Otherwise, return a pair with the
  // boolean entry set to false.
  // Somethig to think about: do you want to share the memory of the map with the application thread?
  // ...you might get unexpected behaviors.
  std::pair<V, bool> lookup(K key) {
    std::pair<V, bool> temp;
    for (auto i = keys->begin(); i != keys->end(); ++i) {
      if(*i == key) {
	temp = std::make_pair(values->at(*i), true);
	return temp;
      }else{
	temp = std::make_pair(0, false);
      }
    }
    //releaseLock(m_lock);
    //release locks
  
    return temp;
  }
  
  //No locks because this function is not invoked multithreading
  // Apply a function to each key in the map
  void apply(void (*f)(K, V)) {
    for (auto i : *keys) {
      f(i, values->at(i));
    }
  }
};

// Spring 2018
//
// Description: This file specifies a simple map implemented using two vectors.
// << use templates for creating a map of generic types >>
// One vector is used for storing keys. One vector is used for storing values.
// The expectation is that items in equal positions in the two vectors correlate.
// That is, for all values of i, keys[i] is the key for the item in values[i].
// Keys in the map are not necessarily ordered.
//
// The specification for each function appears as comments.
// Students are responsible for implementing the simple map as specified.

#include <vector>
#include <cassert>
#include <algorithm>
#include <mutex>

#define MAXTHREADS 16
#define TOTAL 1000
#define ACCNTS 1000

template <class K, class V>
class simplemap_t {
  
  // Define the two vectors of types K and V
  // << use std::vector<K> >>
  std::vector<K> *keys;
  std::vector<V> *values;
  

  public:

  // The constructor should just initialize the vectors to be empty
  simplemap_t() {
    keys = new std::vector<K>();
    values = new std::vector<V>();
  }

  void acquireLock(){
    while(mutex.try_lock());
  }

  void releaseLock(){
    mutex.unlock();
  }
  
  // Insert (key, val) if and only if the key is not currently present in
  // the map.  Returns true on success, false if the key was
  // already present.
  bool insert(K key, V val) {
    //mutex.lock();
    acquireLock();
    typename std::vector<K>::iterator it;
    it = find(keys->begin(), keys->end(), key);
    if(it != keys->end()) {
      //mutex.unlock();
      releaseLock();
      return false; //key was found; no insertion
    }else{
      keys->push_back(key);
      values->push_back(val);
      releaseLock();
      //mutex.unlock();
      return true; //key was inserted
    }
    
  }
  
  // If key is present in the data structure, replace its value with val
  // and return true; if key is not present in the data structure, return
  // false.
  bool update(K key, V val) {
    //mutex.lock();
    acquireLock();
    typename std::vector<K>::iterator itK;
    itK = find(keys->begin(), keys->end(), key);
    int i = itK - keys->begin(); //retrieving key index
    if(itK != keys->end()) {
        values->at(i) = val; //replacing value
	//mutex.unlock();
	releaseLock();
	return true;
    }else{
      //mutex.unlock();
      releaseLock();
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
    //assert("Not Implemented");
    //TO DO: the following is a default return value, do not use it!
    //return std::make_pair(0, false);
    
    //printf("%d\n", key);
    //mutex.lock();
    acquireLock();
    std::pair<V, bool> temp;
    for (auto i = keys->begin(); i != keys->end(); ++i) {
      if(*i == key) {
	temp = std::make_pair(values->at(*i), true);
	//mutex.unlock();
	releaseLock();
	return temp;
      }else{
	temp = std::make_pair(0, false);
      }
    }
    //mutex.unlock();
    releaseLock();
    return temp;
  }
  
  // Apply a function to each key in the map
  void apply(void (*f)(K, V)) {
    for (auto i : *keys) {
      f(i, values->at(i));
    }
  }
};

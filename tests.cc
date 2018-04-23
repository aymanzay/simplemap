// Spring 2018
//
// Description: This file implements a function 'run_custom_tests' that should be able to use
// the configuration information to drive tests that evaluate the correctness
// and performance of the map_t object.

#include <iostream>
#include <ctime>
#include <stdlib.h>
#include <thread>
#include "config_t.h"
#include "tests.h"
#include <mutex>
#include <chrono>
#include "simplemapFG.h"
//#include "simplemap.h"

void printer(int k, float v) {
  std::cout<<"<"<<k<<","<<v<<">"<< std::endl;
}

std::mutex locks[ACCNTS]; //FG

std::mutex mtx; //CG
// Step 1
// Define a simplemap_t of types <int,float>
// this map represents a collection of bank accounts:
// each account has a unique ID of type int;
// each account has an amount of fund of type float.
simplemap_t<int,float> map = simplemap_t<int,float>();        // map for bank accounts

void release2Locks(int index1, int index2) {
  locks[index1].unlock();
  locks[index2].unlock();
}

void acquire2Locks(int index1, int index2){

  while(true){
    while(!locks[index1].try_lock());
    //lock acquired
    //std::cout << "LOCK 1 ACQUIRED" << std::endl;
    if(locks[index2].try_lock()) {
      //lock 2 acquired
      break;
    }else{
      locks[index1].unlock();
      //lock 1 released
      continue;
    }
  }
}

void acquireAll(){
  for(int i = 0; i < ACCNTS; i++){
    while(!locks[i].try_lock());
  }
  
}

void releaseAll(){
  for(int i = 0; i < ACCNTS; i++){
    locks[i].unlock();
  }
}

//void releaseLocks(std::vector<m_lock> locks, int index) {}

// Step 3
// Define a function "deposit" that selects two random bank accounts
// and an amount. This amount is subtracted from the amount
// of the first account and summed to the amount of the second
// account. In practice, give two accounts B1 and B2, and a value V,
// the function performs B1-=V and B2+=V.
void deposit(float val) {
  //CG Lock
  //mtx.lock(); 
  int rando = (int) ACCNTS;
  
  int r1 = rand() % rando;
  int r2 = rand() % rando;
  while(r1 == r2){
    r2 = rand() % rando; //ensure bank accounts are not the same
  }
  //std::cout << "acquiring" << std::endl;

  //FG LOCK
  acquire2Locks(r1, r2);
  
  std::pair<float, bool> B1 = map.lookup(r1);
  std::pair<float, bool> B2 = map.lookup(r2);
  
  float newVal1 = B1.first;
  float newVal2 = B2.first;
  
  newVal1 -= val;
  newVal2 += val;
  
  map.update(r1, newVal1);
  map.update(r2, newVal2);

  //FG LOCK
  release2Locks(r1, r2);

  //CG Lock
  //mtx.unlock();
}

// Step 4
// Define a function "balance" that sums the amount of all the
// bank accounts in the map.
float balance() {
  //CG Lock
  //mtx.lock();

  //FG LOCK
  acquireAll();
  float total = 0;
  for(int i = 0; i < ACCNTS; i++) {
    std::pair<float, bool> foo = map.lookup(i);
    total += std::get<0>(foo);
  }

  //FG LOCK
  releaseAll();
  //std::cout << "total is " << total << "\n"  << std::endl;

  //CG Lock
  //mtx.unlock();
  return total;
}

// Step 5
// Define a function 'do_work', which has a for-loop that
// iterates for config_t.iters times. Each iteration, the function should
// call the function 'deposit' with 80% of the probability;
// otherwise (the rest 20%) the function 'balance' should be called.
// The function 'do_work' should measure 'exec_time_i', which is the
// time needed to perform the entire for-loop.
// The main thread should be able to access this amount of time once the
// thread executing the 'do_work' joins its execution with the main thread.
void do_work(int x) {

  float val = 0;
  int doBal = (int)(x*(0.2));
  for(int i = 0; i < x; i++) {
    val = 20;
    if(i % doBal == 0) {
      balance();
    }else{
      deposit(val);
    }
  }

}

void run_custom_tests(config_t& cfg) {
  
  // Step 2
  // Populate the entire map with the 'insert' function
  // Initialize the map in a way the sum of the amounts of
  // all the accounts in the map is 1000
  float cash = (float)TOTAL/ACCNTS;
  for(int i = 0; i < ACCNTS; i++) {
    map.insert(i, cash);
  }

  
  
  // Step 6
  // The evaluation should be performed in the following way:
  // - the main thread creates one thread
  //   << use std:threds >>
  // - the spawned thread executes the function 'do_work' until completion
  // - the (main) thread waits for the spawned thread to be executed
  //   << use std::thread::join() >>
  //	 and collects the 'exec_time_i' from the spawned thread
  // - once the spawned thread has joined, the function "balance" must be called
  auto start = std::chrono::high_resolution_clock::now();
  std::thread t[MAXTHREADS];
  for (int i = 0; i < MAXTHREADS; i++) {
    t[i] = std::thread(do_work, cfg.iters);
  }
  
  std::cout << "Launched from the main\n";
  
  //Join the threads with the main thread
  for (int i = 0; i < MAXTHREADS; ++i) {
    t[i].join();
  }
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> diff = end-start;
  std::cout << diff.count() << " s\n";
  
  std::cout << "total is " << balance() << std::endl;
 
  // Step 7
  // Remove all the items in the map by leveraging the 'remove' function of the map
  // Execution terminates.
  // If you reach this stage happy, then you did a good job!

  for(int i = 0; i < ACCNTS; i++) {
    map.remove(i);
  }
  
  // You might need the following function to print the entire map.
  // Attention if you use it while multiple threads are operating
  map.apply(printer);
}

void test_driver(config_t &cfg) {
	run_custom_tests(cfg);
}

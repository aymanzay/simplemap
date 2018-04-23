// Spring 2018
//
// Description:  This file implements the config_t::dump() function

#include <iostream>
#include "config_t.h"

// Print the values of the seed, iters, and name fields as comma separated
// values, with a header row
void config_t::dump() {
    std::cout << "# name, seed, key_max, iters" << std::endl;
    std::cout << name << ", "
              << key_max << ", "
              << iters << std::endl;
}

/*
 * Provides random numbers
 */

#ifndef RANDOM_H
#define RANDOM_H

#include "../../../gen/modules_enabled.h"

#ifndef ENABLE_MODULE_RANDOM
#error "Module RANDOM included but not enabled!"
#endif

extern int rnd_seed;

int rnd();

int rnd_range(int low, int high);

#endif
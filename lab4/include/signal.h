#pragma once

#include "sched.h"

typedef void (*sig_t)(int);

/* TODO */
#define SIGKILL    1

/* sig_t signal(int sig, sig_t func) ; */

void send_signal(int pid, int code);

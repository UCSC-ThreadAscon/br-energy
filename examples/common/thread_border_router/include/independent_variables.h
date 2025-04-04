/**
 * Prints out the independent variables of the experiment that the device
 * is currently running.
 *
 * Throughput Independent Variables:
 *  - Cipher Suite (4)
 *  - TX Power (3)
 *  - Confirmable/Non-Confirmable (2)
 *
 * Packet Loss Independent Variables:
 *  - Cipher Suite (4)
 *  - TX Power (3)
 *  - Confirmable/Non-Confirmable (2)
 *
 * Delay Independent Variables:
 *  - Cipher Suite (4)
 *  - TX Power (3)
 */
#pragma once

#include "utilities.h"

#include <sdkconfig.h>
#include <openthread/logging.h>

#define CIPHER_SUITE CONFIG_THREAD_ASCON_CIPHER_SUITE
#define AES 0
#define NONE 1
#define LIBASCON_128A 4
#define LIBASCON_128 5

void printCipherSuite();
void printTxPower();
void printNetworkKey();
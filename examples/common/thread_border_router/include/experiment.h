#ifndef EXPERIMENT_H
#define EXPERIMENT_H

/**
 * This part of the header defines the constants to be used in the experiments,
 * in accordance to the Experimental Setup document:
 * https://docs.google.com/document/d/15ahJuklkLitBmBDbUqnAVBdkny0B3X3-V_8ODYbE8xw/edit?usp=sharing
*/
#define URI "resource"

/**
 * Payload size for both Event and Batty Lifetime packets are the same.
 * 128 bits (16 bytes) for the UUID, and 8 bits to either represent
 * the battery lifetime of the packet (uin8t_t) or a boolean on whether
 * an even has occured.
*/
#define PAYLOAD_SIZE 17

#endif // EXPERIMENT_H
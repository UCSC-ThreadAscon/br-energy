#pragma once

#define BATTERY_URI "battery"
#define EVENT_URI "event"

/**
 * Payload size for both Event and Batty Lifetime packets are the same.
 * 128 bits (16 bytes) for the UUID, and 8 bits to either represent
 * the battery lifetime of the packet (uin8t_t) or a boolean on whether
 * an even has occured.
*/
#define PAYLOAD_SIZE 17
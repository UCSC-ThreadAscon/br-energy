#include "experiment.h"

#include "esp_err.h"
#include "esp_log.h"
#include "esp_openthread.h"
#include "esp_openthread_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "openthread/cli.h"
#include "openthread/instance.h"
#include "openthread/logging.h"
#include "openthread/tasklet.h"
#include "openthread/coap_secure.h"

#include <stdint.h>
#include <inttypes.h>

/**
 * The statement to print when a CoAP request is received is as follows:
 * 
 *    "Received [uint32_t] bytes from [IPv6 address].".
 *
 * The substring "Received " is 9 bytes.
 * The substring " bytes from " is 12 bytes.
 * The substring "." is 1 byte.
 *
 * The 32 bit integer will be represented as a string.
 * 2^32 - 1 = 4294967295 has 10 digits, and thus, the
 * string representation of the uint32_t will make up 10 bytes.
 *
 * Furthermore, an IPv6 string representation is made up of 16 bytes.
 *
 * Thus, the total string size is  9 + 12 + 1 + 10 + 40 = 72 bytes.
*/
#define PRINT_STATEMENT_SIZE 72

#define OT_INSTANCE esp_openthread_get_instance()

/**
 * Empties all memory for `size` bytes starting at memory address `pointer`.
 *
 * @param[in] pointer: the pointer of the stack memory
 * @param[in] size:    the size of the memory that `pointer` points to
 *
 * I got the idea to use `memset()` to clear stack memory from
 * the Google Search AI:
 * https://docs.google.com/document/d/1o-NaEOA-vzWPCv7VX1dONUfwos2epveDk4H_Y2Y5g1Y/edit?usp=sharing
*/
static inline void EmptyMemory(void* pointer, size_t size) {
  memset(pointer, 0, size);
  return;
}

#define OT_DISCONNECTED(role) (role == OT_DEVICE_ROLE_DISABLED) || (role == OT_DEVICE_ROLE_DETACHED)

#define MS_TO_TICKS(ms) ms / portTICK_PERIOD_MS
#define MS_TO_MICRO(ms) ms * 1000

#define CONNECTION_WAIT_TIME_MS MS_TO_TICKS(100)
#define MAIN_WAIT_TIME MS_TO_TICKS(5000) // 5 seconds

void checkConnection(otInstance *aInstance);
void handleError(otError error, char* desc);

#define HandleMessageError(desc, aMessage, error)       \
  if (error != OT_ERROR_NONE) {                         \
    otMessageFree(aMessage);                            \
    handleError(error, desc);                           \
    return;                                             \
  }                                                     \

#define COAP_SOCK_PORT OT_DEFAULT_COAP_PORT

/** ---- CoAP CLI API ---- */
otError expServerStart(void* aContext, uint8_t argsLength, char* aArgs[]) ;
otError expServerFree(void* aContext, uint8_t argsLength, char* aArgs[]);

/** ---- CoAP Common API ---- */
uint16_t getPayloadLength(const otMessage *aMessage);
void getPayload(const otMessage *aMessage, void* buffer);

/* ---- CoAP Server API ---- */
otError createResource(otCoapResource *resource, Route route);
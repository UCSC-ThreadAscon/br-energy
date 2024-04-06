#include "ot_send.h"

#include <openthread/thread.h>
#include <openthread/platform/radio.h>

#include <assert.h>

#define UnhandledError()                                        \
    otLogCritPlat("Cannot Handle Error: %" PRIu8 ",", error)    \

void checkConnection(otInstance *aInstance) {
  otDeviceRole currentRole;
  do {
    currentRole = otThreadGetDeviceRole(aInstance);
    vTaskDelay(DEFAULT_WAIT_TIME_MS);
  }
  while(OT_DISCONNECTED(currentRole));

  otLogNotePlat("OpenThread Connection has been established.");
  return;
}
#include "workload.h"

#include <openthread/thread.h>
#include <openthread/platform/radio.h>

#include <assert.h>

void handleError(otError error, char* desc)
{
  if (error != OT_ERROR_NONE) {
    otLogCritPlat("%s error: %s", desc, otThreadErrorToString(error));
  }
  return;
}

void checkConnection(otInstance *aInstance) {
  otDeviceRole currentRole;
  do {
    currentRole = otThreadGetDeviceRole(aInstance);
    vTaskDelay(CONNECTION_WAIT_TIME_MS);
  }
  while(OT_DISCONNECTED(currentRole));

  otLogNotePlat("OpenThread Connection has been established.");
  return;
}

uint16_t getPayloadLength(const otMessage *aMessage) {
  return otMessageGetLength(aMessage) - otMessageGetOffset(aMessage);
}

/**
 * Based upon the OpenThread CLI UDP Handler code.
 * https://github.com/openthread/openthread/blob/main/src/cli/cli_udp.cpp#L461
 *
 * According to the handler code, `otMessageGetOffset()` returns the end
 * of the Message header, while `otMessageGetLength()` returns the size
 * of the Message header, payload included.
*/
void getPayload(const otMessage *aMessage, void* buffer) {
  uint16_t offset = otMessageGetOffset(aMessage);
  uint16_t length = getPayloadLength(aMessage);

  uint16_t bytesRead = otMessageRead(aMessage, offset, buffer, length);
  assert(bytesRead == length);
  return;
}

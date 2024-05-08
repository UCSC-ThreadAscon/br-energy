#include "workload.h"

static otCoapResource *battery;
static otCoapResource *event;

void startCoapServer(uint16_t port) {
  otError error = otCoapStart(OT_INSTANCE, port);

  if (error != OT_ERROR_NONE) {
    otLogCritPlat("Failed to start COAP server.");
  } else {
    otLogNotePlat("Started CoAP server at port %d.", port);
  }
  return;
}

otError expServerStart(void* aContext, uint8_t argsLength, char* aArgs[]) 
{
  checkConnection(OT_INSTANCE);
  startCoapServer(OT_DEFAULT_COAP_PORT);

  battery = calloc(1, sizeof(otCoapResource));
  event = calloc(1, sizeof(otCoapResource));

  createResource(battery, Battery);
  createResource(event, Event);

  otCoapAddResource(OT_INSTANCE, battery);
  otCoapAddResource(OT_INSTANCE, event);

  otLogNotePlat("Set up battery URI: '%s'.", battery->mUriPath);
  otLogNotePlat("Set up event URI: '%s'.", event->mUriPath);

  return OT_ERROR_NONE;
}

otError expServerFree(void* aContext, uint8_t argsLength, char* aArgs[])
{
  otCoapRemoveResource(OT_INSTANCE, battery);
  otCoapRemoveResource(OT_INSTANCE, event);

  otCoapStop(OT_INSTANCE);

  free(battery);
  free(event);

  return OT_ERROR_NONE;
}
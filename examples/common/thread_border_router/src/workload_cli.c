#include "workload.h"

static otCoapResource *resource;

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

  resource = calloc(1, sizeof(otCoapResource));
  createResource(resource);
  otCoapAddResource(OT_INSTANCE, resource);
  otLogNotePlat("Set up resource URI: '%s'.", resource->mUriPath);

  return OT_ERROR_NONE;
}

otError expServerFree(void* aContext, uint8_t argsLength, char* aArgs[])
{
  otCoapRemoveResource(OT_INSTANCE, resource);
  otCoapStop(OT_INSTANCE);
  free(resource);
  return OT_ERROR_NONE;
}
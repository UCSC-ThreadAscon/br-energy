#include "workload.h"

static otCoapResource *aPeriodicResource;
static otCoapResource *periodicResource;

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

  /**
   * Allocate HEAP Memory for resource that receive packets from Scenario 1.
  */
  aPeriodicResource = calloc(1, sizeof(otCoapResource));
  createAPeriodicResource(aPeriodicResource);
  otCoapAddResource(OT_INSTANCE, aPeriodicResource);
  otLogNotePlat("Set up resource URI: '%s'.", aPeriodicResource->mUriPath);

  /**
   * Allocate HEAP Memory for resource that receive packets from Scenario 2.
  */
  periodicResource = calloc(1, sizeof(otCoapResource));
  createPeriodicResource(periodicResource);
  otCoapAddResource(OT_INSTANCE, periodicResource);
  otLogNotePlat("Set up resource URI: '%s'.", periodicResource->mUriPath);

  return OT_ERROR_NONE;
}

otError expServerFree(void* aContext, uint8_t argsLength, char* aArgs[])
{
  otCoapRemoveResource(OT_INSTANCE, periodicResource);
  otCoapRemoveResource(OT_INSTANCE, aPeriodicResource);

  otCoapStop(OT_INSTANCE);
  free(periodicResource);
  free(aPeriodicResource);

  return OT_ERROR_NONE;
}
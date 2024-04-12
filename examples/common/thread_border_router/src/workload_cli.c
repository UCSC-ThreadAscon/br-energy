#include "workload.h"

/**
 * Will receive packets of Scenario 1.
*/
static otCoapResource *periodicResource;

/**
 * Will receive packets of Scenario 2.
*/
static otCoapResource *aPeriodicResource;

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
  periodicResource = calloc(1, sizeof(otCoapResource));
  createPeriodicResource(periodicResource);
  otCoapAddResource(OT_INSTANCE, periodicResource);
  otLogNotePlat("Set up resource URI: '%s'.", periodicResource->mUriPath);

  /**
   * Allocate HEAP Memory for resource that receive packets from Scenario 2.
  */
  aPeriodicResource = calloc(1, sizeof(otCoapResource));
  createAPeriodicResource(aPeriodicResource);
  otCoapAddResource(OT_INSTANCE, aPeriodicResource);
  otLogNotePlat("Set up resource URI: '%s'.", aPeriodicResource->mUriPath);

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
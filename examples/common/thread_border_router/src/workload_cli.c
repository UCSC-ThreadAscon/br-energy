#include "workload.h"

#define COAP_PORT_1 5685
#define COAP_PORT_2 5686
#define COAP_PORT_3 5687

static otCoapResource *aPeriodicResource;
static otCoapResource *periodicResource;

void startCoapServer(uint16_t port) {
  otError error = otCoapStart(OT_INSTANCE, port);

  if (error != OT_ERROR_NONE) {
    otLogCritPlat("Failed to start COAPS server.");
  } else {
    otLogNotePlat("Started CoAPS server at port %d.", port);
  }
  return;
}

otError expServerStart(void* aContext, uint8_t argsLength, char* aArgs[]) 
{
  checkConnection(OT_INSTANCE);

  startCoapServer(COAP_PORT_1);
  startCoapServer(COAP_PORT_2);
  startCoapServer(COAP_PORT_3);

  /**
   * Allocate HEAP Memory to create APeriodic resource.
  */
  aPeriodicResource = calloc(1, sizeof(otCoapResource));
  createAPeriodicResource(aPeriodicResource);
  otCoapAddResource(OT_INSTANCE, aPeriodicResource);
  otLogNotePlat("Set up resource URI: '%s'.", aPeriodicResource->mUriPath);

  /**
   * Allocate HEAP Memory to create Periodic resource.
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
#include "workload.h"

#define COAP_SECURE_SERVER_PORT CONFIG_COAP_SECURE_SERVER_PORT

static otCoapResource *aPeriodicResource;
static otCoapResource *periodicResource;

void startCoapServer(uint16_t port) {
  otError error = otCoapSecureStartWithMaxConnAttempts(
                    OT_INSTANCE, port,
                    0, NULL, NULL);

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
  x509Init();

  startCoapServer(COAP_SECURE_SERVER_PORT);

  /**
   * Allocate HEAP Memory to create APeriodic resource.
  */
  aPeriodicResource = calloc(1, sizeof(otCoapResource));
  createAPeriodicResource(aPeriodicResource);
  otCoapSecureAddResource(OT_INSTANCE, aPeriodicResource);
  otLogNotePlat("Set up resource URI: '%s'.", aPeriodicResource->mUriPath);

  /**
   * Allocate HEAP Memory to create Periodic resource.
  */
  periodicResource = calloc(1, sizeof(otCoapResource));
  createPeriodicResource(periodicResource);
  otCoapSecureAddResource(OT_INSTANCE, periodicResource);
  otLogNotePlat("Set up resource URI: '%s'.", periodicResource->mUriPath);

  return OT_ERROR_NONE;
}

otError expServerFree(void* aContext, uint8_t argsLength, char* aArgs[])
{
  otCoapSecureRemoveResource(OT_INSTANCE, periodicResource);
  otCoapSecureRemoveResource(OT_INSTANCE, aPeriodicResource);

  if (otCoapSecureIsConnectionActive(OT_INSTANCE)) {
    otCoapSecureDisconnect(OT_INSTANCE);
  }

  otCoapSecureStop(OT_INSTANCE);
  free(periodicResource);
  free(aPeriodicResource);

  return OT_ERROR_NONE;
}
#include "workload.h"

#define COAP_SECURE_SERVER_PORT CONFIG_COAP_SECURE_SERVER_PORT

void aperiodicWorker(void *context) {
    checkConnection(OT_INSTANCE);
    x509Init();

    otError error =
      otCoapSecureStartWithMaxConnAttempts(OT_INSTANCE, COAP_SECURE_SERVER_PORT,
                                           0, NULL, NULL);

    if (error != OT_ERROR_NONE) {
      otLogCritPlat("Failed to start COAPS server.");
    } else {
      otLogNotePlat("Started CoAPS server at port %d.",
                    COAP_SECURE_SERVER_PORT);
    }

    // CoAP server handling aperiodic packets.
    otCoapResource aPeriodicResource;
    createAPeriodicResource(&aPeriodicResource);
    otCoapSecureAddResource(OT_INSTANCE, &aPeriodicResource);
    otLogNotePlat("Set up resource URI: '%s'.", aPeriodicResource.mUriPath);

    // CoAP server for handling periodic packets.
    otCoapResource periodicResource;
    createPeriodicResource(&periodicResource);
    otCoapSecureAddResource(OT_INSTANCE, &periodicResource);
    otLogNotePlat("Set up resource URI: '%s'.", periodicResource.mUriPath);

    while (true) {
      vTaskDelay(MAIN_WAIT_TIME);
    }
  return;
}
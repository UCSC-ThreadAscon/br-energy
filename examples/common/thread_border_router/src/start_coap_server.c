#include "workload.h"
#include "experiment.h"
#include "independent_variables.h"

static otCoapResource battery;
static otCoapResource event;

static inline bool connected(otDeviceRole role)
{
  return (role == OT_DEVICE_ROLE_CHILD)  ||
         (role == OT_DEVICE_ROLE_ROUTER) ||
         (role == OT_DEVICE_ROLE_LEADER);
}

void startCoapServer(uint16_t port) {
  otError error = otCoapStart(OT_INSTANCE, port);

  if (error != OT_ERROR_NONE) {
    otLogCritPlat("Failed to start COAP server.");
  } else {
    otLogNotePlat("Started CoAP server at port %d.", port);
  }
  return;
}

otError expServerStart() 
{
  checkConnection(OT_INSTANCE);
  startCoapServer(OT_DEFAULT_COAP_PORT);

  createResource(&battery, Battery);
  createResource(&event, Event);

  otCoapAddResource(OT_INSTANCE, &battery);
  otCoapAddResource(OT_INSTANCE, &event);

  printNetworkKey();
  PrintDelimiter();
  printCipherSuite();
  printTxPower();
  otLogNotePlat("Set up battery URI: '%s'.", battery.mUriPath);
  otLogNotePlat("Set up event URI: '%s'.", event.mUriPath);
  PrintDelimiter();

  return OT_ERROR_NONE;
}

/**
 * The code for the Delay Server main function comes from the ESP-IDF
 * OpenThread SED state change callback example function:
 * https://github.com/UCSC-ThreadAscon/esp-idf/blob/master/examples/openthread/ot_sleepy_device/deep_sleep/main/esp_ot_sleepy_device.c#L73
 */
void expServerStartCallback(otChangedFlags changed_flags, void* ctx)
{
  OT_UNUSED_VARIABLE(ctx);
  static otDeviceRole s_previous_role = OT_DEVICE_ROLE_DISABLED;

  otInstance* instance = esp_openthread_get_instance();
  if (!instance)
  {
    return;
  }

  otDeviceRole role = otThreadGetDeviceRole(instance);
  if ((connected(role) == true) && (connected(s_previous_role) == false))
  {
    setTxPower();

    /** Start the CoAP server for the Energy Consumption experiments.
     */
    expServerStart();
  }
  s_previous_role = role;
  return;
}
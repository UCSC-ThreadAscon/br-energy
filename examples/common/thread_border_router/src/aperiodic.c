/**
 * The function that determine the rate at which aperiodic packets are
 * sent is from the paper by Betzler et al.:
 * https://www.mdpi.com/1424-8220/14/8/14932
 *
 * Additional resources used:
 * https://en.wikibooks.org/wiki/C_Programming/math.h/log
 * https://en.wikibooks.org/wiki/C_Programming/limits.h
*/
#include "workload.h"

#include "math.h"
#include "limits.h"

/**
 * The smallessst value of lambda used in the experiments by
 * Betlzer et al. was 0.75. However, I noticed that 0.75 generated
 * very short delays that were usually less than 1 second.
 *
 * Turning the lambda way do to 0.1 allows the wait time to be in the magnitude
 * of multiple seconds. This allows me to have the long delays seen in smart-home
 * network traffics, while also giving me a statistically significant number of
 * packets sent within a 2 hour period.
*/
#define LAMBDA 0.1

double randomToLn() {
  double randomNum = (double) esp_random();
  double normalized = randomNum / UINT32_MAX;
  double toLn = 1 - normalized;
  return toLn;
}

double aperiodicWaitTimeSeconds() {
  double toLn = 0;
  do {
    toLn = randomToLn();
  }
  while (toLn == 0); // prevents the case of ln(0)

  double lnResult = log(toLn);
  double numerator = -1 * lnResult;
  double result = numerator / LAMBDA;
  return result;
}

uint32_t aperiodicWaitTimeMs() {
  double waitTimeMs = aperiodicWaitTimeSeconds() * 1000;
  double waitTimeMsFloor = floor(waitTimeMs);
  return (uint32_t) waitTimeMsFloor;
}

static const char* clientAddresses[3] = {
  "fd84:7733:23a0:f199:b184:cbdb:12e4:ef9d",
  "fd84:7733:23a0:f199:d4ef:3f45:f341:500a",
  "fd84:7733:23a0:f199:fb4d:61da:c6d8:60c3"
};

void aperiodicWorkerThread(void *context) {
  otSockAddr socket;
  otIp6Address server;

  EmptyMemory(&socket, sizeof(otSockAddr));
  EmptyMemory(&server, sizeof(otIp6Address));

  uint16_t index = esp_random() % 3;
  otIp6AddressFromString(clientAddresses[index], &server);
  socket.mAddress = server;
  socket.mPort = COAP_SOCK_PORT;

  while (true) {
    sendRequest(APeriodic, &socket);

    uint32_t nextWaitTime = aperiodicWaitTimeMs();
    otLogNotePlat(
      "Will wait %" PRIu32 " ms before sending next aperiodic CoAP request.",
      nextWaitTime
    );

    TickType_t lastWakeupTime = xTaskGetTickCount();

    /**
     * If quotient "nextWaitTime" < "portTICK_PERIOD_MS", then
     * MS_TO_TICKS(nextWaitTime) == 0, causing `vTaskDelayUntil()`
     * to crash. When this happens, set the delay to be exactly
     * `portTICK_PERIOD_MS`.
    */
    TickType_t nextWaitTimeTicks =
      MS_TO_TICKS(nextWaitTime) == 0 ? portTICK_PERIOD_MS :
        MS_TO_TICKS(nextWaitTime);

    vTaskDelayUntil(&lastWakeupTime, nextWaitTimeTicks);
    }
  return;
}
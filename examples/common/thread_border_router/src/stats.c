#include "workload.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

static uint64_t prevBatteryMs = 0;
static bool firstBattery = true;

void printMsElaspedBattery(uint64_t curBatteryMs, char* ipString) {
  if (firstBattery) {
    otLogNotePlat("First battery packet sent by %s.", ipString);
    firstBattery = false;
  }
  else {
    uint64_t msElapsed = curBatteryMs - prevBatteryMs;
    otLogNotePlat("%d ms since last battery packet by %s.",
                  (int) msElapsed, ipString);
  }

  prevBatteryMs = curBatteryMs;
  return;
}

void printUptime(char *ipString, Route route)
{
  char uptimeString[OT_UPTIME_STRING_SIZE];
  EmptyMemory(&uptimeString, sizeof(uptimeString));
  otInstanceGetUptimeAsString(OT_INSTANCE,
                             (char *) uptimeString,
                              sizeof(uptimeString));

  if (route == Battery) {
    otLogNotePlat("[%s] Battery Packet sent by %s.", uptimeString, ipString);
    printMsElaspedBattery(otInstanceGetUptime(OT_INSTANCE), ipString);
  }
  else {
    otLogNotePlat("[%s] Event Packet sent by %s.", uptimeString, ipString);
  }
  return;
}
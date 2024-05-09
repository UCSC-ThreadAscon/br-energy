#include "workload.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

#define ADDRESS_SED_1 "fd84:7733:23a0:f199:e202:6480:d921:b15"

static DebugStats statsSed1 = {
  ADDRESS_SED_1,    // address
  0,                // prevBatteryMs
  true,             // firstBattery
  0,                // eventsReceived
  0,                // firstEventMs
  true              // firstEvent
};

DebugStats *findSed(const char *ipString) {
  if (strcmp(ipString, ADDRESS_SED_1) == 0) {
    return &statsSed1;
  }

  otLogCritPlat("Failed to find device with IP address %s", ipString);
  return NULL;
}

void printMsElaspedBattery(DebugStats *sedStats,
                           uint64_t curBatteryMs,
                           char* ipString)
{
  if (sedStats->firstBattery)
  {
    otLogNotePlat("First battery packet sent by %s.", ipString);
    sedStats->firstBattery = false;
  }
  else
  {
    uint64_t msElapsed = curBatteryMs - sedStats->prevBatteryMs;
    otLogNotePlat("%d ms since last battery packet by %s.",
                  (int) msElapsed, ipString);
  }

  sedStats->prevBatteryMs = curBatteryMs;
  return;
}

void printMsEvents(DebugStats *sedStats,
                   uint64_t curEventMs,
                   char* ipString,
                   char* uptimeString)
{
  sedStats->eventsReceived += 1;

  if (sedStats->firstEvent)
  {
    sedStats->firstEventMs = curEventMs;
    sedStats->firstEvent = false;
  }

  otLogNotePlat("[%s] %d Event Packet so far sent by %s.",
                uptimeString, sedStats->eventsReceived, sedStats->address);
  return;
}

void printStats(char *ipString, Route route)
{
  DebugStats *sedStats = findSed(ipString);
  if (sedStats == NULL) { return; }

  char uptimeString[OT_UPTIME_STRING_SIZE];
  EmptyMemory(&uptimeString, sizeof(uptimeString));
  otInstanceGetUptimeAsString(OT_INSTANCE, (char *) uptimeString, sizeof(uptimeString));

  uint64_t uptime = otInstanceGetUptime(OT_INSTANCE);
  if (route == Battery) {
    printMsElaspedBattery(sedStats, uptime, ipString);
  }
  else {
    printMsEvents(sedStats, uptime, ipString, uptimeString);
  }
  return;
}
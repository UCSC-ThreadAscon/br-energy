#include "workload.h"
#include "time_api.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

#define ADDRESS_SED_1 "fd07:dd09:5dd8:2c0c:9792:c676:49f5:d325"
#define ADDRESS_SED_2 "fd07:dd09:5dd8:2c0c:1ffc:5e2a:b7fd:7243"
#define ADDRESS_SED_3 "fd07:dd09:5dd8:2c0c:87c3:4196:6d07:3a5f"

static DebugStats statsSed1 = {
  ADDRESS_SED_1,    // address
  0,                // prevBatteryMs
  true,             // firstBattery
  0,                // numBatteryPackets
  0,                // eventsReceived
  true,             // firstEvent
  0                 // powerOnTime
};

static DebugStats statsSed2 = {
  ADDRESS_SED_2,    // address
  0,                // prevBatteryMs
  true,             // firstBattery
  0,                // numBatteryPackets
  0,                // eventsReceived
  true,             // firstEvent
  0                 // powerOnTime
};

static DebugStats statsSed3 = {
  ADDRESS_SED_3,    // address
  0,                // prevBatteryMs
  true,             // firstBattery
  0,                // numBatteryPackets
  0,                // eventsReceived
  true,             // firstEvent
  0                 // powerOnTime
};

#define IsSed(address, sedStructPtr)               \
  if (strcmp(ipString, address) == 0) {            \
    return sedStructPtr;                           \
  }                                                \

DebugStats *findSed(const char *ipString)
{
  IsSed(ADDRESS_SED_1, &statsSed1);
  IsSed(ADDRESS_SED_2, &statsSed2);
  IsSed(ADDRESS_SED_3, &statsSed3);
  otLogCritPlat("Failed to find device with IP address %s.", ipString);
  return NULL;
}

#define BATTERY_MARGIN_SECONDS 32
#define BATTERY_MARGIN_MS SECONDS_TO_MS(BATTERY_MARGIN_SECONDS)

void printMsElaspedBattery(DebugStats *sedStats,
                           uint64_t uptime,
                           char* ipString)
{
  sedStats->numBatteyPackets += 1;

  if (sedStats->firstBattery)
  {
    otLogNotePlat("First battery packet sent by %s.", ipString);

    // The first packet sent is always a battery packet.
    sedStats->powerOnTime = uptime;

    sedStats->firstBattery = false;
  }
  else
  {
    uint64_t msElapsed = uptime - sedStats->prevBatteryMs;
    if (msElapsed <= BATTERY_MARGIN_MS)
    {
      otLogNotePlat("[%d ms] last battery packet by %s.", (int) msElapsed, ipString);
    }
    else
    {
      otLogWarnPlat("[%d ms] Large gap between last battery packet sent by %s.",
                    (int) msElapsed, ipString);
    }
  }

  sedStats->prevBatteryMs = uptime;
  otLogNotePlat("%d Battery Packet(s) sent so far by %s.",
                (int) sedStats->numBatteyPackets, ipString);
  return;
}

void printMsEvents(DebugStats *sedStats,
                   uint64_t uptime,
                   char* ipString)
{
  sedStats->eventsReceived += 1;

  if (sedStats->firstEvent)
  {
    sedStats->firstEvent = false;
    otLogNotePlat("First event packet sent by %s.", sedStats->address);
  }

  uint64_t msElapsed = uptime - sedStats->powerOnTime;
  double minsElapsed = MS_TO_MINUTES((double) msElapsed);

  otLogNotePlat("[~%.3f minutes] %d Event Packet(s) so far sent by %s.",
                minsElapsed, sedStats->eventsReceived, sedStats->address);
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
    printMsEvents(sedStats, uptime, uptimeString);
  }
  return;
}
/**
 * All of the code in this file is based upon the both CoAP and CoAP secure
 * source code used as a part of the OpenThread codebase.
 *
 * https://github.com/UCSC-ThreadAscon/openthread/tree/main/src/cli
*/
#include "workload.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

#define PRINT_UPTIME 1
#define PRINT_PAYLOADS 0

void getPeerAddrString(const otMessageInfo *aMessageInfo, char *ipString) {
  otIp6AddressToString(&(aMessageInfo->mPeerAddr), ipString,
                       OT_IP6_ADDRESS_STRING_SIZE);
  return;
}

void printEventPacket(otMessage *aMessage, char *ipString)
{
  EventPayload event;
  EmptyMemory(&event, sizeof(EventPayload));
  getPayload(aMessage, &event);

#if PRINT_UPTIME
  printStats(ipString, Event);
#endif
#if PRINT_PAYLOADS
  char *occured = event.eventOccured ? "Event detected" : "Event not detected";
  otLogNotePlat("%s from %s.", occured, ipString);
#endif
}

void printBatteryPacket(otMessage *aMessage, char *ipString)
{
  BatteryPayload battery;
  EmptyMemory(&battery, sizeof(BatteryPayload));
  getPayload(aMessage, (void *) &battery);

#if PRINT_UPTIME
  printStats(ipString, Battery);
#endif
#if PRINT_PAYLOADS
  int batteryLife = (int) battery.batteryLife;
  otLogNotePlat("Battery of %d from %s.", batteryLife, ipString);
#endif
  return;
} 

/**
 * This function is a modified version of `HandleRequest()` from the OpenThread CLI
 * Secure CoAP source code:
 * https://github.com/UCSC-ThreadAscon/openthread/blob/main/src/cli/cli_coap_secure.cpp#L814
*/
void sendCoapResponse(otMessage *aRequest, const otMessageInfo *aRequestInfo)
{
  otMessage *aResponse = NULL;
  otCoapCode status = OT_COAP_CODE_EMPTY;

  bool isConfirmable = otCoapMessageGetType(aRequest) == OT_COAP_TYPE_CONFIRMABLE;
  bool isGet = otCoapMessageGetCode(aRequest) == OT_COAP_CODE_GET;

  if (isConfirmable || isGet) {
    if (isGet) {
      status = OT_COAP_CODE_CONTENT;
    }
    else {
      status = OT_COAP_CODE_VALID;
    }

    aResponse = otCoapNewMessage(OT_INSTANCE, NULL);
    if (aResponse == NULL) {
      otLogCritPlat("Failed to initialize a new message for CoAP response.");
    }

    otError error = otCoapMessageInitResponse(aResponse, aRequest,
                                              OT_COAP_TYPE_ACKNOWLEDGMENT,
                                              status);
    HandleMessageError("coap message init response", aResponse, error);

    error = otCoapSendResponse(OT_INSTANCE, aResponse, aRequestInfo);
    HandleMessageError("send response", aResponse, error);
  }

  return;
}

void batteryRequestHandler(void* aContext,
                           otMessage *aMessage,
                           const otMessageInfo *aMessageInfo)
{
  uint32_t length = getPayloadLength(aMessage);
  assert(length == sizeof(BatteryPayload));

  char senderAddress[OT_IP6_ADDRESS_STRING_SIZE];
  EmptyMemory(senderAddress, OT_IP6_ADDRESS_STRING_SIZE);

  getPeerAddrString(aMessageInfo, senderAddress);
  printBatteryPacket(aMessage, senderAddress);

  sendCoapResponse(aMessage, aMessageInfo);
  return;
}

void eventRequestHandler(void* aContext,
                         otMessage *aMessage,
                         const otMessageInfo *aMessageInfo)
{
  uint32_t length = getPayloadLength(aMessage);
  assert(length == sizeof(EventPayload));

  char senderAddress[OT_IP6_ADDRESS_STRING_SIZE];
  EmptyMemory(senderAddress, OT_IP6_ADDRESS_STRING_SIZE);

  getPeerAddrString(aMessageInfo, senderAddress);
  printEventPacket(aMessage, senderAddress);

  sendCoapResponse(aMessage, aMessageInfo);
  return;
}


otError createResource(otCoapResource *resource, Route route) {
  resource->mNext = NULL;
  resource->mContext = NULL;

  if (route == Battery) {
    resource->mUriPath = BATTERY_URI;
    resource->mHandler = batteryRequestHandler;
  }
  else {
    resource->mUriPath = EVENT_URI;
    resource->mHandler = eventRequestHandler;
  }

  return OT_ERROR_NONE;
}
/**
 * All of the code in this file is based upon the CoAP source code used
 * as a part of the OpenThread codebase.
 *
 * https://github.com/openthread/openthread/blob/main/src/cli/cli_coap_secure.cpp
 * https://github.com/openthread/openthread/blob/main/include/openthread/coap_secure.h
 * https://github.com/openthread/openthread/blob/main/include/openthread/coap.h
 * https://github.com/openthread/openthread/blob/main/src/cli/README_COAPS.md
*/
#include "workload.h"

#include <stdio.h>
#include <string.h>
#include "stdint.h"
#include "inttypes.h"

void getPeerAddrString(const otMessageInfo *aMessageInfo, char *ipString) {
  otIp6AddressToString(&(aMessageInfo->mPeerAddr), ipString,
                       OT_IP6_ADDRESS_STRING_SIZE);
  return;
}

void printCoapRequest(otMessage *aMessage,
                      uint32_t payloadLen,
                      char *ipString,
                      type type) 
{
  uint16_t payloadSize =
    type == APeriodic ? APERIODIC_PAYLOAD_SIZE : PERIODIC_PAYLOAD_SIZE;

  char payload[payloadSize];
  getPayload(aMessage, payload);
  otLogNotePlat("Received %" PRIu32 " bytes from %s.",
                payloadLen, ipString);
  return;
}

/**
 * This function is a modified version of `HandleRequest()`
 * from the OpenThread CLI CoAP source code:
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

    error = otCoapSecureSendResponse(OT_INSTANCE, aResponse, aRequestInfo);
    HandleMessageError("send response", aResponse, error);
  }

  return;
}

void requestHandler(void *aContext,
                    otMessage *aMessage,
                    const otMessageInfo *aMessageInfo,
                    type type)
{
  uint32_t length = getPayloadLength(aMessage);

  char senderAddress[OT_IP6_ADDRESS_STRING_SIZE];
  EmptyMemory(senderAddress, OT_IP6_ADDRESS_STRING_SIZE);

  getPeerAddrString(aMessageInfo, senderAddress);
  printCoapRequest(aMessage, length, senderAddress, type);

  sendCoapResponse(aMessage, aMessageInfo);
  return;
}

void periodicRequestHandler(void *aContext,
                            otMessage *aMessage,
                            const otMessageInfo *aMessageInfo)
{
  requestHandler(aContext, aMessage, aMessageInfo, Periodic);
  return;
}

void aPeriodicRequestHandler(void *aContext,
                            otMessage *aMessage,
                            const otMessageInfo *aMessageInfo)
{
  requestHandler(aContext, aMessage, aMessageInfo, APeriodic);
  return;
}

otError createAPeriodicResource(otCoapResource *aperiodic) {
  aperiodic->mNext = NULL;
  aperiodic->mContext = NULL;
  aperiodic->mUriPath = "aperiodic";
  aperiodic->mHandler = aPeriodicRequestHandler;
  return OT_ERROR_NONE;
}

otError createPeriodicResource(otCoapResource *periodic) {
  periodic->mNext = NULL;
  periodic->mContext = NULL;
  periodic->mUriPath = "periodic";
  periodic->mHandler = periodicRequestHandler;
  return OT_ERROR_NONE;
}
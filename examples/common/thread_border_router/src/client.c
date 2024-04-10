/**
 * All of the code in this file is based upon the both CoAP and CoAP secure
 * source code used as a part of the OpenThread codebase.
 *
 * https://github.com/UCSC-ThreadAscon/openthread/tree/main/src/cli
*/
#include "workload.h"

void handleResponse(void *aContext,
                    otMessage *aMessage,
                    const otMessageInfo *aMessageInfo,
                    otError aResult)
{
  if (aResult != OT_ERROR_NONE) {
    otLogWarnPlat("Response error: %s", otThreadErrorToString(aResult));
  } else {
    uint16_t payloadLen = getPayloadLength(aMessage);
    char payload[payloadLen];
    EmptyMemory(payload, payloadLen);
    getPayload(aMessage, payload);

    char senderAddr[OT_IP6_ADDRESS_STRING_SIZE];
    otIp6AddressToString(&(aMessageInfo->mPeerAddr), senderAddr,
                         OT_IP6_ADDRESS_STRING_SIZE);

    otLogNotePlat("Response from %s of size %" PRIu16 " bytes.",
                  senderAddr, payloadLen);
  }
  return;
}

void createMessageInfo(otSockAddr *aSocket, otMessageInfo *aMessageInfo) {
  aMessageInfo->mHopLimit = 0;  // default

  aMessageInfo->mPeerAddr = aSocket->mAddress;
  aMessageInfo->mPeerPort = aSocket->mPort;

  aMessageInfo->mSockAddr = *otThreadGetMeshLocalEid(OT_INSTANCE);
  aMessageInfo->mSockPort = COAP_SOCK_PORT;
  return;
}

void sendRequest(type type, otSockAddr *socket) {
  otMessageInfo aMessageInfo;
  EmptyMemory(&aMessageInfo, sizeof(otMessageInfo));
  createMessageInfo(socket, &aMessageInfo);

  otMessage *aRequest = otCoapNewMessage(OT_INSTANCE, NULL);
  if (aRequest == NULL) {
    otLogCritPlat("Failed to create CoAP request.");
    return;
  }

  otCoapMessageInit(aRequest, OT_COAP_TYPE_CONFIRMABLE, OT_COAP_CODE_POST);
  otCoapMessageGenerateToken(aRequest, OT_COAP_DEFAULT_TOKEN_LENGTH);

  otError error = otCoapMessageAppendUriPathOptions(aRequest,
                    type == APeriodic ? APERIODIC_URI : PERIODIC_URI);
  HandleMessageError("append uri options", aRequest, error);

  error = otCoapMessageSetPayloadMarker(aRequest);
  HandleMessageError("set payload marker", aRequest, error);

  uint16_t responseSize =
    type == APeriodic ? APERIODIC_PAYLOAD_SIZE : PERIODIC_PAYLOAD_SIZE;

  char* response[responseSize];
  esp_fill_random(response, responseSize);

  error = otMessageAppend(aRequest, response, responseSize);
  HandleMessageError("message append", aRequest, error);

  error = otCoapSendRequest(OT_INSTANCE, aRequest, &aMessageInfo,
                            handleResponse, NULL);
  HandleMessageError("send request", aRequest, error);

  char destString[OT_IP6_ADDRESS_STRING_SIZE];
  otIp6AddressToString(&(socket->mAddress), destString, OT_IP6_ADDRESS_STRING_SIZE);
  otLogNotePlat("Sent a %s message of %d bytes to %s.", 
                type == APeriodic ? "aperiodic" : "periodic",
                responseSize, destString);

  return;
}
#include "independent_variables.h"

void printCipherSuite()
{
#if (CIPHER_SUITE == AES)
  otLogNotePlat("Cipher Suite: AES");
#elif (CIPHER_SUITE == NONE)
  otLogNotePlat("Cipher Suite: No Encryption");
#elif (CIPHER_SUITE == LIBASCON_128A)
  otLogNotePlat("Cipher Suite: ASCON-128a (LibAscon)");
#elif (CIPHER_SUITE == LIBASCON_128)
  otLogNotePlat("Cipher Suite: ASCON-128 (LibAscon)");
#endif
  return;
}

void printTxPower()
{
  int8_t txPower = 0;
  if (getTxPower(&txPower) != OT_ERROR_NONE)
  {
    otLogCritPlat("Failed to get TX Power.");
  }
  return;
}

void printNetworkKey()
{
  otNetworkKey key;
  EmptyMemory(&key, sizeof(otNetworkKey));

  otThreadGetNetworkKey(esp_openthread_get_instance(), &key);
  otDumpNotePlat("Thread Network Key:", key.m8, OT_NETWORK_KEY_SIZE);
  return;
}
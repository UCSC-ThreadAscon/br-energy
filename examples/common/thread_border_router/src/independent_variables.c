#include "utilities.h"
#include "experiment.h"

void printIndependentVars(void)
{
#if (CIPHER_SUITE == AES)
  otLogNotePlat("Cipher Suite: AES");
#elif (CIPHER_SUITE == NONE)
  otLogNotePlat("Cipher Suite: No Encryption");
#elif (CIPHER_SUITE == LIBASCON_128A)
  otLogNotePlat("Cipher Suite: ASCON-128a");
#elif (CIPHER_SUITE == LIBASCON_128)
  otLogNotePlat("Cipher Suite: ASCON-128");
#endif

  int8_t currentPower = 0;
  if (getTxPower(&currentPower) != OT_ERROR_NONE)
  {
    otLogCritPlat("Failed to get TX Power.");
  }
  return;
}
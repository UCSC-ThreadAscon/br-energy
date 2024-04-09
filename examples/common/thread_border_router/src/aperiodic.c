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
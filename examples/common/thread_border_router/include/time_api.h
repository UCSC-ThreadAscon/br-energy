#pragma once

#define US_TO_MS(us) us / 1000
#define MS_TO_US(ms) ms * 1000

#define MS_TO_SECONDS(ms) ms / 1000
#define MS_TO_MINUTES(ms) MS_TO_SECONDS(ms) / 60
#define US_TO_MINUTES(us) MS_TO_MINUTES(US_TO_MS(us))

#define SECONDS_TO_MS(secs) secs * 1000
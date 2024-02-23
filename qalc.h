#ifndef _QALC_H
#define _QALC_H
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

void initialize_qalc();
int update_exchange_rates();
time_t exchange_rates_time();
int qalculate(const char *input, char *out, int len, int color);

#ifdef __cplusplus
}
#endif

#endif

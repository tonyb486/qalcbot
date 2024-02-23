#include "qalc.h"
#include <stdio.h>
#include <libqalculate/Calculator.h>
#include <string>

extern "C"
{

    void initialize_qalc()
    {
        new Calculator();
        CALCULATOR->loadExchangeRates();
        CALCULATOR->loadGlobalDefinitions();
        CALCULATOR->loadLocalDefinitions();
    }

    int update_exchange_rates()
    {
        bool r1 = CALCULATOR->fetchExchangeRates();
        bool r2 = CALCULATOR->loadExchangeRates();

        return (r1 && r2) ? 1 : 0;
    }

    time_t exchange_rates_time()
    {
        return CALCULATOR->getExchangeRatesTime();
    }

    int qalculate(const char *in, char *out, int len, int color)
    {

        EvaluationOptions eo;
        PrintOptions po;

        eo.calculate_variables = false;
        eo.interval_calculation = INTERVAL_CALCULATION_NONE;
        eo.approximation = APPROXIMATION_APPROXIMATE;

        std::string pe;
        std::string rs = CALCULATOR->calculateAndPrint(CALCULATOR->unlocalizeExpression(in), 2000, eo, po, AUTOMATIC_FRACTION_OFF, AUTOMATIC_APPROXIMATION_AUTO, &pe, 1024, NULL, true, color, TAG_TYPE_TERMINAL);

        return snprintf(out, len, "%s ≈ %s", pe.c_str(), rs.c_str());
    }
}
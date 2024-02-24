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

        bool is_approx;

        EvaluationOptions eo;
        PrintOptions po;

        eo.calculate_variables = true;

        po.interval_display = INTERVAL_DISPLAY_SIGNIFICANT_DIGITS;
        po.is_approximate = &is_approx;

        po.use_max_decimals = true;
        po.max_decimals = 10;
        

        std::string pe;
        std::string rs = CALCULATOR->calculateAndPrint(CALCULATOR->unlocalizeExpression(in), 2000, eo, po, AUTOMATIC_FRACTION_AUTO, AUTOMATIC_APPROXIMATION_AUTO, &pe, 1024, NULL, true, color, TAG_TYPE_TERMINAL);

        CALCULATOR->resetVariables();

        if(CALCULATOR->checkSaveFunctionCalled()) 
            return snprintf(out, len, "Cannot save variables.");

        if(is_approx)
            return snprintf(out, len, "%s ≈ %s", pe.c_str(), rs.c_str());
        else
            return snprintf(out, len, "%s = %s", pe.c_str(), rs.c_str());

    }
}
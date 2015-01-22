#include "locale.h"
#include "string.h"

static const char *__default_locale = "C";

const char *setlocale(int category, const char *locale)
{
    (void)category;
    (void)locale;
    //FIXME
    return __default_locale;
}

static struct lconv _locale_data;
static bool _locale_data_initted = false;

struct lconv *localeconv(void)
{
    if (!_locale_data_initted) {
        _locale_data.decimal_point = strdup(".");
        _locale_data.thousands_sep = strdup("");
        _locale_data.grouping = strdup("30");

        _locale_data.int_curr_symbol = strdup("USD ");
        _locale_data.currency_symbol = strdup("$");
        _locale_data.mon_decimal_point = strdup(".");
        _locale_data.mon_thousands_sep = strdup("");
        _locale_data.mon_grouping = strdup("30");
        _locale_data.positive_sign = strdup("");
        _locale_data.negative_sign = strdup("-");

        _locale_data.int_frac_digits = 2;
        _locale_data.frac_digits = 2;
        _locale_data.p_cs_precedes = 1;
        _locale_data.p_sep_by_space = 1;
        _locale_data.n_cs_precedes = 0;
        _locale_data.n_sep_by_space = 0;

        _locale_data.p_sign_posn = 3;
        _locale_data.n_sign_posn = 3;

        _locale_data_initted = true;
    }

    return &_locale_data;
}

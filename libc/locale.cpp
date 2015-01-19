#include "locale.h"

static const char *__default_locale = "C";

const char *setlocale(int category, const char *locale)
{
    (void)category;
    (void)locale;
    //FIXME
    return __default_locale;
}

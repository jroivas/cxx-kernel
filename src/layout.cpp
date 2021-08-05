#include "kb/layout.h"
#include "kb/layout_us.h"
#include "kb/layout_fi.h"

const char **get_layout(enum layout_code lang)
{
    switch (lang) {
    case LAYOUT_FI:
        return layout_fi;
    case LAYOUT_US:
    default:
        return layout_us;
    };
}

const char **get_layout_upcase(enum layout_code lang)
{
    switch (lang) {
    case LAYOUT_FI:
        return layout_fi_up;
    case LAYOUT_US:
    default:
        return layout_us_up;
    };
}

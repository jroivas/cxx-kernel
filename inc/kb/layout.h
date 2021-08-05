#pragma once

enum layout_code {
    LAYOUT_DEFAULT = 0,
    LAYOUT_US = 1,
    LAYOUT_FI = 2
};

extern "C" const char **get_layout(enum layout_code lang);
extern "C" const char **get_layout_upcase(enum layout_code lang);

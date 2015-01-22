#ifndef __LOCALE_H
#define __LOCALE_H

#ifdef __cplusplus
extern "C" {
#endif

#define LC_ALL 1
#define LC_ADDRESS 2
#define LC_COLLATE 3
#define LC_CTYPE 4
#define LC_IDENTIFICATION 5
#define LC_MEASUREMENT 6
#define LC_MESSAGES 7
#define LC_MONETARY 8
#define LC_NAME 9
#define LC_NUMERIC 10
#define LC_PAPER 11
#define LC_TELEPHONE 12
#define LC_TIME 13

struct lconv
{
    char *decimal_point;
    char *thousands_sep;
    char *grouping;

    char *int_curr_symbol;
    char *currency_symbol;
    char *mon_decimal_point;
    char *mon_thousands_sep;
    char *mon_grouping;
    char *positive_sign;
    char *negative_sign;

    char int_frac_digits;
    char frac_digits;
    char p_cs_precedes;
    char p_sep_by_space;
    char n_cs_precedes;
    char n_sep_by_space;
    char p_sign_posn;
    char n_sign_posn;
};

const char *setlocale(int category, const char *locale);
struct lconv *localeconv(void);

#ifdef __cplusplus
}
#endif

#endif

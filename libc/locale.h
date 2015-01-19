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

const char *setlocale(int category, const char *locale);

#ifdef __cplusplus
}
#endif

#endif

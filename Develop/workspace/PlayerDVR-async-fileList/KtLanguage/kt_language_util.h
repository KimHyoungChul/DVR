#ifndef __KT_LANGUAGE_UTIL_H
#define __KT_LANGUAGE_UTIL_H
#ifdef __cplusplus
extern "C" {
#endif /**/

int kt_language_load(const char *path);
int kt_language_close();
int kt_language_parser();
int kt_language_resFini();

#ifdef __cplusplus
}
#endif /**/

#endif /*__KT_LANGUAGE_UTIL_H*/

/**
 * @file xf_string.h
 * @author catcatBlue (catcatblue@qq.com)
 * @brief
 * @version 1.0
 * @date 2025-01-18
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef __XF_STRING_H__
#define __XF_STRING_H__

/* ==================== [Includes] ========================================== */

#include "xf_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

/* ==================== [Global Prototypes] ================================= */

void *xf_memcpy(void *dst, const void *src, size_t len);
void xf_memset(void *dst, uint8_t v, size_t len);
void *xf_memmove(void *dst, const void *src, size_t len);
int xf_memcmp(const void *p1, const void *p2, size_t len);
size_t xf_strlen(const char *str);
size_t xf_strnlen(const char *str, size_t maxlen);
size_t xf_strlcpy(char *dst, const char *src, size_t dst_size);
char *xf_strncpy(char *dst, const char *src, size_t dest_size);
char *xf_strcpy(char *dst, const char *src);
int xf_strcmp(const char *s1, const char *s2);
// char *xf_strdup(const char *src);
char *xf_strcat(char *dst, const char *src);
char *xf_strncat(char *dst, const char *src, size_t src_len);

/* ==================== [Macros] ============================================ */

#define xf_memzero(dst, len) xf_memset((dst), 0x00, (len))

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __XF_STRING_H__ */

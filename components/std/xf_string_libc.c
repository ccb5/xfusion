/**
 * @file xf_string_libc.c
 * @author catcatBlue (catcatblue@qq.com)
 * @brief 
 * @version 1.0
 * @date 2025-06-24
 * 
 * SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
 * SPDX-License-Identifier: Apache-2.0
 * 
 */

/* ==================== [Includes] ========================================== */

#include "xf_string.h"

#if XF_STRING_ENABLE_LIBC

#include <string.h>

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

/* ==================== [Static Prototypes] ================================= */

/* ==================== [Static Variables] ================================== */

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================= */

void *XF_ATTR_FAST_MEM xf_memcpy(void *dst, const void *src, size_t len)
{
    return memcpy(dst, src, len);
}

void XF_ATTR_FAST_MEM xf_memset(void *dst, uint8_t v, size_t len)
{
    memset(dst, v, len);
}

void *XF_ATTR_FAST_MEM xf_memmove(void *dst, const void *src, size_t len)
{
    return memmove(dst, src, len);
}

int xf_memcmp(const void *p1, const void *p2, size_t len)
{
    return memcmp(p1, p2, len);
}

size_t xf_strlen(const char *str)
{
    return strlen(str);
}

size_t xf_strnlen(const char *str, size_t maxlen)
{
    return strnlen(str, maxlen);
}
size_t xf_strlcpy(char *dst, const char *src, size_t dst_size)
{
    size_t src_len = strlen(src);
    if (dst_size > 0) {
        size_t copy_size = src_len < dst_size ? src_len : dst_size - 1;
        memcpy(dst, src, copy_size);
        dst[copy_size] = '\0';
    }
    return src_len;
}

char *xf_strncpy(char *dst, const char *src, size_t dest_size)
{
    return strncpy(dst, src, dest_size);
}

char *xf_strcpy(char *dst, const char *src)
{
    return strcpy(dst, src);
}

int xf_strcmp(const char *s1, const char *s2)
{
    return strcmp(s1, s2);
}

#if XF_TODO
char *xf_strdup(const char *src)
{
    /*strdup uses malloc, so use the xf_malloc when XF_USE_STDLIB_MALLOC is not XF_STDLIB_CLIB */
    size_t len = xf_strlen(src) + 1;
    char *dst = xf_malloc(len);
    if (dst == NULL) { return NULL; }

    xf_memcpy(dst, src, len); /*do memcpy is faster than strncpy when length is known*/
    return dst;
}
#endif

char *xf_strcat(char *dst, const char *src)
{
    return strcat(dst, src);
}

char *xf_strncat(char *dst, const char *src, size_t src_len)
{
    return strncat(dst, src, src_len);
}

/* ==================== [Static Functions] ================================== */

#endif /* XF_STRING_ENABLE_LIBC */

/**
 * @file xf_string_builtin.c
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

#if CONFIG_XF_STRING_BUILTIN

/* ==================== [Defines] =========================================== */

#define ALIGN_MASK       (sizeof(void *) - 1)

/* ==================== [Typedefs] ========================================== */

/* ==================== [Static Prototypes] ================================= */

/* ==================== [Static Variables] ================================== */

/* ==================== [Macros] ============================================ */

#define _COPY(d, s) *d = *s; d++; s++;
#define _SET(d, v) *d = v; d++;
#define _REPEAT8(expr) expr expr expr expr expr expr expr expr

/* ==================== [Global Functions] ================================= */

void *XF_ATTR_FAST_MEM xf_memcpy(void *dst, const void *src, size_t len)
{
    uint8_t *d8 = dst;
    const uint8_t *s8 = src;

    /*Simplify for small memories*/
    if (len < 16) {
        while (len) {
            *d8 = *s8;
            d8++;
            s8++;
            len--;
        }
        return dst;
    }

    uintptr_t d_align = (uintptr_t)d8 & ALIGN_MASK;
    uintptr_t s_align = (uintptr_t)s8 & ALIGN_MASK;

    /*Byte copy for unaligned memories*/
    if (s_align != d_align) {
        while (len > 32) {
            _REPEAT8(_COPY(d8, s8));
            _REPEAT8(_COPY(d8, s8));
            _REPEAT8(_COPY(d8, s8));
            _REPEAT8(_COPY(d8, s8));
            len -= 32;
        }
        while (len) {
            _COPY(d8, s8)
            len--;
        }
        return dst;
    }

    /*Make the memories aligned*/
    if (d_align) {
        d_align = ALIGN_MASK + 1 - d_align;
        while (d_align && len) {
            _COPY(d8, s8);
            d_align--;
            len--;
        }
    }

    uint32_t *d32 = (uint32_t *)d8;
    const uint32_t *s32 = (uint32_t *)s8;
    while (len > 32) {
        _REPEAT8(_COPY(d32, s32))
        len -= 32;
    }

    d8 = (uint8_t *)d32;
    s8 = (const uint8_t *)s32;
    while (len) {
        _COPY(d8, s8)
        len--;
    }

    return dst;
}

void XF_ATTR_FAST_MEM xf_memset(void *dst, uint8_t v, size_t len)
{
    uint8_t *d8 = (uint8_t *)dst;
    uintptr_t d_align = (uintptr_t) d8 & ALIGN_MASK;

    /*Make the address aligned*/
    if (d_align) {
        d_align = ALIGN_MASK + 1 - d_align;
        while (d_align && len) {
            _SET(d8, v);
            len--;
            d_align--;
        }
    }

    uint32_t v32 = (uint32_t)v + ((uint32_t)v << 8) + ((uint32_t)v << 16) + ((uint32_t)v << 24);
    uint32_t *d32 = (uint32_t *)d8;

    while (len > 32) {
        _REPEAT8(_SET(d32, v32));
        len -= 32;
    }

    d8 = (uint8_t *)d32;
    while (len) {
        _SET(d8, v);
        len--;
    }
}

void *XF_ATTR_FAST_MEM xf_memmove(void *dst, const void *src, size_t len)
{
    if (dst < src || (char *)dst > ((char *)src + len)) {
        return xf_memcpy(dst, src, len);
    }

    if (dst > src) {
        char *tmp = (char *)dst + len - 1;
        char *s   = (char *)src + len - 1;

        while (len--) {
            *tmp-- = *s--;
        }
    } else {
        char *tmp = (char *)dst;
        char *s   = (char *)src;

        while (len--) {
            *tmp++ = *s++;
        }
    }

    return dst;
}

int xf_memcmp(const void *p1, const void *p2, size_t len)
{
    const char *s1 = (const char *) p1;
    const char *s2 = (const char *) p2;
    while (--len > 0 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

/* See https://en.cppreference.com/w/c/string/byte/strlen for reference */
size_t xf_strlen(const char *str)
{
    size_t i = 0;
    while (str[i]) { i++; }
    return i;
}

size_t xf_strnlen(const char *str, size_t maxlen)
{
    size_t i = 0;
    while (str[i] && (i < maxlen)) { i++; }
    return i;
}

size_t xf_strlcpy(char *dst, const char *src, size_t dst_size)
{
    size_t i = 0;
    if (dst_size > 0) {
        for (; i < dst_size - 1 && src[i]; i++) {
            dst[i] = src[i];
        }
        dst[i] = '\0';
    }
    while (src[i]) { i++; }
    return i;
}

char *xf_strncpy(char *dst, const char *src, size_t dst_size)
{
    size_t i;
    for (i = 0; i < dst_size && src[i]; i++) {
        dst[i] = src[i];
    }
    for (; i < dst_size; i++) {
        dst[i] = '\0';
    }
    return dst;
}

char *xf_strcpy(char *dst, const char *src)
{
    char *tmp = dst;
    while ((*dst++ = *src++) != '\0');
    return tmp;
}

int xf_strcmp(const char *s1, const char *s2)
{
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

 __compiletime_error("unfinished") char *xf_strdup(const char *src)
{
    /* TODO */
#if 0
    size_t len = xf_strlen(src) + 1;
    char *dst = xf_malloc(len);
    if (dst == NULL) { return NULL; }

    xf_memcpy(dst, src, len); /*memcpy is faster than strncpy when length is known*/
    return dst;
#else
    return NULL;
#endif
}

char *xf_strcat(char *dst, const char *src)
{
    xf_strcpy(dst + xf_strlen(dst), src);
    return dst;
}

char *xf_strncat(char *dst, const char *src, size_t src_len)
{
    char *tmp = dst;
    while (*dst != '\0') {
        dst++;
    }
    while (src_len != 0 && *src != '\0') {
        src_len--;
        *dst++ = *src++;
    }
    *dst = '\0';
    return tmp;
}

/* ==================== [Static Functions] ================================== */

#endif /* CONFIG_XF_STRING_BUILTIN */

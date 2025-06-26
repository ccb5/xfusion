/**
 * @file xf_porting.h
 *
 * Configuration file v1.0.0 for xtiny vx.x.x
 *
 * @note 此文件不独立存在，禁止直接使用，需配合 `xf_conf.h` 使用。
 *       如果 XF_ENABLE_PORTING 设为 1，则会在 xf_conf_internal.h 中
 *       包含 XF_PORTING_PATH 。因此 `xf_conf.h` 内定义的宏可以直接使用。
 *
 * @note 此文件主要用于编写 Kconfig 无法覆盖的配置，例如：带参宏。
 */

/*
 * Copy this file as `xf_porting.h`
 * 1. simply next to `xf_conf.h`
 * 2. or to any other place and
 *    - define `XF_CONF_INCLUDE_SIMPLE`;
 *    - add the path as an include path.
 */

/* clang-format off */
#if 0 /* Set this to "1" to enable content */

#ifndef __XF_PORTING_H__
#define __XF_PORTING_H__

/* If you need to include anything here, do it inside the `__ASSEMBLY__` guard */
#if  0
#include "my_include.h"
#endif

/* -------------------- components ------------------------------------------ */

/* -------------------- components/log -------------------------------------- */

#if XF_LOG_ENABLE_CUSTOM_PORTING
__IMPL void xf_log_printf(const char *format, ...);
__IMPL void xf_log_level(char level, const char *tag, const char *format, ...);
#endif /* XF_LOG_ENABLE_CUSTOM_PORTING */

/* -------------------- components/system/safe ------------------------------ */

#define XF_CRIT_STAT()                      ((void)0)
#define XF_CRIT_ENTRY()                     ((void)0)
#define XF_CRIT_EXIT()                      ((void)0)

#endif /* __XF_PORTING_H__ */

#endif /* End of "Content enable" */

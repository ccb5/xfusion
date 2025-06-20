/**
 * @file xf_compiler.h
 * @author catcatBlue (catcatblue@qq.com)
 * @brief
 * @version 1.0
 * @date 2025-05-14
 */

/*
 * SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __XF_COMPILER_H__
#define __XF_COMPILER_H__

/* ==================== [Includes] ========================================== */

#include "xf_conf_internal.h"
#include "xf_types.h"
#include "xf_macro_definition.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

/**
 * @note 参考:
 * - https://gcc.gnu.org/onlinedocs/gcc/Common-Variable-Attributes.html
 * - /usr/include/x86_64-linux-gnu/sys/cdefs.h
 */

#if (XF_COM_USE_GNU)

#if !defined(__weak)
/**
 * @brief 弱声明符号.
 * 被 __weak 修饰的符号（变量或函数）
 * 可以被 **没有** 用 __weak 修饰的符号覆盖或重定义.
 */
#   define __weak                       __attribute__((weak))
#endif

#if !defined(__used)
/**
 * @brief 编辑器不会优化掉被 __used 修饰的符号, 即使在代码中即使没有使用到该符号.
 */
#   define __used                       __attribute__((__used__))
#endif

#if !defined(__const)
/**
 * @brief 告诉编译器该值是常量.
 */
#   define __const                      __attribute__((__const__))
#endif

#if !defined(__deprecated)
/**
 * @brief 标注已弃用的函数.
 */
#   define __deprecated                 __attribute__((__deprecated__))
#endif

#if !defined(__aligned)
/**
 * @brief 指定变量或结构体的对齐方式（单位字节）.
 */
#   define __aligned(x)                 __attribute__((aligned(x)))
#endif

#if !defined(__packed)
/**
 * @brief 取消结构在编译过程中的优化对齐,
 *        按照实际占用字节数进行对齐.
 */
#   define __packed                     __attribute__((packed))
#endif

#if !defined(struct_packed)
#   define struct_packed                struct __packed
#endif

#if !defined(union_packed)
#   define union_packed                 union __packed
#endif

#if !defined(enum_packed)
#   define enum_packed                  enum __packed
#endif

#if !defined(__section)
/**
 * @brief 指定函数或变量的段位置, 如 data 或 bss.
 */
#   define __section(x)                 __attribute__((section(x)))
#endif

#if !defined(__always_inline)
/**
 * @brief 强制内联.
 */
#   define __always_inline              inline __attribute__((__always_inline__))
#endif

#if !defined(__noinline)
/**
 * @brief 不要内联.
 */
#   define __noinline                   __attribute__((__noinline__))
#endif

#if !defined(__optimize)
/**
 * @brief 修改函数优化等级.
 *
 * @param level 优化等级.
 */
#   define __optimize(level)            __attribute__((__optimize__(level)))
#endif

#if !defined(__alias)
/**
 * @brief 指定新名称.
 */
#   define __alias(x)                   __attribute__((alias(x)))
#endif

#if !defined(__compiletime_warning)
/**
 * @brief 编译时警告.
 *
 * @param message 警告消息.
 */
#   define __compiletime_warning(message)   __attribute__((warning(message)))
#endif

#if !defined(__compiletime_error)
/**
 * @brief 编译时报错.
 *
 * @param message 报错消息.
 */
#   define __compiletime_error(message)     __attribute__((error(message)))
#endif

#if XF_COM_USE_BUILTIN
#if !defined(likely)
/**
 * @brief 分支预测, x 为真的可能性更大.
 */
#   define likely(x)                    __builtin_expect(!!(x), 1)
#endif
#if !defined(unlikely)
/**
 * @brief 分支预测, x 为假的可能性更大.
 */
#   define unlikely(x)                  __builtin_expect(!!(x), 0)
#endif
#else /* !XF_COM_USE_BUILTIN */
#if !defined(likely)
#   define likely(x)                    (x)
#endif
#if !defined(unlikely)
#   define unlikely(x)                  (x)
#endif
#endif /* XF_COM_USE_BUILTIN */

#if !defined(__format_attribute)
/**
 * @brief 格式化字符串检查.
 */
#if defined(PYCPARSER) || defined(__CC_ARM)
#   define __format_attribute(fmtstr, vararg)
#elif defined(__GNUC__) && ((__GNUC__ == 4 && __GNUC_MINOR__ >= 4) || __GNUC__ > 4)
#   define __format_attribute(fmtstr, vararg) __attribute__((format(gnu_printf, fmtstr, vararg)))
#elif (defined(__clang__) || defined(__GNUC__) || defined(__GNUG__))
#   define __format_attribute(fmtstr, vararg) __attribute__((format(printf, fmtstr, vararg)))
#else
#   define __format_attribute(fmtstr, vararg)
#endif
#endif

#define XF_READ_ONCE(ptr_expr) \
        ((void *)(*(volatile void * const *)&(ptr_expr)))

#define XF_WRITE_ONCE(ptr_lvalue, new_val) \
        ( (void)(*(volatile void **)&(ptr_lvalue) = (void *)(new_val)), \
          (void *)(new_val) )

#define XF_READ_ONCE_T(type, lvalue) \
        (*(volatile type const *)&(lvalue))

#define XF_WRITE_ONCE_T(type, lvalue, new_val) \
        ( (void)(*(volatile type *)&(lvalue) = (new_val)), \
          (new_val) )

#if !defined(__fallthrough)
#   define __fallthrough                __attribute__((fallthrough));
#endif

#else /* !XF_COM_USE_GNU */

#if !defined(__weak)
#   define __weak
#endif

#if !defined(__used)
#   define __used
#endif

#if !defined(__const)
#   define __const
#endif

#if !defined(__deprecated)
#   define __deprecated
#endif

#if !defined(__aligned)
#   define __aligned(x)
#endif

#if !defined(__packed)
#   define __packed
#endif

#if !defined(__section)
#   define __section(x)
#endif

#if !defined(__always_inline)
#   define __always_inline
#endif

#if !defined(__noinline)
#   define __noinline
#endif

#if !defined(__optimize)
#   define __optimize(level)
#endif

#if !defined(__alias)
#   define __alias(x)
#endif

#if !defined(__compiletime_warning)
#   define __compiletime_warning(message)
#endif

#if !defined(__compiletime_error)
#   define __compiletime_error(message)
#endif

#if !defined(likely)
#   define likely(x)                    (x)
#endif

#if !defined(unlikely)
#   define unlikely(x)                  (x)
#endif

#if !defined(__format_attribute)
#   define __format_attribute(fmtstr, vararg)
#endif

#if !defined(__fallthrough)
#   define __fallthrough
#endif

#endif /* XF_COM_USE_GNU */

#if !defined(XF_ATTR_FAST_MEM)
#   define XF_ATTR_FAST_MEM
#endif

/* TODO 其他编译器情况 */

/* ==================== [Global Prototypes] ================================= */

/* ==================== [Macros] ============================================ */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* __XF_COMPILER_H__ */

/**
 * @file xf_macro_definition.h
 * @author catcatBlue (catcatblue@qq.com)
 * @brief
 * @version 1.0
 * @date 2025-05-14
 *
 * SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#ifndef __XF_MACRO_DEFINITION_H__
#define __XF_MACRO_DEFINITION_H__

/* ==================== [Includes] ========================================== */

#include "xf_common_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

#if !defined(__INLINE)
#   define __INLINE                     inline
#endif

#if !defined(__STATIC_INLINE)
#   define __STATIC_INLINE              static inline
#endif

#if !defined(__EXT_IMPL)
#   define __EXT_IMPL                   /*!< 仅用于标记不在此处实现 */
#endif

#if !defined(__IMPL)
#   define __IMPL                       /*!< 仅用于标记在此处实现 */
#endif

#if !defined(NULL)
/**
 * @brief 空指针.
 */
#   define NULL                         ((void *)0)
#endif

#if !defined(XF_WORD_SIZE)
/**
 * @brief 平台字长.
 */
#   define XF_WORD_SIZE                 (sizeof(void *) * 8)
#endif

#if defined(__INTELLISENSE__)
/**
 * @brief 防止 vscode 的 c/c++ 插件误报错。
 */
#   define __FILENAME__                 (__FILE__)
#endif

#if !defined(__FILENAME__) && XF_COMMON_ENABLE_BUILTIN
/**
 * @brief 获取不含路径的文件名.
 */
#   define __FILENAME__                 (__builtin_strrchr(__FILE__, '/') \
                                            ? (__builtin_strrchr(__FILE__, '/') + 1) \
                                            : ((__builtin_strrchr(__FILE__, '\\') \
                                                    ? (__builtin_strrchr(__FILE__, '\\') + 1) \
                                                    : (__FILE__))\
                                              ) \
                                        )
#else
#   define __FILENAME__                 __FILE__
#endif

/* ==================== [Typedefs] ========================================== */

/* ==================== [Global Prototypes] ================================= */

/* ==================== [Macros] ============================================ */

#if !defined(UNUSED)
/**
 * @brief 未使用的变量通过 UNUSED 防止编译器警告.
 */
#   define UNUSED(x)                    (void)(x)
#endif

#if !defined(CAT2)
/**
 * @brief 拼接.
 */
#   define CAT2(a, b)                   a##b
#endif

#if !defined(CAT3)
/**
 * @brief 拼接 3 个参数.
 */
#   define CAT3(a, b, c)                a##b##c
#endif

#if !defined(XCAT2)
/**
 * @brief 展开拼接.
 */
#   define XCAT2(a, b)                  CAT2(a, b)
#endif

#if !defined(XCAT3)
/**
 * @brief 展开拼接 3 个参数.
 */
#   define XCAT3(a, b, c)               CAT3(a, b, c)
#endif

#if !defined(CONCAT2)
/**
 * @brief 拼接.
 */
#   define CONCAT2(a, b)                a##b
#endif

#if !defined(CONCAT3)
/**
 * @brief 拼接 3 个参数.
 */
#   define CONCAT3(a, b, c)             a##b##c
#endif

#if !defined(XCONCAT2)
/**
 * @brief 展开拼接.
 */
#   define XCONCAT2(a, b)               CONCAT2(a, b)
#endif

#if !defined(XCONCAT3)
/**
 * @brief 展开拼接 3 个参数.
 */
#   define XCONCAT3(a, b, c)            CONCAT3(a, b, c)
#endif

#if !defined(STR)
/**
 * @brief 字符串化.
 * 见 @see https://gcc.gnu.org/onlinedocs/gcc-3.4.3/cpp/Stringification.html
 *
 * @example
 * ```
 * #define foo 4
 * STR (foo)
 *     ==> "foo"
 * ```
 */
#   define STR(x)                       #x
#endif

#if !defined(XSTR)
/**
 * @brief 参数字符串化.
 *
 * @example
 * ```
 * #define foo 4
 * XSTR (foo)
 *     ==> XSTR (4)
 *     ==> STR (4)
 *     ==> "4"
 * ```
 */
#   define XSTR(x)                      STR(x)
#endif

/**
 * @brief 将版本号转换为整数的宏。
 */
#define XF_VERSION_VAL(major, minor, patch) \
    (((major) << 16) | ((minor) << 8) | (patch))

/**
 * @brief 当前版本号, 返回一个整数。
 * 编译时使用。比如：XF_VERSION >= XF_VERSION_VAL(1, 0, 0)
 */
#define XF_VERSION \
    XF_VERSION_VAL(XF_VERSION_MAJOR, XF_VERSION_MINOR, XF_VERSION_PATCH)

/**
 * @brief 当前版本号, 返回一个字符串。
 */
#define XF_VERSION_STR \
    "v" XSTR(XF_VERSION_MAJOR) "." XSTR(XF_VERSION_MINOR) "." XSTR(XF_VERSION_PATCH)

#if !defined(XF_CAST)
/**
 * @brief 强制类型转换.
 *
 * @param t 目标类型.
 * @param e 表达式.
 */
#   define XF_CAST(t, e)                ((t)(e))
#endif

#if !defined(XF_DEREF)
/**
 * @brief 将输入指针 ptr 解引用为类型 t.
 *
 * @param t 目标类型.
 * @param ptr 输入指针.
 */
#   define XF_DEREF(t, ptr)             (*(t *)(ptr))
#endif

#if !defined(XF_READ_ONCE)
#   define XF_READ_ONCE(ptr_expr)       ((void *)(*(volatile void * const *)&(ptr_expr)))
#endif

#if !defined(XF_WRITE_ONCE)
#   define XF_WRITE_ONCE(ptr_lvalue, new_val) \
                                        ( (void)(*(volatile void **)&(ptr_lvalue) = (void *)(new_val)), \
                                          (void *)(new_val) )
#endif

#if !defined(XF_READ_ONCE_T)
#   define XF_READ_ONCE_T(type, lvalue) (*(volatile type const *)&(lvalue))

#endif

#if !defined(XF_WRITE_ONCE_T)
#   define XF_WRITE_ONCE_T(type, lvalue, new_val) \
                                        ( (void)(*(volatile type *)&(lvalue) = (new_val)), \
                                          (void *)(new_val) )
#endif

#if !defined(STATIC_ASSERT)
/**
 * @brief 静态断言。
 *
 * @param e 需要断言的常量表达式(const expression)。
 *          当 e 不成立时编译报错。必须是字面常量。
 */
#   define STATIC_ASSERT(e) \
        extern char ( \
            *O_o( \
                char \
static_assertion_failure \
                    [1 - 2 * !(e)] \
                ) \
            )
#endif

#if !defined(STATIC_ASSERT_MSG)
/**
 * @brief 带有提示消息的静态断言。
 *
 * @param e 需要断言的常量表达式(const expression)。
 *          当 e 不成立时编译报错。必须是字面常量。
 * @param msg 提示消息。
 */
#define STATIC_ASSERT_MSG(e, msg) \
        extern char ( \
            *O_o( \
                char \
static_assertion_failure \
                    [1 - 2 * !(e)] \
                ) \
            )
#endif

#if !defined(BUILD_BUG_ON)
/**
 * @brief BUILD_BUG_ON - 条件为真，则停止编译。
 *
 * 如果代码依赖于某些常量相等或其他编译时评估的条件，应该使用 BUILD_BUG_ON 来检测是否有人修改了它。
 *
 * @param condition 编译器应知道为假的条件。
 */
#   define BUILD_BUG_ON(condition)      ((void)sizeof(char[1 - 2 * !!(condition)]))
#endif

#if !defined(BUILD_BUG_ON_ZERO)
/**
 * @brief BUILD_BUG_ON_ZERO - 条件为真，则停止编译；条件为假时返回 0。
 *
 * 条件为真时产生编译错误，同时产生一个结果（值为 0），
 * 可用于结构初始化器等（或其他不允许使用逗号表达式的地方）。
 *
 * @param condition 条件。
 */
#   define BUILD_BUG_ON_ZERO(condition) (!sizeof(char[1 - 2 * !!(condition)]))
#endif

#if !defined(BUILD_BUG_ON_NOT_POWER_OF_2)
/**
 * @brief 如果常量表达式不是 2 的幂，则强制出现编译错误。
 */
#   define BUILD_BUG_ON_NOT_POWER_OF_2(n) \
                                        BUILD_BUG_ON(IS_POWER_OF_TWO(n))
#endif

#if !defined(xf_offsetof)
/**
 * @brief offsetof - 返回结构成员相对于结构开头的字节偏移量.
 *
 * @param type 结构体类型.
 * @param member 结构中成员的名称.
 *
 * @return 结构成员相对于结构开头的字节偏移量.
 */
#   define xf_offsetof(type, member)    ((size_t)&((type *)0)->member)
#endif

#if !defined(xf_container_of)
/**
 * @brief container_of - 通过结构体成员变量地址获取结构体的地址.
 *
 * @param ptr 指向成员的指针.
 * @param type 结构体类型.
 * @param member 结构中成员的名称.
 *
 * @return 结构体的地址.
 *
 * 示例如下：
 * @code{c}
 * typedef struct {
 *     int member1;
 *     char member2;
 *     float member3;
 * } struct_test_t;
 *
 * int main()
 * {
 *     struct_test_t stru = {
 *         .member1 = 0x12345678,
 *         .member2 = 0xAB,
 *         .member3 = -123.456f,
 *     };
 *     char *p_member2 = &stru.member2;
 *     struct_test_t *p_stru = xf_container_of(p_member2, struct_test_t, member2);
 *     if ((p_stru->member1 != (int)0x12345678)
 *             || (p_stru->member2 != (char)0xAB)
 *             || ((p_stru->member3 - (-123.456f)) > 1e-6f)) {
 *         printf("xf_container_of error!\n");
 *     } else {
 *         printf("xf_container_of ok!\n");
 *     }
 * }
 * @endcode
 */
#   define xf_container_of(ptr, type, member) \
                                        ((type *)((char *)(ptr) - xf_offsetof(type, member)))
#endif

#if !defined(ARRAY_SIZE)
/**
 * @brief ARRAY_SIZE - 获取数组 arr 中的元素数量。
 *
 * @param arr 待求的数组。
 *
 * @return 数组元素个数。
 */
#   define ARRAY_SIZE(arr)              (sizeof(arr) / sizeof((arr)[0]))
#endif

#if !defined(__ALIGN_MASK)
#   define __ALIGN_MASK(x, mask)        (((x) + (mask)) & ~(mask))
#endif

#if !defined(ALIGN)
/**
 * @brief 将 x 向上对齐（增加）到 a 最接近（较大）倍数.
 *
 * @param x 需要被对齐的数.
 * @param a 必须是 2 的幂.
 *
 * @return 已对齐（增加）的数.
 */
#   define ALIGN(x, a)                  __ALIGN_MASK(x, (a) - 1)
#endif

#if !defined(ALIGN_DOWN)
/**
 * @brief 将 x 向下对齐（减少）到 a 最接近（较小）倍数.
 *
 * @param x 需要被对齐的数.
 * @param a 必须是 2 的幂.
 *
 * @return 已对齐（减少）的数.
 */
#   define ALIGN_DOWN(x, a)             ALIGN((x) - ((a) - 1), (a))
#endif

#if !defined(PTR_ALIGN)
/**
 * @brief 将指针向上对齐到 a.
 *
 * @param p 需要被对齐的指针.
 * @param a 必须是 2 的幂.
 *
 * @return 已对齐的指针.
 */
#   define PTR_ALIGN(p, a)              ALIGN((uintptr_t)(p), (a))
#endif

#if !defined(IS_ALIGNED)
/**
 * @brief 检查 x 是否对齐到 a.
 *
 * @param x 需要被检查的数.
 * @param a 必须是 2 的幂.
 *
 * @return 如果 x 对齐到 a，则返回 1，否则返回 0.
 */
#   define IS_ALIGNED(x, a)             (((x) & ((a) - 1)) == 0)
#endif

#if !defined(ALIGN_NUM)
/**
 * @brief 将 x 向上对齐到正整数 n.
 *
 * @param x 需要被对齐的数.
 * @param n 必须为正整数.
 *
 * @return 已对齐的数.
 */
#   define ALIGN_NUM(x, n)              ((!n) ? (0) : (((x) - 1) + (n) - (((x) - 1) % (n))))
#endif

#if !defined(IS_POWER_OF_TWO)
/**
 * @brief 检查整数是否为 2 的幂。
 *
 * @param x 待检查的整数.
 *
 * @return 如果 x 是 2 的幂，则返回 1，否则返回 0.
 */
#   define IS_POWER_OF_TWO(x)           (((x) != 0) && ((((x) - 1) & (x)) == 0))
#endif

/**
 * @brief 算数相关.
 */

#if !defined(xf_swap)
/**
 * @brief 交换 a 和 b 的值.
 */
#   define xf_swap(a, b)                do { /* 只是为了不使用新类型 */ \
                                            if ((a) != (b)) { \
                                                (a) ^= (b); (b) ^= (a); (a) ^= (b); \
                                            } \
                                        } while (0)
#endif

#if !defined(xf_swap_t)
/**
 * @brief 交换 a 和 b 的值.
 */
#   define xf_swap_t(type, a, b)        do { \
                                            type __tmp = (a); \
                                            (a) = (b); \
                                            (b) = __tmp; \
                                        } while (0)
#endif

#if !defined(xf_abs)
/**
 * @brief abs - 返回参数的绝对值
 *
 * @return x 的绝对值.
 */
#   define xf_abs(x)                    ((x) < 0 ? -(x) : (x))
#endif

#if !defined(xf_max)
/**
 * @brief 求最大值.
 *
 * @return x 和 y 中较大者.
 */
#   define xf_max(x, y)                 ((x) > (y) ? (x) : (y))
#endif

#if !defined(xf_min)
/**
 * @brief 求最小值.
 *
 * @return x 和 y 中较小者.
 */
#   define xf_min(x, y)                 ((x) < (y) ? (x) : (y))
#endif

#if !defined(xf_div_round_up)
/**
 * @brief 向上取整.
 *
 * @param n 被除数.
 * @param d 除数.
 *
 * @return 向上取整结果.
 *
 * @example
 * xf_div_round_up(100, 3) == 34
 * xf_div_round_up(100, 4) == 25
 * xf_div_round_up(100, 6) == 17
 */
#   define xf_div_round_up(n, d)        (((n) + (d) - 1) / (d))
#endif

#if !defined(xf_div_round_down)
/**
 * @brief 向下取整.
 *
 * @param n 被除数.
 * @param d 除数.
 *
 * @return 向下取整结果.
 *
 * @example
 * xf_div_round_down(100, 3) == 33
 * xf_div_round_down(100, 4) == 25
 * xf_div_round_down(100, 6) == 16
 */
#   define xf_div_round_down(n, d)      ((n) / (d))
#endif

#if !defined(xf_roundup)
/**
 * @brief 向上圆整.
 *
 * @param x 被除数.
 * @param y 除数.
 *
 * @return 向上圆整结果.
 *
 * @example
 * roundup(100, 3) == 102 ///!< 100 不能被 3 整除, 100 以上能被 3 整除的最小数是 102
 * roundup(100, 4) == 100 ///!< 100 能被 4 整除
 * roundup(100, 7) == 105 ///!< 100 不能被 7 整除, 100 以上能被 7 整除的最小数是 105
 */
#   define xf_roundup(x, y)             ((((x) + ((y) - 1)) / (y)) * (y))
#endif

#if !defined(xf_rounddown)
/**
 * @brief 向下圆整.
 *
 * @param x 被除数.
 * @param y 除数.
 *
 * @return 向下圆整结果.
 *
 * @example
 * rounddown(100, 3) == 99   ///!< 100 不能被 3 整除, 100 以下能被 3 整除的最大数是 99
 * rounddown(100, 4) == 100  ///!< 100 能被 4 整除
 * rounddown(100, 7) == 98   ///!< 100 不能被 7 整除, 100 以下能被 7 整除的最大数是 98
 */
#   define xf_rounddown(x, y)           ((x) - ((x) % (y)))
#endif

#if !defined(xf_div_round_closest)
/**
 * @brief 四舍五入到整数.
 *
 * 整型被除数除以整型被除数, 四舍五入到最接近的整数.
 * 如果除数变量类型为无符号, 则对负除数的结果是未定义的,
 * 如果除数变量类型为无符号, 则对负除数的结果是未定义的.
 *
 * @param x 被除数.
 * @param divisor 除数.
 *
 * @return 四舍五入的整数结果.
 */
#   define xf_div_round_closest(x, divisor) \
                                        ( \
                                            ((((x) >= 0) && ((divisor) >= 0)) \
                                                    || (((x) < 0) && ((divisor) < 0))) \
                                                ? (((x) + ((divisor) >> 1)) / (divisor)) \
                                                : (((x) - ((divisor) >> 1)) / (divisor)) \
                                        )
#endif

#if !defined(xf_mult_frac)
/**
 * @brief 将整数乘以分数.
 *
 * 即为: x * (numer / denom)
 * 通过此宏可以避免不必要的溢出或精度损失.
 *
 * @param x 被分数乘的整数.
 * @param numer 分数的分子.
 * @param denom 分数的分母.
 * @return 整数乘与分数相乘结果
 */
#   define xf_mult_frac(x, numer, denom) \
                                        ((((x) / (denom)) * (numer)) \
                                            + ((((x) % (denom)) * (numer)) / (denom)))
#endif

#if !defined(xf_clamp)
/**
 * @brief 将值限制在给定范围内.
 *
 * @param val 要钳位的值.
 * @param low 最低允许值（含）.
 * @param high 最高允许值（含）.
 * @return 限制值
 */
#   define xf_clamp(val, low, high)     (((val) <= (low)) ? (low) : xf_min(val, high))
#endif

#if !defined(xf_in_range)
/**
 * @brief 检查值是否在范围内.
 *
 * @param val 要检查的值.
 * @param min 下限（包含）.
 * @param max 上限（包含）.
 *
 * @return true 在范围内
 * @return false 不在范围内
 */
#   define xf_in_range(val, min, max)   ((val) >= (min) && (val) <= (max))
#endif

/**
 * @brief 去除括号.
 *
 * @code
 *  #define TEST_DEBRACKET(_1, _2, _3)  _1 = _2 XF_DEBRACKET _3
 *  TEST_DEBRACKET(int a, 0, (;))
 *      ->     _1 = _2 XF_DEBRACKET _3
 *      ->  int a =  0 XF_DEBRACKET (;)
 *      ->  int a =  0 ;
 * @endcode
 */
#define XF_DEBRACKET(...)               __VA_ARGS__
#define XF__EVAL(...)                   __VA_ARGS__
#define XF_EMPTY(...)

#if !defined(XF_VA_ARGS_NUM_LESS_1) && !defined(XF_VA_ARGS_NUM_EVAL_LESS_1) && !defined(XF_VA_ARGS_NUM_IMPL_LESS_1)

/**
 * @brief 获取参数的数量.
 *
 * @param ... 参数列表.
 *
 * @return 参数列表中的可变参数数量.
 */
#define XF_VA_ARGS_NUM(...)                 XF_VA_ARGS_NUM_LESS_1(ignore_this, ##__VA_ARGS__)

/**
 * @brief 获取参数的数量, 减 1 个.
 *
 * @param ... 参数列表.
 *
 * @return 参数列表中的可变参数数量, 减一.
 */
#define XF_VA_ARGS_NUM_LESS_1(...)          XF_VA_ARGS_NUM_EVAL_LESS_1(__VA_ARGS__)
#define XF_VA_ARGS_NUM_EVAL_LESS_1(...)     XF_VA_ARGS_NUM_IMPL_LESS_1(__VA_ARGS__, \
        255, 254, 253, 252, 251, 250, 249, 248, 247, 246, 245, 244, 243, 242, 241, 240, \
        239, 238, 237, 236, 235, 234, 233, 232, 231, 230, 229, 228, 227, 226, 225, 224, \
        223, 222, 221, 220, 219, 218, 217, 216, 215, 214, 213, 212, 211, 210, 209, 208, \
        207, 206, 205, 204, 203, 202, 201, 200, 199, 198, 197, 196, 195, 194, 193, 192, \
        191, 190, 189, 188, 187, 186, 185, 184, 183, 182, 181, 180, 179, 178, 177, 176, \
        175, 174, 173, 172, 171, 170, 169, 168, 167, 166, 165, 164, 163, 162, 161, 160, \
        159, 158, 157, 156, 155, 154, 153, 152, 151, 150, 149, 148, 147, 146, 145, 144, \
        143, 142, 141, 140, 139, 138, 137, 136, 135, 134, 133, 132, 131, 130, 129, 128, \
        127, 126, 125, 124, 123, 122, 121, 120, 119, 118, 117, 116, 115, 114, 113, 112, \
        111, 110, 109, 108, 107, 106, 105, 104, 103, 102, 101, 100,  99,  98,  97,  96, \
         95,  94,  93,  92,  91,  90,  89,  88,  87,  86,  85,  84,  83,  82,  81,  80, \
         79,  78,  77,  76,  75,  74,  73,  72,  71,  70,  69,  68,  67,  66,  65,  64, \
         63,  62,  61,  60,  59,  58,  57,  56,  55,  54,  53,  52,  51,  50,  49,  48, \
         47,  46,  45,  44,  43,  42,  41,  40,  39,  38,  37,  36,  35,  34,  33,  32, \
         31,  30,  29,  28,  27,  26,  25,  24,  23,  22,  21,  20,  19,  18,  17,  16, \
         15,  14,  13,  12,  11,  10,   9,   8,   7,   6,   5,   4,   3,   2,   1,   0)

/**
 * @brief 辅助宏. 获取参数的数量.
 */
#define XF_VA_ARGS_NUM_IMPL_LESS_1( \
          _0,   _1,   _2,   _3,   _4,   _5,   _6,   _7, \
          _8,   _9,  _10,  _11,  _12,  _13,  _14,  _15, \
         _16,  _17,  _18,  _19,  _20,  _21,  _22,  _23, \
         _24,  _25,  _26,  _27,  _28,  _29,  _30,  _31, \
         _32,  _33,  _34,  _35,  _36,  _37,  _38,  _39, \
         _40,  _41,  _42,  _43,  _44,  _45,  _46,  _47, \
         _48,  _49,  _50,  _51,  _52,  _53,  _54,  _55, \
         _56,  _57,  _58,  _59,  _60,  _61,  _62,  _63, \
         _64,  _65,  _66,  _67,  _68,  _69,  _70,  _71, \
         _72,  _73,  _74,  _75,  _76,  _77,  _78,  _79, \
         _80,  _81,  _82,  _83,  _84,  _85,  _86,  _87, \
         _88,  _89,  _90,  _91,  _92,  _93,  _94,  _95, \
         _96,  _97,  _98,  _99, _100, _101, _102, _103, \
        _104, _105, _106, _107, _108, _109, _110, _111, \
        _112, _113, _114, _115, _116, _117, _118, _119, \
        _120, _121, _122, _123, _124, _125, _126, _127, \
        _128, _129, _130, _131, _132, _133, _134, _135, \
        _136, _137, _138, _139, _140, _141, _142, _143, \
        _144, _145, _146, _147, _148, _149, _150, _151, \
        _152, _153, _154, _155, _156, _157, _158, _159, \
        _160, _161, _162, _163, _164, _165, _166, _167, \
        _168, _169, _170, _171, _172, _173, _174, _175, \
        _176, _177, _178, _179, _180, _181, _182, _183, \
        _184, _185, _186, _187, _188, _189, _190, _191, \
        _192, _193, _194, _195, _196, _197, _198, _199, \
        _200, _201, _202, _203, _204, _205, _206, _207, \
        _208, _209, _210, _211, _212, _213, _214, _215, \
        _216, _217, _218, _219, _220, _221, _222, _223, \
        _224, _225, _226, _227, _228, _229, _230, _231, \
        _232, _233, _234, _235, _236, _237, _238, _239, \
        _240, _241, _242, _243, _244, _245, _246, _247, \
        _248, _249, _250, _251, _252, _253, _254, _255, \
        N, ...) N
#endif /* !defined(XF_VA_ARGS_NUM) && !defined(XF_VA_ARGS_NUM_EVAL) && !defined(XF_VA_ARGS_NUM_IMPL) */

/* ------------------------------------------------------------------------- */

/*
 * Created by William Swanson in 2012.
 *
 * I, William Swanson, dedicate this work to the public domain.
 * I waive all rights to the work worldwide under copyright law,
 * including all related and neighboring rights,
 * to the extent allowed by law.
 *
 * You can copy, modify, distribute and perform the work,
 * even for commercial purposes, all without asking permission.
 *
 * @ref https://github.com/swansontec/map-macro
 * @ref https://stackoverflow.com/questions/6707148/foreach-macro-on-macros-arguments/13459454#13459454
 * @ref https://www.cnblogs.com/shangdawei/archive/2013/05/27/3100889.html
 */

#define XF_EVAL0(...) __VA_ARGS__
#define XF_EVAL1(...) XF_EVAL0 (XF_EVAL0 (XF_EVAL0 (__VA_ARGS__)))
#define XF_EVAL2(...) XF_EVAL1 (XF_EVAL1 (XF_EVAL1 (__VA_ARGS__)))
#define XF_EVAL3(...) XF_EVAL2 (XF_EVAL2 (XF_EVAL2 (__VA_ARGS__)))
#define XF_EVAL4(...) XF_EVAL3 (XF_EVAL3 (XF_EVAL3 (__VA_ARGS__)))
#define XF_EVAL(...)  XF_EVAL4 (XF_EVAL4 (XF_EVAL4 (__VA_ARGS__)))

#define XF_MACRO_MAP_END(...)
#define XF_MACRO_MAP_OUT
#define XF_MACRO_MAP_COMMA ,

#define XF_MACRO_MAP_GET_END2()             0, XF_MACRO_MAP_END
#define XF_MACRO_MAP_GET_END1(...)          XF_MACRO_MAP_GET_END2
#define XF_MACRO_MAP_GET_END(...)           XF_MACRO_MAP_GET_END1
#define XF_MACRO_MAP_NEXT0(test, next, ...) next XF_MACRO_MAP_OUT
#define XF_MACRO_MAP_NEXT1(test, next)      XF_MACRO_MAP_NEXT0(test, next, 0)
#define XF_MACRO_MAP_NEXT(test, next)       XF_MACRO_MAP_NEXT1(XF_MACRO_MAP_GET_END test, next)

#define XF_MACRO_MAP0(f, sep, x, peek, ...) f(x) XF_DEBRACKET sep XF_MACRO_MAP_NEXT(peek, XF_MACRO_MAP1) (f, sep, peek, __VA_ARGS__)
#define XF_MACRO_MAP1(f, sep, x, peek, ...) f(x) XF_DEBRACKET sep XF_MACRO_MAP_NEXT(peek, XF_MACRO_MAP0) (f, sep, peek, __VA_ARGS__)

#define XF_MACRO_MAP(f, sep, ...)           XF_EVAL(XF_MACRO_MAP1(f, sep, __VA_ARGS__, ()()(), ()()(), ()()(), 0))
#define XF_FOR_EACH(f, sep, ...)            XF_EVAL(XF_MACRO_MAP1(f, sep, __VA_ARGS__, ()()(), ()()(), ()()(), 0))

/* ------------------------------------------------------------------------- */

#if !defined(KB) && defined(__LINKER__)
#   define KB(x)                        ((x) << 10)
#elif !defined(KB) && !defined(__LINKER__)
/*
    TODO size_t 于 xf_types.h 包含.
 */
#   define KB(x)                        (((size_t)x) << 10)
#endif
#if !defined(MB)
#   define MB(x)                        (KB(x) << 10)
#endif
#if !defined(GB)
#   define GB(x)                        (MB(x) << 10)
#endif

#if !defined(KHZ)
#   define KHZ(x)                       ((x) * 1000)
#endif
#if !defined(MHZ)
#   define MHZ(x)                       (KHZ(x) * 1000)
#endif

#if !defined(XF_WAIT_FOR)
/**
 * @brief 等待条件满足。
 *
 * @param[in] _condition 要满足的条件。
 * @param[in] _attempts 条件检查的最大次数。
 * @param[in] _delay_num 连续检查之间的延迟，单位由 `XF_WAIT_DELAY` 宏决定。
 * @param[out] _result 布尔变量，用于存储等待过程的结果。
 *                     如果满足条件则设置为 true，否则设置为 false。
 */
#   define XF_WAIT_FOR(_condition, _attempts, _delay_num, _result) \
        do { \
            uint32_t remaining_attempts = (_attempts); \
            if (!(_attempts)) { break; } \
            (_result) = false; \
            do { \
                if (_condition) { \
                    (_result) = true; \
                    break; \
                } \
                XF_WAIT_DELAY(_delay_num); \
            } while (--remaining_attempts); \
        } while (0)
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __XF_MACRO_DEFINITION_H__ */

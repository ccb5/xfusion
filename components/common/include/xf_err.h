/**
 * @file xf_err.h
 * @author catcatBlue (catcatblue@qq.com)
 * @brief 错误码及错误类型定义.
 * @version 1.0
 * @date 2025-05-14
 *
 * SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#ifndef __XF_ERR_H__
#define __XF_ERR_H__

/* ==================== [Includes] ========================================== */

#include "xf_common_internal.h"
#include "xf_types.h"
#include "xf_compiler.h"
#include "xf_bit.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

/**
 * @brief 通用错误码。
 * @attention 不要直接使用此错误码，使用 @ref xf_err_t 下方定义的.
 */
typedef enum_packed xf_err_common_code {
    _XF_ERR_COM_FAIL = 0x01,            /*!< 通用错误 */
    _XF_ERR_COM_NO_MEM,                 /*!< 内存不足 */
    _XF_ERR_COM_INVALID_ARG,            /*!< 无效参数 */
    _XF_ERR_COM_INVALID_STATE,          /*!< 无效状态 */
    _XF_ERR_COM_INVALID_CHECK,          /*!< 无效校验（校验发生错误） */
    _XF_ERR_COM_INVALID_PORT,           /*!< 对接的方法无效 */
    _XF_ERR_COM_NOT_FOUND,              /*!< 未找到 */
    _XF_ERR_COM_NOT_SUPPORTED,          /*!< 不支持 */
    _XF_ERR_COM_BUSY,                   /*!< 正忙 */
    _XF_ERR_COM_TIMEOUT,                /*!< 超时 */
    _XF_ERR_COM_UNINIT,                 /*!< 未初始化 */
    _XF_ERR_COM_INITED,                 /*!< 已初始化 */
    _XF_ERR_COM_RESOURCE,               /*!< 资源不可用 */
    _XF_ERR_COM_ISR,                    /*!< 中断调用错误 */
    _XF_ERR_COM_IOP,                    /*!< 非法操作 IOP Illegal OPeration */
    _XF_ERR_COM_MAX,                    /*!< 错误类型最大值 */
} xf_err_comm_code_t;

/* ==================== [Typedefs] ========================================== */

/**
 * @brief 整形错误类型。
 * 错误码具体值见 @ref xf_err_code_t.
 */
typedef int32_t xf_err_t;
typedef int32_t xf_ret_t;
#define XF_OK                           (0)
/* 符号为负号时为错误情况，其他情况视为正常数据。 */
#define XF_FAIL                         (-_XF_ERR_COM_FAIL)
#define XF_ERR_FAIL                     XF_FAIL
#define XF_ERR_NO_MEM                   (-_XF_ERR_COM_NO_MEM)
#define XF_ERR_INVALID_ARG              (-_XF_ERR_COM_INVALID_ARG)
#define XF_ERR_INVALID_STATE            (-_XF_ERR_COM_INVALID_STATE)
#define XF_ERR_INVALID_CHECK            (-_XF_ERR_COM_INVALID_CHECK)
#define XF_ERR_INVALID_PORT             (-_XF_ERR_COM_INVALID_PORT)
#define XF_ERR_NOT_FOUND                (-_XF_ERR_COM_NOT_FOUND)
#define XF_ERR_NOT_SUPPORTED            (-_XF_ERR_COM_NOT_SUPPORTED)
#define XF_ERR_BUSY                     (-_XF_ERR_COM_BUSY)
#define XF_ERR_TIMEOUT                  (-_XF_ERR_COM_TIMEOUT)
#define XF_ERR_UNINIT                   (-_XF_ERR_COM_UNINIT)
#define XF_ERR_INITED                   (-_XF_ERR_COM_INITED)
#define XF_ERR_RESOURCE                 (-_XF_ERR_COM_RESOURCE)
#define XF_ERR_ISR                      (-_XF_ERR_COM_ISR)
#define XF_ERR_IOP                      (-_XF_ERR_COM_IOP)
#define XF_ERR_MAX                      (-_XF_ERR_COM_MAX)

#define XF_RET_COMMON_ERRNO_BITSIZE     8
#define XF_RET_MODULE_ERRNO_BITSIZE     8
#define XF_RET_LINE_BITSIZE             15
#define XF_RET_FLAG_BITSIZE             1

#if XF_TODO
/* *INDENT-OFF* */
typedef union_packed xf_return_information {
    xf_err_t full;
    struct_packed {
        int32_t common_errno    : XF_RET_COMMON_ERRNO_BITSIZE;  /*!< 通用错误码, 范围 [0, 255], 见 @ref XF_RET_BASE_*. */
        int32_t module_errno    : XF_RET_MODULE_ERRNO_BITSIZE;  /*!< 模块内部错误码, 范围 [0, 255]. */
        int32_t line            : XF_RET_LINE_BITSIZE;          /*!< 错误所在行号, 范围 [0, 32767]. */
        int32_t flag            : XF_RET_FLAG_BITSIZE;          /*!< 错误标识, 最高位, 此位为 0 时返回值视为正常数据,
                                                                 *   见 @ref XF_FAIL.
                                                                 */
    };
    struct_packed {
        int32_t data            : 31;   /*!< 正常数据,
                                         *   范围 [-1073741824, 1073741823],
                                         *   或   [0, 2147483647].
                                         */
        int32_t reserve         : 1;    /*!< 对应 flag, 通常不需要访问. */
    };
} xf_ret_info_t;
/* *INDENT-ON* */
#endif /* XF_TODO */

/* ==================== [Global Prototypes] ================================= */

/**
 * @brief 返回 xf_err_code_t 错误代码对应的错误信息字符串。
 *
 * @param code xf_err_code_t 错误代码。
 * @return const char * 错误信息字符串。
 */
const char *xf_err_to_name(xf_err_t code);

/* ==================== [Macros] ============================================ */

#if XF_TODO
#define XF_RET_GET_COM(_err)                    (BITSn_GET_RSH((_err), 9,  (0)))
#define XF_RET_GET_MOD(_err)                    (BITSn_GET_RSH((_err), 9,  (0+9)))
#define XF_RET_GET_LINE(_err)                   (BITSn_GET_RSH((_err), 13, (0+9+9)))
#define XF_RET_GET_FLAG(_err)                   (      BIT_GET((_err),     (0+9+9+13)))
#define XF_RET_TEST(_err)                       ((_err) & BIT31)

#define XF_RET_SET_COM(_err, _common_errno)     (BITSn_SET((_err), 9,  (0),          (_common_errno)))
#define XF_RET_SET_MOD(_err, _module_errno)     (BITSn_SET((_err), 9,  (0+9),        (_module_errno)))
#define XF_RET_SET_LINE(_err, _line)            (BITSn_SET((_err), 13, (0+9+9),    (_line)        ))
#define XF_RET_SET_FLAG1(_err)                  ( BIT_SET1((_err),     (0+9+9+13)                 ))
#define XF_RET_SET_FLAG0(_err)                  ( BIT_SET0((_err),     (0+9+9+13)                 ))
#define XF_RET_SET_FLAG(_err, _flag)            (  BIT_SET((_err),     (0+9+9+13), (_flag)        ))
#endif /* XF_TODO */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* __XF_ERR_H__ */

/**
 * @file xf_ble_gatt_common.h
 * @author dotc (dotchan@qq.com)
 * @brief
 * @version 1.0
 * @date 2024-08-06
 *
 * Copyright (c) 2024, CorAL. All rights reserved.
 *
 */

#ifndef __XF_BLE_GATT_COMMON_H__
#define __XF_BLE_GATT_COMMON_H__

/* ==================== [Includes] ========================================== */

#include "xf_utils.h"

#include "xf_ble_types.h"
#include "xf_ble_gap_types.h"

#if XF_BLE_IS_ENABLE

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

/**
 * @brief BT UUID 类型 (长度类型)
 */
typedef enum {
    XF_BLE_UUID_TYPE_16 = 2,        /*!< 长度 16-bit */
    XF_BLE_UUID_TYPE_32 = 4,        /*!< 长度 32-bit */
    XF_BLE_UUID_TYPE_128 = 16,      /*!< 长度 128-bit */
} xf_bt_uuid_type_t;

/**
 * @brief BT UUID 信息 (多种长度类型通用)
 */
typedef struct {
    union {
        xf_bt_uuid_type_t len_type;                 /*!< UUID 长度类型，见 @ref xf_bt_uuid_type_t */
        uint8_t _invalid;                           /*!< 用于固定 UUID 长度类型变量的大小为 8-bit */
    };
    union {
        uint16_t    uuid16;                         /*!< 16-bit UUID */
        uint32_t    uuid32;                         /*!< 32-bit UUID */
        uint8_t     uuid128[XF_BLE_UUID_TYPE_128];  /*!< 128-bit UUID */
    };
} __attribute__((packed)) xf_bt_uuid_info_t;


/**
 * @brief BT 16-bit UUID 信息
 */
typedef struct {
    union {
        xf_bt_uuid_type_t type;                     /*!< UUID 长度类型，见 @ref xf_bt_uuid_type_t */
        uint8_t _invalid;                           /*!< 用于固定 UUID 长度类型变量的大小为 8-bit */
    };
    uint16_t value;                                 /*!< 16-bit UUID */
} __attribute__((packed)) xf_bt_uuid16_info_t;

/**
 * @brief BT 32-bit UUID 信息
 */
typedef struct {
    union {
        xf_bt_uuid_type_t type;                     /*!< UUID 长度类型，见 @ref xf_bt_uuid_type_t */
        uint8_t _invalid;                           /*!< 用于固定 UUID 长度类型变量的大小为 8-bit */
    };
    uint32_t value;                                 /*!< 32-bit UUID */
} __attribute__((packed)) xf_bt_uuid32_info_t;

/**
 * @brief BT 128-bit UUID 信息
 */
typedef struct {
    union {
        xf_bt_uuid_type_t type;                     /*!< UUID 长度类型，见 @ref xf_bt_uuid_type_t */
        uint8_t _invalid;                           /*!< 用于固定 UUID 长度类型变量的大小为 8-bit */
    };
    uint8_t value[XF_BLE_UUID_TYPE_128];            /*!< 128-bit UUID */
} __attribute__((packed)) xf_bt_uuid128_info_t;


#define _XF_BLE_UUID16_INIT(uuid16)         \
    {                                       \
        .type = XF_BLE_UUID_TYPE_16,        \
        .value = (uuid16),                  \
    }

#define _XF_BLE_UUID32_INIT(uuid32)         \
    {                                       \
        .type = XF_BLE_UUID_TYPE_32,        \
        .value = (uuid32),                  \
    }

#define _XF_BLE_UUID128_INIT(uuid128 ...)   \
    {                                       \
        .type = XF_BLE_UUID_TYPE_128,       \
        .value = { uuid128 },               \
    }

/**
 * @brief BLE 声明一个 16-bit UUID
 * @note 通常用于填充服务结构时填入常量 UUID
 */
#define XF_BLE_DECLARE_UUID16(uuid16) \
    ((xf_bt_uuid_info_t *) (&(xf_bt_uuid16_info_t) _XF_BLE_UUID16_INIT(uuid16)))

/**
 * @brief BLE 声明一个 32-bit UUID
 * @note 通常用于填充服务结构时填入常量 UUID
 */
#define XF_BLE_DECLARE_UUID32(uuid32) \
    ((xf_bt_uuid_info_t *) (&(xf_bt_uuid32_info_t) _XF_BLE_UUID32_INIT(uuid32)))

/**
 * @brief BLE 声明一个 128-bit UUID
 * @note 通常用于填充服务结构时填入常量 UUID
 */
#define XF_BLE_DECLARE_UUID128(uuid128...) \
    ((xf_bt_uuid_info_t *) (&(xf_bt_uuid128_info_t) _XF_BLE_UUID128_INIT(uuid128)))


/**
 * @brief BLE 无效属性句柄
 * @note 通常用于对接服务结构添加时，判断服务、特征、特征描述符等属性的句柄是否有效
 */
#define XF_BLE_INVALID_ATTR_HANDLE  (0x0000)

/**
 * @brief BT 属性句柄
 */
typedef uint16_t xf_bt_attr_handle_t;

/**
 * @brief BLE GATT 属性权限
 */
typedef enum {
    XF_BLE_GATT_ATTR_PERMISSION_READ = 0x01,                /*!< 可读 */
    XF_BLE_GATT_ATTR_PERMISSION_WRITE = 0x02,               /*!< 可写 */
    XF_BLE_GATT_ATTR_PERMISSION_ENCRYPTION_NEED = 0x04,     /*!< 需要加密 */
    XF_BLE_GATT_ATTR_PERMISSION_AUTHENTICATION_NEED = 0x08, /*!< 需要认证 */
    XF_BLE_GATT_ATTR_PERMISSION_AUTHORIZATION_NEED = 0x10,  /*!< 需要授权 */
    XF_BLE_GATT_ATTR_PERMISSION_MITM_NEED = 0x20,           /*!< 需要MITM保护 */
} xf_ble_gatt_attr_permission_t;


/**
 * @brief BLE 特征的特性 (property)
 * @note
 *  @code
 *  1. 此处并不是严格按蓝牙标准进行定义 ———— 一字节 8 个特征的基本特性类型，
 *  而是 多字节 8 个基本特性类型 + 拓展特性类型的类型定义。
 *  2. 拓展类型从 基于蓝牙标准 1 字节基本特性类型的最高位( 0x80 ) 开始替换续接为拓展特性类型。
 *  3. 拓展类型名字都带上 EXT （暂定），以便开发者查阅、对比官方描述
 *  @endcode
 */
typedef enum {
    XF_BLE_GATT_CHARA_PROPERTY_BROADCAST    = 0x01,     /*!< 广播特征值，允许对该特征值进行广播 */
    XF_BLE_GATT_CHARA_PROPERTY_READ         = 0x02,     /*!< 读特征值，允许读取特征值 */
    XF_BLE_GATT_CHARA_PROPERTY_WRITE_NO_RSP = 0x04,     /*!< 写特征值且无需响应，允许写入特征值 (没有响应) */
    XF_BLE_GATT_CHARA_PROPERTY_WRITE        = 0x08,     /*!< 写特征值，允许写入特征值 (有响应) */
    XF_BLE_GATT_CHARA_PROPERTY_NOTIFY       = 0x10,     /*!< 通知特征值，允许向客户端进行特征值通知 (无需确认) */
    XF_BLE_GATT_CHARA_PROPERTY_INDICATE     = 0x20,     /*!< 指示特征值，允许向客户端进行特征值指示 (需要确认)*/
    XF_BLE_GATT_CHARA_PROPERTY_SIGNED_WRITE = 0x40,     /*!< 签名写特征值，允许对特征值进行签名写操作 */

    /* 以下为 特征的拓展特性类型  */
    XF_BLE_GATT_CHARA_PROPERTY_EXT_RELIABLE_WRITE   = 0x0080,
    XF_BLE_GATT_CHARA_PROPERTY_EXT_AUX_WRITE        = 0x0100,
    XF_BLE_GATT_CHARA_PROPERTY_EXT_READ_ENC         = 0x0200,
    XF_BLE_GATT_CHARA_PROPERTY_EXT_READ_AUTHEN      = 0x0400,
    XF_BLE_GATT_CHARA_PROPERTY_EXT_READ_AUTHOR      = 0x0800,
    XF_BLE_GATT_CHARA_PROPERTY_EXT_WRITE_ENC        = 0x1000,
    XF_BLE_GATT_CHARA_PROPERTY_EXT_WRITE_AUTHEN     = 0x2000,
    XF_BLE_GATT_CHARA_PROPERTY_EXT_WRITE_AUTHOR     = 0x4000,
} xf_ble_gatt_chara_property_t;

/**
 * @brief BLE GATT 属性
 * @note 属性由四部分组成：属性句柄、属性类型、属性值和属性权限
 *  @code
 *  |    2 octets | 2 or 16 octets| variable length  | implementation specific   |
 *  | attr handle |  attr type    | attr value       | attr permissions          |
 * @endcode
 */
typedef struct _xf_ble_gatt_attr_t {
    uint16_t handle;                /*!< 属性句柄 */
    xf_bt_uuid_info_t type;         /*!< UUID 类型 */
    /**
     * @brief
     *  用于存放数据。
     * 如果属性类型是:
     *  GATT特定类型（如服务项类型或特征值声明类型）:
     *      那么属性值就是UUID（各自类型的UUID，如服务UUID、特征UUID）等信息。
     *  如果是普通的特征值，则属性值是用户的数据。
     */
    uint8_t *value;
    uint16_t permissions;
    // uint16_t max_length;               /*!< Maximum length of the element*/
    // uint16_t length;                   /*!< Current length of the element*/
} xf_ble_gatt_attr_t;
/**
 * @brief BLE GATT 最大属性长度
 */
#define XF_BLE_GATT_MAX_ATTR_LEN        (600)
#define XF_BLE_GATT_INVALID_HANDLE      (0x00)


/**
 * @brief BLE GATT 特征描述符类型
 */
typedef enum {
    XF_BLE_GATT_CHARA_DESC_TYPE_CHARA_EXT_PROPERTIES,       // 0x2900, CEP:Characteristic Extended Properties
    XF_BLE_GATT_CHARA_DESC_TYPE_CHARA_USER_DESC,            // 0x2901, CUD:Characteristic User Description
    XF_BLE_GATT_CHARA_DESC_TYPE_CLIENT_CHARA_CFG,           // 0x2902, CCC:Client Characteristic Configuration
    XF_BLE_GATT_CHARA_DESC_TYPE_SERVER_CHARA_CFG,           // 0x2903, SCC:Server Characteristic Configuration
    XF_BLE_GATT_CHARA_DESC_TYPE_CHARA_PRESENTATION_FORMAT,  // 0x2904, CPF:Characteristic Presentation Format
    XF_BLE_GATT_CHARA_DESC_TYPE_CHARA_AGGREGATE_FORMAT,     // 0x2903, CAF:Characteristic Aggregate Format
} xf_ble_gatt_chara_desc_type_t;

/**
 * @brief BLE GATT 服务类型
 */
typedef enum {
    XF_BLE_GATT_SERVICE_TYPE_PRIMARY,           /*!< 首要服务 */
    XF_BLE_GATT_SERVICE_TYPE_SECONDARY,         /*!< 次要服务 */
} xf_ble_gatt_service_type_t;

/**
 * @brief BLE GATT 操作错误码
 */
typedef enum {
    XF_BLE_GATT_ERR_SUCCESS                             = 0x00, /*!< 执行成功 */
    XF_BLE_GATT_ERR_INVALID_HANDLE                      = 0x01, /*!< 无效句柄 */
    XF_BLE_GATT_ERR_READ_NOT_PERMITTED                  = 0x02, /*!< 无读权限 */
    XF_BLE_GATT_ERR_WRITE_NOT_PERMITTED                 = 0x03, /*!< 无写权限 */
    XF_BLE_GATT_ERR_INVALID_PDU                         = 0x04, /*!< 无效 PDU */
    XF_BLE_GATT_ERR_INSUFFICIENT_AUTHENTICATION         = 0x05, /*!< 未认证 */
    XF_BLE_GATT_ERR_REQUEST_NOT_SUPPORTED               = 0x06, /*!< 不支持客户端的请求 */
    XF_BLE_GATT_ERR_INVALID_OFFSET                      = 0x07, /*!< 无效偏移的读写 */
    XF_BLE_GATT_ERR_INSUFFICIENT_AUTHORIZATION          = 0x08, /*!< 未授权 */
    XF_BLE_GATT_ERR_PREPARE_QUEUE_FULL                  = 0x09, /*!< 排队的 prepare writes 太多，队列已满 */
    XF_BLE_GATT_ERR_ATTRIBUTE_NOT_FOUND                 = 0x0A, /*!< 排队的 prepare writes 太多，队列已满 */
    XF_BLE_GATT_ERR_ATTRIBUTE_NOT_LONG                  = 0x0B, /*!< 该属性不是长属性, 不能使用 ATT_READ_BLOB_REQ PDU */
    XF_BLE_GATT_ERR_INSUFFICIENT_ENCRYPTION_KEY_SIZE    = 0x0C, /*!< 用于加密此连接的加密密钥大小不足 */
    XF_BLE_GATT_ERR_INVALID_ATTRIBUTE_VALUE_LENGTH      = 0x0D, /*!< 用于加密此连接的加密密钥大小不足 */
    XF_BLE_GATT_ERR_UNLIKELY_ERROR                      = 0x0E, /*!< 不可能的错误 */
    XF_BLE_GATT_ERR_INSUFFICIENT_ENCRYPTION             = 0x0F, /*!< 加密不足 */
    XF_BLE_GATT_ERR_UNSUPPORTED_GROUP_TYPE              = 0x10, /*!< 不支持的 grouping attribute */
    XF_BLE_GATT_ERR_INSUFFICIENT_RESOURCES              = 0x11, /*!< 资源不足错误码 */
    XF_BLE_GATT_ERR_DATABASE_OUT_OF_SYNC                = 0x12, /*!< 服务端与客户端未同步数据库 */
    XF_BLE_GATT_ERR_VALUE_NOT_ALLOWED                   = 0x13, /*!< 非法的参数值 */

    /* 0x80 – 0x9F ：由更高层规范定义的应用程序错误代码。 */
    XF_BLE_GATT_ERR_APP_ERROR_START                     = 0x80,
    XF_BLE_GATT_ERR_APP_ERROR_END                       = 0x9F,

    /* 0xE0 – 0xFF ：通用配置文件和服务错误代码 */
    XF_BLE_GATT_ERR_COMMON_PROFILE_AND_SERVICE_START    = 0xE0,
    XF_BLE_GATT_ERR_COMMON_PROFILE_AND_SERVICE_END      = 0xFF,
} xf_ble_gatt_err_t;


/* ==================== [Global Prototypes] ================================= */

/* ==================== [Macros] ============================================ */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* XF_BLE_IS_ENABLE */

#endif /* __XF_BLE_GATT_COMMON_H__ */

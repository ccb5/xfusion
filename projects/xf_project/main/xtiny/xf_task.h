/**
 * @file xf_task.h
 * @author catcatBlue (catcatblue@qq.com)
 * @brief xf_task 模块公共接口。
 * @version 1.0
 * @date 2025-06-16
 *
 * SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#ifndef __XF_TASK_H__
#define __XF_TASK_H__

/* ==================== [Includes] ========================================== */

#include "xf_task_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

/* ==================== [Global Prototypes] ================================= */

/**
 * @brief 初始化任务调度器.
 *
 * @return xf_err_t
 *      - XF_OK                 成功
 */
xf_err_t xf_task_sched_init(void);

/**
 * @brief 反初始化任务调度器.
 *
 * @return xf_err_t
 *      - XF_OK                 成功
 */
xf_err_t xf_task_sched_deinit(void);

/**
 * @brief 任务句柄转任务 ID.
 *
 * @return xf_task_id_t
 *      - XF_TASK_ID_INVALID    无效句柄
 *      - OTHER                 任务 ID
 */
xf_task_id_t xf_task_to_id(const xf_task_t *task);

/**
 * @brief 任务 ID 转任务句柄.
 *
 * @return xf_task_t *
 *      - NULL                  无效 ID
 *      - OTHER                 任务句柄
 */
xf_task_t *xf_task_id_to_task(xf_task_id_t id);

/* ==================== [Macros] ============================================ */

/**
 * @brief 声明或定义任务回调函数.
 *
 * @param _name         任务函数名。 @ref xf_task_cb_t.
 */
#define xf_task_cb(_name)               xf_task_state_t _name(xf_task_t *me, void *arg)

/**
 * @brief 创建任务.
 *
 * @note 1. 创建的任务都在任务池中，可以不保存返回的句柄，通过 xf_task_sched() 调度。
 * @note 2. 任务创建后不会立即执行。
 *
 * @param _cb_func      任务函数。 @ref xf_task_cb_t.
 * @param _user_data    任务内的用户数据。 @ref xf_task_t.user_data.
 * @return xf_task_t*   创建的任务
 *      - NULL          创建失败
 *      - OTHER         创建成功的任务
 */
#define xf_task_create(_cb_func, _user_data) \
                                        xf_task_create_i((_cb_func), (_user_data))

/**
 * @brief 销毁任务.
 *
 * @note 通常不需要手动销毁，在 @ref xf_task_end 分支结束后会自动销毁。
 *
 * @param _task         需要销毁的任务。 @ref xf_task_t.
 * @param _cb_func      任务函数。 @ref xf_task_cb_t.
 * @param _user_data    任务内的用户数据。 @ref xf_task_t.user_data.
 * @return xf_err_t
 *      - XF_ERR_INVALID_ARG    无效参数
 *      - XF_OK                 成功
 */
#define xf_task_destroy(_task)          xf_task_destroy_i(_task)

/**
 * @brief 开始任务.
 *
 * @note 开始任务 = 创建任务 + 立即运行任务.
 *
 * @param[out] _task    传出的任务句柄。 @ref xf_task_t 指针.
 * @param _cb_func      任务函数。 @ref xf_task_cb_t.
 * @param _user_data    任务内的用户数据。 @ref xf_task_t.user_data.
 * @param _arg          传给任务的参数。
 * @return xf_task_state_t 任务状态。
 *      - XF_TASK_READY        就绪
 *      - XF_TASK_BLOCKED      阻塞
 *      - XF_TASK_TERMINATED   结束
 */
#define xf_task_start(_task, _cb_func, _user_data, _arg) \
                                        xf_task_start_i((_task), (_cb_func), (_user_data), (_arg))

/**
 * @brief 运行任务.
 *
 * @note 运行任务 = 设置任务为就绪状态 + 调用任务.
 *
 * @param _task         要运行的任务。 @ref xf_task_t.
 * @param _arg          传给任务的参数。
 * @return xf_task_state_t 任务状态。
 *      - XF_TASK_READY        就绪
 *      - XF_TASK_BLOCKED      阻塞
 *      - XF_TASK_TERMINATED   结束
 */
#define xf_task_run(_task, _arg)        xf_task_run_i((_task), (_arg))

/**
 * @brief 创建子任务.
 *
 * @note 1. 父任务与子任务之间会相互记录 ID.
 * @note 2. 任务创建后不会立即执行。
 *
 * @param _me           父任务。 @ref xf_task_t.
 * @param _cb_func      任务函数。 @ref xf_task_cb_t.
 * @param _user_data    任务内的用户数据。 @ref xf_task_t.user_data.
 * @return xf_task_t*   创建的任务
 *      - NULL          创建失败
 *      - OTHER         创建成功的任务
 */
#define xf_task_create_subtask(_me, _cb_func, _user_data) \
                                        xf_task_create_subtask_i((_me), (_cb_func), (_user_data))

/**
 * @brief 等待子任务结束.
 *
 * @note 仅当子任务执行完毕后才会继续执行当前任务后面的代码。
 * @warning
 *
 * @param _me           当前任务。 @ref xf_task_t.
 * @param _arg          传给子任务的参数。
 */
#define xf_task_wait_subtask(_me, _arg) \
                                        xf_task_wait_subtask_i((_me), (_arg))

/**
 * @brief 开始子任务.
 *
 * @note 开始任务 = 创建子任务 + 等待子任务结束.
 *
 * @param _task         需要销毁的任务。 @ref xf_task_t.
 * @param _cb_func      任务函数。 @ref xf_task_cb_t.
 * @param _user_data    任务内的用户数据。 @ref xf_task_t.user_data.
 * @param _arg          传给任务的参数。
 * @return xf_err_t
 *      - XF_ERR_INVALID_ARG    无效参数
 *      - XF_OK                 成功
 */
#define xf_task_start_subtask(_me, _cb_func, _user_data, _arg) \
                                        xf_task_start_subtask_i((_me), (_cb_func), (_user_data), (_arg))

/**
 * @brief 任务块起始.
 *
 * @note 任务块起始 = 设置系统变量 + 恢复 lc.
 *
 * @param _me           当前任务。 @ref xf_task_t.
 */
#define xf_task_begin(_me)              xf_task_begin_i(_me)

/**
 * @brief 任务块结束.
 *
 * @note 任务块结束 = 重置 lc + 销毁任务 (含 @ref xf_task_t.user_data).
 *
 * @param _me           当前任务。 @ref xf_task_t.
 */
#define xf_task_end(_me)                xf_task_end_i(_me)

/**
 * @brief 获取任务状态.
 *
 * @param _task         任务句柄。 @ref xf_task_t.
 * @return xf_task_state_t @ref xf_task_state_t
 *      - XF_TASK_READY        就绪
 *      - XF_TASK_BLOCKED      阻塞
 *      - XF_TASK_TERMINATED   结束
 */
#define xf_task_get_state(_task)        xf_task_attr_get_state(_task)

/**
 * @brief 设置任务为就绪状态.
 *
 * @note 通常不需要设置，任务创建后会自动设置为就绪状态。
 *
 * @param _task         任务句柄。 @ref xf_task_t.
 */
#define xf_task_set_ready(_task)        xf_task_attr_set_state((_task), XF_TASK_READY)

/**
 * @brief 设置任务为阻塞状态.
 *
 * @note 通常不需要设置。
 *
 * @param _task         任务句柄。 @ref xf_task_t.
 */
#define xf_task_set_blocked(_task)      xf_task_attr_set_state((_task), XF_TASK_BLOCKED)

/**
 * @brief 重启一个任务.
 *
 * @note 不会离开重新执行，需要下一次进入后才能重新执行。
 *
 * @param _task         需要重启的任务。 @ref xf_task_t.
 */
#define xf_task_restart(_task)          xf_task_restart_i(_task)

/**
 * @brief 任务让出.
 *
 * @note 让出一次，让出后任务会进入就绪状态，等待下一次 xf_task_sched() 调度。
 *
 * @param _me           当前任务。 @ref xf_task_t.
 */
#define xf_task_yield(_me)              xf_task_yield_i(_me)

/**
 * @brief 阻塞任务.
 *
 * @warning 如果没有唤醒源，任务会一直阻塞，请勿随意调用。
 *
 * @param _me           当前任务。 @ref xf_task_t.
 */
#define xf_task_block(_me)              xf_task_block_i(_me)

/**
 * @brief 任务延时.
 *
 * @param _me           当前任务。 @ref xf_task_t.
 * @param _tick         延时时间，单位 (tick)。 @ref xf_tick_t.
 */
#define xf_task_delay(_me, _tick)       xf_task_delay_i((_me), (_tick))

/**
 * @brief 任务 ms 延时.
 *
 * @param _me           当前任务。 @ref xf_task_t.
 * @param _ms           延时时间，单位 (ms)。 @ref xf_tick_t.
 */
#define xf_task_delay_ms(_me, _ms)      xf_task_delay_ms_i((_me), (_ms))

/**
 * @brief 任务等待事件.
 *
 * @param _me           当前任务。 @ref xf_task_t.
 * @param _id           事件 ID。 @ref xf_event_id_t.
 * @param _tick         等待时间，单位 (tick)。 @ref xf_tick_t.
 * @param[out] _xf_err  错误码。 @ref xf_err_t.
 *                      - XF_ERR_TIMEOUT     超时
 *                      - XF_OK              成功
 */
#define xf_task_wait_until(_me, _id, _tick, _xf_err) \
                                        xf_task_wait_until_i((_me), (_id), (_tick), (_xf_err))

/**
 * @brief 任务等待事件，单位 (ms).
 *
 * @param _me           当前任务。 @ref xf_task_t.
 * @param _id           事件 ID。 @ref xf_event_id_t.
 * @param _tick         等待时间，单位 (tick)。 @ref xf_tick_t.
 * @param[out] _xf_err  错误码。 @ref xf_err_t.
 *                      - XF_ERR_TIMEOUT     超时
 *                      - XF_OK              成功
 */
#define xf_task_wait_until_ms(_me, _id, _ms, _xf_err) \
                                        xf_task_wait_until_ms_i((_me), (_id), (_ms), (_xf_err))

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __XF_TASK_H__ */

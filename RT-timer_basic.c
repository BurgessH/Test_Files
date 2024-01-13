/*
 *   文件名: FILE NAME
 *   作者:    -
 *   日期:    -
 *   版本:    -
 *   说明:    -
 *   修改记录: -
 */
struct rt_timer
{
    struct rt_object parent;                            /**< inherit from rt_object */

    rt_list_t        row[RT_TIMER_SKIP_LIST_LEVEL];

    void (*timeout_func)(void *parameter);              /**< timeout function */
    void            *parameter;                         /**< timeout function's parameter */

    rt_tick_t        init_tick;                         /**< timer timeout tick */
    rt_tick_t        timeout_tick;                      /**< timeout tick */
};
typedef struct rt_timer *rt_timer_t;


  rt_timer_check()



1. rt_timer_create()创建定时器时根据定时器flag,放入rt_soft_timer_list,rt_hard_timer_list,;

2.放入rt_soft_timer_list调用流程:
    取出链表中的第一个timer,while (!rt_list_isempty(&_timer_list[RT_TIMER_SKIP_LIST_LEVEL - 1])),判断是否超时，超时后执行超时函数,移除timer_list,
    判断是否为周期,为周期调用, rt_timer_start(t),再放入timer_list;
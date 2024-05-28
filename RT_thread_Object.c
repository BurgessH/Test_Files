/*
 *   文件名: FILE NAME
 *   作者:    -
 *   日期:    -
 *   版本:    -
 *   说明:    -
 *   修改记录: -
 */


//1.函数调用方式：
typedef int (*init_fn_t)(void);
#define SECTION(x)                  __attribute__((section(x)))
#define RT_USED                     __attribute__((used))

//Export 给函数特定的段属性、程序在链接时根据不同的段属性，依次链接执行；components.c分别根据段属性执行相应的函数Board、INIT_COMPONENT_EXPORT
INIT_BOARD_EXPORT(uart_init);
#define INIT_BOARD_EXPORT(fn)        INIT_EXPORT(fn, 	"1")
#define INIT_EXPORT(fn, level)       RT_USED SECTION(".rti_fn.init") const init_fn_t __rt_init_##fn  = fn
															 const init_fn_t __rt_init_uart_init = uart_init
xx.map .o文件链接顺序,按照链接文件统一顺序链接； 
指针保存在flash 某个地址中,地址中保存的值为函数地址；          


//2.启动流程
加载域与执行域：
    2.1.重定位 +RW 数据段
    2.2.+ZI
    2.3.调用__main->$sub$$main->($sup$$main()->)rtthread_startup()
        单板的初始化
        显示rt_thread 版本
        定时器初始化
        调度器初始化
        application_init->(main只是在某个线程的函数里面被调用而已);




//3.rt_object:
struct rt_object
{
    char       name[RT_NAME_MAX];                       /**< name of kernel object */
    rt_uint8_t type;                                    /**< type of kernel object */
    rt_uint8_t flag;                                    /**< flag of kernel object */

    rt_list_t  list;                                    /**< list node of kernel object */
};
typedef struct rt_object *rt_object_t;                  /**< Type for kernel objects. */

struct rt_timer
{
    struct rt_object parent;                            /**< inherit from rt_object */

    rt_list_t        row[RT_TIMER_SKIP_LIST_LEVEL];		// RT_TIMER_SKIP_LIST_LEVEL=1 

    void (*timeout_func)(void *parameter);              /**< timeout function */
    void            *parameter;                         /**< timeout function's parameter */

    rt_tick_t        init_tick;                         /**< timer timeout tick */
    rt_tick_t        timeout_tick;                      /**< timeout tick */
};
typedef struct rt_timer *rt_timer_t;

static void _rt_timer_init(rt_timer_t timer,
                           void (*timeout)(void *parameter),
                           void      *parameter,
                           rt_tick_t  time,
                           rt_uint8_t flag)
{
    int i;

    /* set flag */
    timer->parent.flag  = flag;
    /* set deactivated */
    timer->parent.flag &= ~RT_TIMER_FLAG_ACTIVATED;

    timer->timeout_func = timeout;
    timer->parameter    = parameter;

    timer->timeout_tick = 0;
    timer->init_tick    = time;

    /* initialize timer list */
    for (i=0; i<RT_TIMER_SKIP_LIST_LEVEL; i++)
    {
        rt_list_init(&(timer->row[i]));
    }
}





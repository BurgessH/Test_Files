

//1.函数调用方式：
typedef int (*init_fn_t)(void);

#define SECTION(x)                  __attribute__((section(x)))
#define RT_USED                     __attribute__((used))

//Export 给函数特定的段属性、程序在链接时根据不同的段属性，依次链接执行；components.c分别根据段属性执行相应的函数Board、INIT_COMPONENT_EXPORT
INIT_BOARD_EXPORT(uart_init);
#define INIT_BOARD_EXPORT(fn)        INIT_EXPORT(fn, 	"1")
#define INIT_EXPORT(fn, level)       RT_USED SECTION(".rti_fn.init") const init_fn_t __rt_init_##fn  = fn
																	 const init_fn_t __rt_init_uart_init = uart_init
//2.启动流程
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





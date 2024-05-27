/*
 *   文件名: FILE NAME
 *   作者:    -
 *   日期:    -
 *   版本:    -
 *   说明:    -
 *   修改记录: -
 */
线程结构体信息：（运行一段时间、选取下一个线程tlist）
    1.线程栈的大小、起始地址、长度
    2.线程运行时间,初始tick、剩下tick
	3.任务就绪状态list
	4.定时器


1.创建线程：
    0.构造线程结构体，

    1.伪造现场：保存CPU各个寄存器的值，每个线程栈地址（起始地址、大小）找到线程的栈地址，（将栈上各个寄存器的值给CPU的寄存器，运行）
            thread->sp = (void *)rt_hw_stack_init(thread->entry, thread->parameter, (rt_uint8_t *)((char *)thread->stack_addr + thread->stack_size - sizeof(rt_ubase_t)), (void *)_thread_exit);
    
    2.初始化定时器        



2.启动第一个线程： rt_system_scheduler_start()

    1.线程运行一段时间（初始tick、剩下tick）
   
    2.选出下一个线程(从哪里选)
        rt_thread_create()

        rt_thread_startup(): 并未真正的启动,只是将线程放入就绪列表
		
            2.1.就绪的线程: 将每个就绪的线程按照优先级的不同放入某个tlist列表(多个优先级->链表数组实现)
                rt_list_insert_before(&(rt_thread_priority_table[thread->current_priority]), &(thread->tlist))
                如果调度器已经启动则调用一次调度rt_schedule()
			
        2.3.定时器中断调度：
            
        2.4.找出第一个需要执行的线程：根据优先级的高低找到一个最高优先级的链表，void rt_schedule(void)
			/*找到优先级最高的线程*/
           highest_ready_priority = __rt_ffs(rt_thread_ready_priority_group) - 1;
			
            to_thread = rt_list_entry(rt_thread_priority_table[highest_ready_priority].next, struct rt_thread, tlist);
			
            rt_current_thread = to_thread;

            rt_hw_context_switch_interrupt((rt_ubase_t)&from_thread->sp, (rt_ubase_t)&Vto_thread->sp);
			 rt_hw_context_switch_to((rt_uint32_t)&to_thread->sp);
			
异常处理过程：
    3.中断现场恢复PendSV:       
            2.5.保存现场：
				硬件自动保存A部分现场-也就是异常栈帧；
				软件保存其他寄存器；r4-r11

            2.6..恢复现场：
                软件恢复: SP指向线程的栈,将栈保存的信息写入各个寄存器 r4-r11, PSP->r1
                BxLR_bit2 触发硬件的中断返回时: PSP, 中断返回时，硬件自动恢复异常栈帧；

    4.线程挂起与恢复
            1.B运行时挂起A：把A从就绪链表中移除->rt_thread_suspend();
			2.A挂起自己： 从就绪链表中移除->rt_schedule(void);
            3.C恢复A, rt_thread_resume(), 放入就绪链表->rt_schedule(void);
            3.休眠 A rt_thread_dalay(): A从就绪链表中移除, 启动A本身定时器,rt_schedule(void);超时处理函数，重新放入就绪链表->触发一次调度rt_schedule(void)；










#define _OBJ_CONTAINER_LIST_INIT(c)   {&(_object_container[c].object_list), &(_object_container[c].object_list)}

struct rt_object_information
{
    enum rt_object_class_type type;                     /*  RT_Object_Class_Thread*< object class type        */
    rt_list_t                 object_list;              /*  _OBJ_CONTAINER_LIST_INIT(RT_Object_Info_Thread)   *< object list */
    rt_size_t                 object_size;              /*  sizeof(struct rt_thread)*< object size            */
};

{RT_Object_Class_Thread, _OBJ_CONTAINER_LIST_INIT(RT_Object_Info_Thread), sizeof(struct rt_thread)},






//所有CPU寄存器：
struct exception_stack_frame
{
    rt_uint32_t r0;
    rt_uint32_t r1;
    rt_uint32_t r2;
    rt_uint32_t r3;
    rt_uint32_t r12;
    rt_uint32_t lr; 			    /*r14*/
    rt_uint32_t pc;					/*r15*/
    rt_uint32_t psr;
};

struct stack_frame
{
    /* r4 ~ r11 register */
    rt_uint32_t r4;
    rt_uint32_t r5;
    rt_uint32_t r6;
    rt_uint32_t r7;
    rt_uint32_t r8;
    rt_uint32_t r9;
    rt_uint32_t r10;
    rt_uint32_t r11;

    struct exception_stack_frame exception_stack_frame;
};
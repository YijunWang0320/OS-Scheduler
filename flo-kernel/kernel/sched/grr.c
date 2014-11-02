/*
 * Multicore Round Robin Scheduling Class (mapped to the SCHED_GRR
 * policy)
 */
#include "sched.h"

#include <linux/slab.h>

static void enqueue_task_grr(struct rq *rq, struct task_struct *p, int wakeup)
{
	p->grr.time_slice = RR_TIMESLICE;
	struct rq *this_rq;
	int cpu = 0;
	int lowest_cpu = 0;
	int lowest = -1;
	for_each_cpu(cpu, this_rq->rd->rto_mask)
	{
		this_rq = cpu_rq(cpu);

		if(lowest == -1 || this_rq->grr.nr_running < lowest)
		{
			lowest = this_rq->grr.nr_running;
			lowest_cpu = cpu;
		}
		
	}

	rq = cpu_rq(lowest_cpu);

	list_add_tail(&p->grr.run_list, &rq->grr.queue);
	rq->grr.nr_running++;
}

static void requeue_task_grr(struct rq *rq, struct task_struct *p)
{
	list_move_tail(&p->grr.run_list,&rq->grr.queue);
}

static void yield_task_grr(struct rq *rq)
{
	printk("yield_task %ld\n",(unsigned long int)rq->curr);
	requeue_task_grr(rq,rq->curr);
}
static void dequeue_task_grr(struct rq *rq, struct task_struct *p, int sleep)
{
	/* update_curr_other_rr(rq); */
	list_del(&p->grr.run_list);
	rq->grr.nr_running--;
}
static void check_preempt_curr_grr(struct rq *rq, struct task_struct *p, int flags)
{
}
static struct task_struct *pick_next_task_grr(struct rq *rq)
{
	struct sched_grr_entity *temp_next;
	struct task_struct *next;

	next = NULL;

	if (!list_empty(&rq->grr.queue)) {
		temp_next = list_entry(rq->grr.queue.next, struct sched_grr_entity, run_list);
		next = container_of(temp_next, struct task_struct, grr);
		next->se.exec_start = rq->clock;
	}
	return next;
}
static void put_prev_task_grr(struct rq *rq, struct task_struct *p)
{
}

#ifdef CONFIG_SMP
static int select_task_rq_grr(struct task_struct *p, int sd_flag, int flags)
{
	return task_cpu(p);
}


static unsigned long
load_balance_grr(struct rq *this_rq)
{
}

static int move_one_task_grr(struct rq *this_rq, int this_cpu, struct rq *busiest, struct sched_domain *sd, enum cpu_idle_type idle)
{
        return 0;
}

#endif


static void requeue_grr_entity(struct grr_rq *grr_rq, struct sched_grr_entity *grr_se, int head)
{

}

void init_grr_rq(struct grr_rq *grr_rq, struct rq *rq) {
	INIT_LIST_HEAD(&grr_rq->queue);
}


static void set_curr_task_grr(struct rq *rq)
{
	struct task_struct *p = rq->curr;

	p->se.exec_start = rq->clock;
}

/*TODO*/
static void task_tick_grr(struct rq *rq, struct task_struct *curr, int queued)
{
	if (RR_TIMESLICE == 0)
	{
		//This case we will do FIFO
		return;
	} else {
		if (curr->grr.time_slice-- == 0) {
			curr->grr.time_slice = RR_TIMESLICE;
			set_tsk_need_resched(curr);
			yield_task_grr(rq);
		}
	}
}

static void task_fork_grr(struct task_struct *p)
{

}

unsigned int get_rr_interval_grr(struct task_struct *task)
{
        /*
 *          * Time slice is 0 for SCHED_FIFO tasks
 *                   */
        if (task->policy == SCHED_GRR)
                return RR_TIMESLICE;
        else
                return 0;
}

static void prio_changed_grr(struct rq *rq, struct task_struct *p, int oldprio, int running) {
}

static void switched_to_grr(struct rq *rq, struct task_struct *p, int running) {
}

static void switched_from_grr(struct rq *rq, struct task_struct *p) {
}

static void task_move_group_grr(struct task_struct *p, int on_rq)
{

}

const struct sched_class grr_sched_class = {
	.next			= &fair_sched_class,
	.enqueue_task		= enqueue_task_grr,
	.dequeue_task		= dequeue_task_grr,
	.yield_task		= yield_task_grr,

	.check_preempt_curr	= check_preempt_curr_grr,

	.pick_next_task		= pick_next_task_grr,
	.put_prev_task		= put_prev_task_grr,

#ifdef CONFIG_SMP
	.select_task_rq		= select_task_rq_grr,
#endif

	.set_curr_task      = set_curr_task_grr,
	.task_tick			= task_tick_grr,
	.task_fork			= task_fork_grr,
	.switched_from		= switched_from_grr,
	.switched_to        = switched_to_grr,
	.get_rr_interval	= get_rr_interval_grr,
	.prio_changed		= prio_changed_grr,

#ifdef CONFIG_FAIR_GROUP_SCHED
	.task_move_group    = task_move_group_grr,
#endif
};


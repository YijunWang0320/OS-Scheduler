/*
 * Multicore Round Robin Scheduling Class (mapped to the SCHED_GRR
 * policy)
 */
#include "sched.h"

#include <linux/slab.h>

static void enqueue_task_grr(struct rq *rq, struct task_struct *p, int wakeup)
{
	p->grr.time_slice = RR_TIMESLICE;
	spin_lock(&rq->grr.grr_runtime_lock);
	list_add_tail(&p->grr.run_list, &rq->grr.queue);
	rq->grr.nr_running++;
	spin_unlock(&rq->grr.grr_runtime_lock);
	//printk( "enqueue(): add %ld, n = %ld\n", (unsigned long int)p, rq->grr.nr_running);
}

static void requeue_task_grr(struct rq *rq, struct task_struct *p)
{
	spin_lock(&rq->grr.grr_runtime_lock);
	list_move_tail(&p->grr.run_list,&rq->grr.queue);
	spin_unlock(&rq->grr.grr_runtime_lock);
}

static void yield_task_grr(struct rq *rq)
{
	//printk("yield_task %ld\n",(unsigned long int)rq->curr);
	requeue_task_grr(rq,rq->curr);
}
static void dequeue_task_grr(struct rq *rq, struct task_struct *p, int sleep)
{
	/* update_curr_other_rr(rq); */
	spin_lock(&rq->grr.grr_runtime_lock);
	list_del(&p->grr.run_list);
	rq->grr.nr_running--;
	spin_unlock(&rq->grr.grr_runtime_lock);
	//printk( "deueue(): del %ld, n = %ld\n", (unsigned long int)p, rq->grr.nr_running);
}
static void check_preempt_curr_grr(struct rq *rq, struct task_struct *p, int flags)
{
}
static struct task_struct *pick_next_task_grr(struct rq *rq)
{
	struct sched_grr_entity *temp_next;
	struct task_struct *next;

	next = NULL;
	spin_lock(&rq->grr.grr_runtime_lock);
	if (!list_empty(&rq->grr.queue)) {
		temp_next = list_entry(rq->grr.queue.next, struct sched_grr_entity, run_list);
		next = container_of(temp_next, struct task_struct, grr);
		next->se.exec_start = rq->clock;
	}
	spin_unlock(&rq->grr.grr_runtime_lock);
	return next;
}
static void put_prev_task_grr(struct rq *rq, struct task_struct *p)
{
}

#ifdef CONFIG_SMP
static int select_task_rq_grr(struct task_struct *p, int sd_flag, int flags)
{
	struct rq *this_rq;
	int lowest = -1;
	int cpu;
	int lowest_cpu = -1;
	
	for_each_possible_cpu(cpu)
	{
		this_rq = cpu_rq(cpu);
	 	
		if(lowest == -1 || this_rq->grr.nr_running < lowest)
		{
			spin_lock(&rq->grr.grr_runtime_lock);
			lowest = this_rq->grr.nr_running;
			lowest_cpu = cpu;
			spin_unlock(&rq->grr.grr_runtime_lock);
		}
		
	}
	
	if (lowest_cpu == -1)
		return task_cpu(p);
	else
		return lowest_cpu;
}


static unsigned long
load_balance_grr(struct rq *this_rq)
{
	// int cpu,ret;
	// struct rq *this_rq;
	// struct grr_rq *lowest_grr_rq = NULL;
	// struct grr_rq *highest_grr_rq = NULL;
	// struct rq *highest_rq = NULL;
	// struct rq *lowest_rq = NULL;

	// int highest = INT_MIN,lowest = INT_MAX, curr;
	// ret = 0;
	// for_each_possible_cpu(cpu) {
	// 	this_rq = cpu_rq(cpu);
	// 	if(this_rq == NULL)
	// 		continue;
	// 	curr = this_rq->grr.nr_running;
	// 	if(curr >= highest) {
	// 		highest = curr;
	// 		highest_grr_rq = this_rq->grr;
	// 		highest_rq = this_rq;
	// 		continue;
	// 	}
	// 	if(curr < lowest) {
	// 		lowest = curr;
	// 		lowest_grr_rq = this_rq->grr;
	// 		lowest_rq = this_rq;
	// 		continue;
	// 	}
	// }
	// if(highest - lowest <= 1) {
	// 	return ret;
	// }
	// if (highest_grr_rq->nr_running == 1)
	// {
	// 	return ret;
	// }
	// struct list_head *pos = &highest->next->next;
	// int to_cpu = lowest_rq->cpu;

	// struct sched_grr_entity *from_entity = list_entry(pos,struct sched_grr_entity,run_list);
	// struct task_struct *from_task = container_of(from_entity,struct task_struct,grr);
	
	// double_lock_balance(highest_rq, lowest_rq);

	// deactivate_task(highest_grr_rq,from_task,0);
	// set_task_cpu(from_task,to_cpu);
	// activate_task(lowest_grr_rq,from_task,0);

	// double_unlock_balance(highest_rq, lowest_rq);

	// return 1;
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


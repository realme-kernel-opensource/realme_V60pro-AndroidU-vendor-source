/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2022 Oplus. All rights reserved.
 */

#ifdef CONFIG_LOCKING_PROTECT
#ifndef _SCHED_ASSIST_LOCKING_H_
#define _SCHED_ASSIST_LOCKING_H_

void oplus_replace_locking_task_fair(struct rq *rq,
		struct task_struct **p, struct sched_entity **se, bool *repick, bool simple);
void dequeue_locking_thread(struct rq *rq, struct task_struct *p);
void enqueue_locking_thread(struct rq *rq, struct task_struct *p);
bool task_inlock(struct oplus_task_struct *ots);
int sched_assist_locking_init(void);
void sched_assist_locking_exit(void);
void locking_state_systrace_c(unsigned int cpu, struct task_struct *p);
void locking_tick_hit(struct task_struct *prev, struct task_struct *next);
bool task_skip_protect(struct task_struct *p);
void check_preempt_tick_handler_locking(struct task_struct *p,
			unsigned long *ideal_runtime, bool *skip_preempt,
			unsigned long delta_exec, struct cfs_rq *cfs_rq,
			struct sched_entity *curr, unsigned int granularity);
extern unsigned int locking_wakeup_preepmt_enable;
extern bool locking_protect_disable;
void record_lock_starttime(struct task_struct *tsk, unsigned long settime);
#endif /* _SCHED_ASSIST_LOCKING_H_ */
#endif /* CONFIG_LOCKING_PROTECT */

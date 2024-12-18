// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2022 Oplus. All rights reserved.
 */

#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/printk.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/ktime.h>
#include <linux/pm_opp.h>
#include <linux/completion.h>
#include <uapi/linux/sched/types.h>
#include <trace/hooks/cpufreq.h>

#include "osi_topology.h"
#include "osi_freq.h"
#include "osi_base.h"


static inline int cpufreq_table_find_index(struct cpufreq_policy *policy,
					     unsigned int target_freq)
{
	if (policy->freq_table_sorted == CPUFREQ_TABLE_SORTED_ASCENDING)
		return cpufreq_table_find_index_al(policy, target_freq);
	else
		return cpufreq_table_find_index_dl(policy, target_freq);
}

void get_cpufreq_info(bool *is_sample)
{
	int cls;
	int start_cpu;
	int cur_idx, max_idx, min_idx, orig_max_id, orig_min_id;
	struct cpufreq_policy *pol;

	for (cls = 0; cls < cluster_num; cls++) {
		start_cpu = cluster[cls].start_cpu;
		pol = cpufreq_cpu_get(start_cpu);
		if (likely(pol)) {
			cur_idx = cpufreq_table_find_index(pol, pol->cur);
			min_idx = cpufreq_table_find_index(pol, pol->min);
			max_idx = cpufreq_table_find_index(pol, pol->max);
			orig_max_id = cpufreq_table_find_index(pol, pol->cpuinfo.max_freq);
			orig_min_id = cpufreq_table_find_index(pol, pol->cpuinfo.min_freq);
			osi_debug("cpu:%d,pol->cur:%d,idx:%d min_idx:%d,max_idx:%d,orig_min_idx:%d,orig_max_idx:%d",
				start_cpu, pol->cur, cur_idx, min_idx, max_idx, orig_min_id, orig_max_id);
#ifdef CONFIG_ARCH_MEDIATEK
			if ((max_idx < orig_min_id - 2) && (cur_idx <= min_idx - 2))
				*is_sample = true;
#else
			if ((max_idx > 2) && (cur_idx >= max_idx - 2))
				*is_sample = true;
#endif
			cpufreq_cpu_put(pol);
		}
	}
}

void jank_curr_freq_show(struct seq_file *m, u32 win_idx, u64 now)
{
}

/* target_freq > policy->max */
static void update_freq_count(struct cpufreq_policy *policy,
			u32 old_freq, u32 new_freq, u32 flags)
{
}

void jankinfo_update_freq_reach_limit_count(
			struct cpufreq_policy *policy,
			u32 old_target_freq, u32 new_target_freq, u32 flags)
{
	update_freq_count(policy, old_target_freq, new_target_freq, flags);
}

/* target_freq > policy->max */
void jank_burst_freq_show(struct seq_file *m, u32 win_idx, u64 now)
{
}

/* target_freq > policy->cur */
void jank_increase_freq_show(struct seq_file *m,
			u32 win_idx, u64 now)
{
}

struct freq_duration {
	u8  opp_num;
	u8  prev_index;
	u32 prev_freq;
	u32 *freq_table;
	u64 *duration_table;
	ktime_t prev_timestamp;
} freq_duration;

static bool transition_ready;
static DEFINE_PER_CPU(struct freq_duration, freq_duration_info);
static DEFINE_PER_CPU(int, opp_num);

void osi_cpufreq_transition_handler(struct cpufreq_policy *policy,  unsigned int new_freq)
{
	u8 cpu, idx, prev_index;
	ktime_t now;
	u64 time_delta;
	struct freq_duration *freq_duration;

	if (unlikely(!transition_ready))
		return;
	now = ktime_get();
	new_freq = policy->cur;
	cpu = cpumask_first(policy->related_cpus);

	freq_duration = per_cpu_ptr(&freq_duration_info, cpu);
	idx = cpufreq_table_find_index(policy, new_freq);
	prev_index = freq_duration->prev_index;
	if (unlikely((prev_index == idx) || idx >= per_cpu(opp_num, cpu)))
		return;
	freq_duration->freq_table[idx] = new_freq;
	time_delta = ktime_us_delta(now, freq_duration->prev_timestamp);
	freq_duration->duration_table[prev_index] = max(time_delta, freq_duration->duration_table[prev_index]);
	freq_duration->prev_timestamp = now;
	freq_duration->prev_freq = new_freq;
	freq_duration->prev_index = idx;
}

static void osi_opp_init(void)
{
	int  i, nr_caps, opp_idx;
	struct freq_duration *freq_duration;
	struct cpufreq_policy *pol;
	struct em_perf_domain *pd;

	for_each_possible_cpu(i) {
		struct device *cpu_dev = get_cpu_device(i);
		if (!cpu_dev)
			return;
		pol = cpufreq_cpu_get(i);
		pd = em_cpu_get(i);
		if (!pd || !pol) {
			pr_info("cpu_get return NULL for cpu#%d", i);
			continue;
		}
		nr_caps = pd->nr_perf_states;
		per_cpu(opp_num, i) = nr_caps;
		opp_idx = dev_pm_opp_get_opp_count(cpu_dev);
		pr_info("opp_num:%d, opp_idx:%d,i:%d", nr_caps, opp_idx, i);
		freq_duration = per_cpu_ptr(&freq_duration_info, i);
		freq_duration->opp_num = nr_caps;
		freq_duration->freq_table = kcalloc(nr_caps, sizeof(u32), GFP_KERNEL);
		freq_duration->duration_table = kcalloc(nr_caps, sizeof(u64), GFP_KERNEL);
		freq_duration->prev_timestamp = ktime_get();
		freq_duration->prev_index = cpufreq_table_find_index(pol, pol->cur);
		cpufreq_cpu_put(pol);
	}
	transition_ready = true;
}

static void osi_opp_exit(void)
{
	int  i;
	struct freq_duration *freq_duration;

	for_each_possible_cpu(i) {
		struct device *cpu_dev = get_cpu_device(i);
		if (!cpu_dev)
			return;
		freq_duration = per_cpu_ptr(&freq_duration_info, i);
		kfree(freq_duration->freq_table);
		kfree(freq_duration->duration_table);
	}
}

static void clear_freq_duration(struct task_struct *p)
{
	struct freq_duration *freq_duration;
	int im_flag = IM_FLAG_NONE;
	int cpu;

	if (p && p->group_leader)
		im_flag = oplus_get_im_flag(p->group_leader);
	if (im_flag == IM_FLAG_MIDASD) {
		pr_info("clear_freq_duration, %d, %s", p->pid, p->comm);
		for_each_possible_cpu(cpu) {
			if (!is_cluster_cpu(cpu))
				continue;
			freq_duration = per_cpu_ptr(&freq_duration_info, cpu);
			memset(freq_duration->duration_table, 0, freq_duration->opp_num);
		}
	}
}

static int proc_long_freq_duration_show(struct seq_file *m, void *v)
{
	int cpu, i;
	struct freq_duration *freq_duration;

	for_each_possible_cpu(cpu) {
		if (!is_cluster_cpu(cpu))
			continue;
		freq_duration = per_cpu_ptr(&freq_duration_info, cpu);
		seq_printf(m, "cpu(%d), %7s, %10s\n", cpu, "opp", "duration");
		for (i = 0; i < freq_duration->opp_num; i++) {
			seq_printf(m, "%6d, %7u, %10lu\n", i, freq_duration->freq_table[i],
				freq_duration->duration_table[i] >> 10);
		}
		seq_printf(m, "\n");
	}
	clear_freq_duration(current);
	return 0;
}

static int proc_long_freq_duration_open(struct inode *inode,
		struct file *file)
{
	return single_open(file, proc_long_freq_duration_show, inode);
}

static const struct proc_ops proc_long_freq_duration_operations = {
	.proc_open	=	proc_long_freq_duration_open,
	.proc_read	=	seq_read,
	.proc_lseek	=	seq_lseek,
	.proc_release   =	single_release,
};

static void osi_freq_proc_init(struct proc_dir_entry *pde)
{
	struct proc_dir_entry *entry = NULL;

	entry = proc_create("long_freq_duration", S_IRUGO | S_IWUGO,
				pde, &proc_long_freq_duration_operations);
	if (!entry) {
		osi_err("create long_freq_duration fail\n");
		return;
	}
}

static void osi_freq_proc_deinit(struct proc_dir_entry *pde)
{
	remove_proc_entry("long_freq_duration", pde);
}

int osi_freq_init(struct proc_dir_entry *pde)
{
	int ret = 0;
	osi_opp_init();
	osi_freq_proc_init(pde);
	return ret;
}

void osi_freq_exit(struct proc_dir_entry *pde)
{
	osi_opp_exit();
	osi_freq_proc_deinit(pde);
}

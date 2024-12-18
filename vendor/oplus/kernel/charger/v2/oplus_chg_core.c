#define pr_fmt(fmt) "[CORE]([%s][%d]): " fmt, __func__, __LINE__

#include <linux/module.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/of.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/device.h>
#ifndef CONFIG_DISABLE_OPLUS_FUNCTION
#include <soc/oplus/system/boot_mode.h>
#include <soc/oplus/system/oplus_project.h>
#endif
#ifdef CONFIG_OPLUS_CHARGER_MTK
#include <mtk_boot_common.h>
#endif

#include <oplus_chg_module.h>
#include <oplus_chg_ic.h>
#include <oplus_mms.h>

int oplus_log_level = LOG_LEVEL_INFO;
module_param(oplus_log_level, int, 0644);
MODULE_PARM_DESC(oplus_log_level, "debug log level");
EXPORT_SYMBOL(oplus_log_level);

int charger_abnormal_log = 0;

int oplus_is_rf_ftm_mode(void)
{
#ifndef CONFIG_DISABLE_OPLUS_FUNCTION
	int boot_mode = get_boot_mode();
#ifdef CONFIG_OPLUS_CHARGER_MTK
	struct device_node * of_chosen = NULL;
	char *bootargs = NULL;

	of_chosen = of_find_node_by_path("/chosen");

	if (boot_mode == META_BOOT || boot_mode == FACTORY_BOOT ||
	    boot_mode == ADVMETA_BOOT || boot_mode == ATE_FACTORY_BOOT) {
		chg_debug(" boot_mode:%d, return\n", boot_mode);
		if (of_chosen) {
			/* Add for MTK FTM AGING DDR test mode, if FTM AGING mode, enable charging.
			If Qcom platform want enable this feature, need resubmit issue */
			bootargs = (char *)of_get_property(of_chosen, "bootargs", NULL);
			if (!bootargs)
				chg_err("%s: failed to get bootargs\n", __func__);
			else {
				chg_debug("%s: bootargs: %s\n", __func__, bootargs);
				if (strstr(bootargs, "oplus_ftm_mode=ftmaging")) {
					chg_debug("%s: ftmaging!\n", __func__);
					return false;
				} else {
					chg_debug("%s: not ftmaging!\n", __func__);
				}
			}
		} else {
			chg_err("%s: failed to get /chosen \n", __func__);
		}
		return true;
	} else {
		/*chg_debug(" boot_mode:%d, return false\n",boot_mode);*/
		return false;
	}
#else
	if (boot_mode == MSM_BOOT_MODE__RF ||
	    boot_mode == MSM_BOOT_MODE__WLAN ||
	    boot_mode == MSM_BOOT_MODE__FACTORY) {
		chg_debug(" boot_mode:%d, return\n", boot_mode);
		return true;
	} else {
		/*chg_debug(" boot_mode:%d, return false\n",boot_mode);*/
		return false;
	}
#endif
#else /*CONFIG_DISABLE_OPLUS_FUNCTION*/
	return false;
#endif /*CONFIG_DISABLE_OPLUS_FUNCTION*/
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0))
#ifndef CONFIG_DISABLE_OPLUS_FUNCTION
bool __attribute__((weak)) qpnp_is_charger_reboot(void);
bool __attribute__((weak)) qpnp_is_power_off_charging(void);
#endif /*CONFIG_DISABLE_OPLUS_FUNCTION*/
#else
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0))
/* only for GKI compile */
bool __attribute__((weak)) qpnp_is_charger_reboot(void)
{
	return false;
}

bool __attribute__((weak)) qpnp_is_power_off_charging(void)
{
	return false;
}
#endif
#endif

bool oplus_is_power_off_charging(void)
{
#ifndef CONFIG_DISABLE_OPLUS_FUNCTION
#ifdef CONFIG_OPLUS_CHARGER_MTK
	if (get_boot_mode() == KERNEL_POWER_OFF_CHARGING_BOOT) {
		return true;
	} else {
		return false;
	}
#else
	return qpnp_is_power_off_charging();
#endif
#else /*CONFIG_DISABLE_OPLUS_FUNCTION*/
	return false;
#endif /*CONFIG_DISABLE_OPLUS_FUNCTION*/
}

bool oplus_is_charger_reboot(void)
{
#ifndef CONFIG_DISABLE_OPLUS_FUNCTION
#ifdef CONFIG_OPLUS_CHARGER_MTK
	/*TODO
	int charger_type;

	charger_type = oplus_chg_get_chg_type();
	if (charger_type == 5) {
		chg_debug("dont need check fw_update\n");
		return true;
	} else {
		return false;
	}*/
	return false;
#else
	return qpnp_is_charger_reboot();
#endif
#else /*CONFIG_DISABLE_OPLUS_FUNCTION*/
	return false;
#endif /*CONFIG_DISABLE_OPLUS_FUNCTION*/
}

struct timespec oplus_current_kernel_time(void)
{
	struct timespec ts;
	getnstimeofday(&ts);
	return ts;
}

bool oplus_is_ptcrb_version(void)
{
#ifndef CONFIG_DISABLE_OPLUS_FUNCTION
#ifndef CONFIG_OPLUS_CHARGER_MTK
	return (get_eng_version() == PTCRB);
#else
	return false;
#endif
#else /*CONFIG_DISABLE_OPLUS_FUNCTION*/
	return false;
#endif /*CONFIG_DISABLE_OPLUS_FUNCTION*/
}

#ifdef MODULE
__attribute__((weak)) size_t __oplus_chg_module_start;
__attribute__((weak)) size_t __oplus_chg_module_end;

static int oplus_chg_get_module_num(void)
{
	size_t addr_size = (size_t)&__oplus_chg_module_end -
			   (size_t)&__oplus_chg_module_start;

	if (addr_size == 0)
		return 0;
	if (addr_size % sizeof(struct oplus_chg_module) != 0) {
		chg_err("oplus chg module address is error, please check oplus_chg_module.lds\n");
		return 0;
	}

	return (addr_size / sizeof(struct oplus_chg_module));
}

int oplus_get_chg_spec_version(void)
{
	struct device_node *node;
	static int oplus_chg_spec_ver = -EINVAL;
	int rc;

	if (oplus_chg_spec_ver == -EINVAL) {
		node = of_find_node_by_path("/soc/oplus_chg_core");
		if (node != NULL) {
			rc = of_property_read_u32(node, "oplus,chg_spec_version",
						  &oplus_chg_spec_ver);
			if (rc < 0) {
				chg_err("get oplus,vooc_spec_version property error\n");
				oplus_chg_spec_ver = OPLUS_CHG_SPEC_VER_V3P6;
			}
			chg_info("oplus_chg_spec_ver = %d\n", oplus_chg_spec_ver);
		} else {
			chg_err("not found oplus_chg_core node\n");
			oplus_chg_spec_ver = OPLUS_CHG_SPEC_VER_V3P6;
			rc = -ENODEV;
		}
	}

	return oplus_chg_spec_ver;
}

static struct oplus_chg_module *oplus_chg_find_first_module(void)
{
	size_t start_addr = (size_t)&__oplus_chg_module_start;
	return (struct oplus_chg_module *)READ_ONCE_NOCHECK(start_addr);
}
#endif /* MODULE */

static int __init oplus_chg_class_init(void)
{
#ifdef MODULE
	int rc;
	int module_num, i;
	struct oplus_chg_module *first_module;
	struct oplus_chg_module *oplus_module;

#if __and(IS_MODULE(CONFIG_OPLUS_CHG), IS_MODULE(CONFIG_OPLUS_CHG_V2))
	struct device_node *node;

	node = of_find_node_by_path("/soc/oplus_chg_core");
	if (node == NULL)
		return 0;
	if (!of_property_read_bool(node, "oplus,chg_framework_v2"))
		return 0;
#endif /* CONFIG_OPLUS_CHG_V2 */

	module_num = oplus_chg_get_module_num();
	if (module_num == 0) {
		chg_err("oplus chg module not found, please check oplus_chg_module.lds\n");
		return 0;
	} else {
		chg_info("find %d oplus chg module\n", module_num);
	}
	first_module = oplus_chg_find_first_module();
	for (i = 0; i < module_num; i++) {
		oplus_module = &first_module[i];
		if ((oplus_module->magic == OPLUS_CHG_MODEL_MAGIC) &&
		    (oplus_module->chg_module_init != NULL)) {
			chg_info("%s init\n", oplus_module->name);
			rc = oplus_module->chg_module_init();
			if (rc < 0) {
				chg_err("%s init error, rc=%d\n",
					oplus_module->name, rc);
				goto module_init_err;
			}
		}
	}
#endif /* MODULE */
	return 0;

#ifdef MODULE
module_init_err:
	for (i = i - 1; i >= 0; i--) {
		oplus_module = &first_module[i];
		if ((oplus_module->magic == OPLUS_CHG_MODEL_MAGIC) &&
		    (oplus_module->chg_module_exit != NULL))
			oplus_module->chg_module_exit();
	}

	return rc;
#endif /* MODULE */
}

static void __exit oplus_chg_class_exit(void)
{
#ifdef MODULE
	int module_num, i;
	struct oplus_chg_module *first_module;
	struct oplus_chg_module *oplus_module;

#if __and(IS_MODULE(CONFIG_OPLUS_CHG), IS_MODULE(CONFIG_OPLUS_CHG_V2))
	struct device_node *node;

	node = of_find_node_by_path("/soc/oplus_chg_core");
	if (node == NULL)
		return;
	if (!of_property_read_bool(node, "oplus,chg_framework_v2"))
		return;
#endif /* CONFIG_OPLUS_CHG_V2 */

	module_num = oplus_chg_get_module_num();
	first_module = oplus_chg_find_first_module();
	for (i = module_num - 1; i >= 0; i--) {
		oplus_module = &first_module[i];
		if ((oplus_module->magic == OPLUS_CHG_MODEL_MAGIC) &&
		    (oplus_module->chg_module_exit != NULL))
			oplus_module->chg_module_exit();
	}
#endif /* MODULE */
}

subsys_initcall(oplus_chg_class_init);
module_exit(oplus_chg_class_exit);

MODULE_DESCRIPTION("oplus charge management subsystem");
MODULE_LICENSE("GPL");

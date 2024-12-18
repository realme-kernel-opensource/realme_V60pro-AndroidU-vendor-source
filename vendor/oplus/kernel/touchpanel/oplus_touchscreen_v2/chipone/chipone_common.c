
#include <linux/module.h>
#include "chipone_common.h"

static int cts_before_autotest(struct seq_file *s, struct touchpanel_data *ts,
	struct auto_testdata *p_cts_testdata)
{
	const struct firmware *fw = NULL;
	struct auto_test_header *test_head = NULL;
	uint32_t *p_data32 = NULL;

	TPD_INFO("%s: enter\n", __func__);
	fw = ts->com_test_data.limit_fw;

	test_head = (struct auto_test_header *)fw->data;
	p_data32 = (uint32_t *)(fw->data + 16);

	if ((test_head->magic1 != Limit_MagicNum1) || (test_head->magic2 != Limit_MagicNum2)) {
		TPD_INFO("limit image is not generated by oplus\n");
		seq_printf(s, "limit image is not generated by oplus\n");
		//@@ return  -1;
	}

	TPD_INFO("@@ magic1 num: 0x%x\n", test_head->magic1);
	TPD_INFO("@@ magic2 num: 0x%x\n", test_head->magic2);
	TPD_INFO("@@ test item: 0x%llx\n", test_head->test_item);
	
	TPD_INFO("current test item: %llx\n", test_head->test_item);
	p_cts_testdata->tx_num = ts->hw_res.tx_num;
	p_cts_testdata->rx_num = ts->hw_res.rx_num;
	p_cts_testdata->irq_gpio = ts->hw_res.irq_gpio;
	p_cts_testdata->tp_fw = ts->panel_data.tp_fw;
	p_cts_testdata->fp = ts->com_test_data.result_data;
	p_cts_testdata->length = ts->com_test_data.result_max_len;
	p_cts_testdata->pos = &ts->com_test_data.result_cur_len;
	p_cts_testdata->fw = fw;
	p_cts_testdata->test_item = test_head->test_item;
	return 0;
}

static int cts_doing_autotest(struct seq_file *s, struct touchpanel_data *ts,
	struct auto_testdata *p_cts_testdata)
{
	int error_count = 0;
	int ret = 0;
	struct test_item_info *p_test_item_info = NULL;
	struct cts_auto_test_operations *cts_test_ops = NULL;
	struct com_test_data *com_test_data_p = NULL;

	com_test_data_p = &ts->com_test_data;

	if (!com_test_data_p || !com_test_data_p->chip_test_ops) {
		TPD_INFO("%s: com_test_data is null\n", __func__);
		error_count++;
		goto end;
	}

	cts_test_ops = (struct cts_auto_test_operations *)com_test_data_p->chip_test_ops;

	if (!cts_test_ops->auto_test_preoperation) {
		TPD_INFO("not support ops->auto_test_preoperation callback\n");
	} else {
		ret = cts_test_ops->auto_test_preoperation(s, ts->chip_data, p_cts_testdata,
			p_test_item_info);
		if (ret) {
			TPD_INFO("auto_test_preoperation failed\n");
			error_count++;
			goto end;
		}
	}

	if (!cts_test_ops->test1) {
		TPD_INFO("not support ops->test1 callback\n");
	} else {
		ret = cts_test_ops->test1(s, ts->chip_data, p_cts_testdata, p_test_item_info);
		if (ret) {
			TPD_INFO("test1 failed(%d)!\n", ret);
			error_count++;
		}
	}
	tp_kfree((void **)&p_test_item_info);

	if (!cts_test_ops->test2) {
		TPD_INFO("not support ops->test2 callback\n");
	} else {
		ret = cts_test_ops->test2(s, ts->chip_data, p_cts_testdata, p_test_item_info);
		if (ret) {
			TPD_INFO("test2 failed(%d)!\n", ret);
			error_count++;
		}
	}
	tp_kfree((void **)&p_test_item_info);


	if (!cts_test_ops->test3) {
		TPD_INFO("not support ops->test3 callback\n");
	} else {
		ret = cts_test_ops->test3(s, ts->chip_data, p_cts_testdata, p_test_item_info);
		if (ret) {
			TPD_INFO("test3 failed(%d)!\n", ret);
			error_count++;
		}
	}
	tp_kfree((void **)&p_test_item_info);

	if (!cts_test_ops->test4) {
		TPD_INFO("not support ops->test4 callback\n");

	} else {
		ret = cts_test_ops->test4(s, ts->chip_data, p_cts_testdata, p_test_item_info);

		if (ret) {
			TPD_INFO("test4 failed(%d)!\n", ret);
			error_count++;
		}
	}
	tp_kfree((void **)&p_test_item_info);

	if (!cts_test_ops->test5) {
		TPD_INFO("not support ops->test5 callback\n");

	} else {
		ret = cts_test_ops->test5(s, ts->chip_data, p_cts_testdata, p_test_item_info);
		if (ret) {
			TPD_INFO("test5 failed(%d)!\n", ret);
			error_count++;
		}
	}
	tp_kfree((void **)&p_test_item_info);

	if (!cts_test_ops->test6) {
		TPD_INFO("not support ops->test6 callback\n");
	} else {
		ret = cts_test_ops->test6(s, ts->chip_data, p_cts_testdata, p_test_item_info);
		if (ret) {
			TPD_INFO("test6 failed(%d)!\n", ret);
			error_count++;
		}
	}
	tp_kfree((void **)&p_test_item_info);

	if (!cts_test_ops->test7) {
		TPD_INFO("not support ops->test7 callback\n");

	} else {
		ret = cts_test_ops->test7(s, ts->chip_data, p_cts_testdata, p_test_item_info);
		if (ret) {
			TPD_INFO("test7 failed(%d)!\n", ret);
			error_count++;
		}
	}
	tp_kfree((void **)&p_test_item_info);

	if (!cts_test_ops->auto_test_endoperation) {
		TPD_INFO("not support ops->auto_test_preoperation callback\n");

	} else {
		ret = cts_test_ops->auto_test_endoperation(s, ts->chip_data, p_cts_testdata,
			p_test_item_info);
		if (ret) {
			TPD_INFO("auto_test_endoperation failed\n");
			error_count++;
		}
	}

end:
	return error_count;
}

int cts_auto_test(struct seq_file *s, struct touchpanel_data *ts)
{
	struct auto_testdata cts_testdata = {
		.tx_num = 0,
		.rx_num = 0,
		.fp = NULL,
		.irq_gpio = -1,
		.tp_fw = 0,
		.fw = NULL,
		.test_item = 0,
	};
	int error_count = 0;
	int ret = 0;

	TPD_INFO("%s + \n", __func__);
	ret = cts_before_autotest(s, ts, &cts_testdata);

	if (ret) {
		error_count++;
		goto END;
	}

	error_count += cts_doing_autotest(s, ts, &cts_testdata);

END:
	seq_printf(s, "imageid = 0x%llx, deviceid = 0x%llx\n", cts_testdata.tp_fw,
		   cts_testdata.dev_tp_fw);
	seq_printf(s, "%d error(s). %s\n", error_count,
		   error_count ? "" : "All test passed.");
	TPD_INFO(" TP auto test %d error(s). %s\n", error_count,
		 error_count ? "" : "All test passed.");
	
	TPD_INFO("%s - \n", __func__);
	return error_count;
}
EXPORT_SYMBOL(cts_auto_test);

static int cts_before_black_screen_autotest(struct seq_file *s,
	struct touchpanel_data *ts, struct auto_testdata *p_cts_testdata)
{
	const struct firmware *fw = NULL;
	struct auto_test_header *test_head = NULL;
	uint32_t *p_data32 = NULL;

	TPD_INFO("%s + \n", __func__);
	
	fw = ts->com_test_data.limit_fw;

	test_head = (struct auto_test_header *)fw->data;
	p_data32 = (uint32_t *)(fw->data + 16);

	if ((test_head->magic1 != Limit_MagicNum1) || (test_head->magic2 != Limit_MagicNum2)) {
		TPD_INFO("limit image is not generated by oplus\n");
		/*seq_printf(s, "limit image is not generated by oplus\n");*/
		return  -1;
	}

	TPD_INFO("current test item: %llx\n", test_head->test_item);
	p_cts_testdata->tx_num = ts->hw_res.tx_num;
	p_cts_testdata->rx_num = ts->hw_res.rx_num;
	p_cts_testdata->irq_gpio = ts->hw_res.irq_gpio;
	p_cts_testdata->tp_fw = ts->panel_data.tp_fw;
	p_cts_testdata->fp = ts->com_test_data.bs_result_data;
	p_cts_testdata->length = ts->com_test_data.bs_result_max_len;
	p_cts_testdata->pos = &ts->com_test_data.bs_result_cur_len;
	p_cts_testdata->fw = fw;
	p_cts_testdata->test_item = test_head->test_item;
	
	TPD_INFO("%s - \n", __func__);
	
	return 0;
}

static int cts_doing_black_screen_autotest(struct seq_file *s,
	struct touchpanel_data *ts, struct auto_testdata *p_cts_testdata)
{
	int error_count = 0;
	int ret = 0;
	struct test_item_info *p_test_item_info = NULL;
	struct cts_auto_test_operations *cts_test_ops = NULL;
	struct com_test_data *com_test_data_p = NULL;

	TPD_INFO("%s + \n", __func__);
	
	com_test_data_p = &ts->com_test_data;

	if (!com_test_data_p || !com_test_data_p->chip_test_ops) {
		TPD_INFO("%s: com_test_data is null\n", __func__);
		error_count++;
		goto END;
	}

	cts_test_ops = (struct cts_auto_test_operations *)com_test_data_p->chip_test_ops;

	if (!cts_test_ops->black_screen_test_preoperation) {
		TPD_INFO("not support ops->black_screen_test_preoperation callback\n");

	} else {
		ret = cts_test_ops->black_screen_test_preoperation(s, ts->chip_data,
			p_cts_testdata, p_test_item_info);
		if (ret) {
			TPD_INFO("black_screen_test_preoperation failed\n");
			error_count++;
			goto END;
		}
	}

	if (!cts_test_ops->black_screen_test1) {
		TPD_INFO("not support ops->black_screen_test1 callback\n");
	} else {
		ret = cts_test_ops->black_screen_test1(s, ts->chip_data, p_cts_testdata,
			p_test_item_info);
		if (ret) {
			TPD_INFO("black_screen_test1 failed(%d)!\n", ret);
			error_count++;
		}
	}
	tp_kfree((void **)&p_test_item_info);


	if (!cts_test_ops->black_screen_test2) {
		TPD_INFO("not support ops->black_screen_test2 callback\n");
	} else {
		ret = cts_test_ops->black_screen_test2(s, ts->chip_data, p_cts_testdata,
			p_test_item_info);
		if (ret) {
			TPD_INFO("black_screen_test2 failed(%d)!\n", ret);
			error_count++;
		}
	}
	tp_kfree((void **)&p_test_item_info);

	if (!cts_test_ops->black_screen_test3) {
		TPD_INFO("not support ops->black_screen_test3 callback\n");
	} else {
		ret = cts_test_ops->black_screen_test3(s, ts->chip_data, p_cts_testdata,
			p_test_item_info);
		if (ret) {
			TPD_INFO("black_screen_test3 failed(%d)!\n", ret);
			error_count++;
		}
	}
	tp_kfree((void **)&p_test_item_info);

	if (!cts_test_ops->black_screen_test4) {
		TPD_INFO("not support ops->black_screen_test4 callback\n");
	} else {
		ret = cts_test_ops->black_screen_test4(s, ts->chip_data, p_cts_testdata,
			p_test_item_info);
		if (ret) {
			TPD_INFO("black_screen_test4 failed(%d)!\n", ret);
			error_count++;
		}
	}
	tp_kfree((void **)&p_test_item_info);

	if (!cts_test_ops->black_screen_test_endoperation) {
		TPD_INFO("not support ops->black_screen_test_endoperation callback\n");

	} else {
		ret = cts_test_ops->black_screen_test_endoperation(s, ts->chip_data,
			p_cts_testdata, p_test_item_info);
		if (ret) {
			TPD_INFO("auto_test_endoperation failed\n");
			error_count++;
		}
	}

	TPD_INFO("%s - \n", __func__);
END:
	return error_count;
}


#define LEN_BLACK_SCREEN_TEST_ALLOC		128
int cts_black_screen_autotest(struct black_gesture_test *p,
	struct touchpanel_data *ts)
{
	char buf[LEN_BLACK_SCREEN_TEST_ALLOC] = {0};
	struct auto_testdata cts_testdata = {
		.tx_num = 0,
		.rx_num = 0,
		.fp = NULL,
		.irq_gpio = -1,
		.tp_fw = 0,
		.fw = NULL,
		.test_item = 0,
	};
	int error_count = 0;
	int ret = 0;

	TPD_INFO("%s + \n", __func__);
	
	ret = cts_before_black_screen_autotest(NULL, ts, &cts_testdata);

	if (ret) {
		error_count++;
		goto end;
	}

	error_count += cts_doing_black_screen_autotest(NULL, ts, &cts_testdata);
	//error_count = 0;
end:
	snprintf(p->message, MESSAGE_SIZE, "%d error(s). %s%s\n", error_count,
		 error_count ? "" : "All test passed.", buf);
	TPD_INFO("%d errors. %s", error_count, buf);
	TPD_INFO(" TP auto test %d error(s). %s\n", error_count,
		 error_count ? "" : "All test passed.");
	
	TPD_INFO("%s - \n", __func__);
	return error_count;
}
EXPORT_SYMBOL(cts_black_screen_autotest);

MODULE_DESCRIPTION("Chipone-tddi common Driver");
MODULE_LICENSE("GPL");


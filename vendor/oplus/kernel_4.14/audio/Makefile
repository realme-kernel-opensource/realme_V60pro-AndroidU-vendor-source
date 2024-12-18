#
# Copyright (C) 2015 MediaTek Inc.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#

# Zhao.Pan@MULTIMEDIA.AUDIODRIVER.MACHINE, 2020/10/10, add for audiohal feedback
obj-y += driver/
# Zhao.Pan@MULTIMEDIA.AUDIODRIVER.SMARTPA, 2020/04/28, add for aw87339 PA
#obj-y += aw87339/
# Jingjing.Jiang@MULTIMEDIA.AUDIODRIVER.SMARTPA, 2020/05/19, add for tfa98xx smartPA
obj-$(CONFIG_SND_SOC_TFA9874) += tfa98xx/
# Yongpei.Yao@MULTIMEDIA.AUDIODRIVER.HEADSET, 2021/02/23, add for fsa switch
obj-$(CONFIG_SND_SOC_FSA) += fsa44xx/
# Jianqing.Liao@MULTIMEDIA.AUDIODRIVER.TFA_HAPTIC, 2021/04/15, add for tfa haptic
obj-$(CONFIG_SND_SOC_TFA_HAPTIC) += tfa_haptic/
# Xianliang.Zhong@MULTIMEDIA.AUDIODRIVER.SMARTPA, 2021/08/17, move sia81XX driver folder to oplus/kernel-4.14
obj-$(CONFIG_SIA_PA_ALGO) += sia81xx/
# Haoyun.Luo@MULTIMEDIA.AUDIODRIVER.SMARTPA, 2021/08/27, move aw87339 driver folder to oplus/kernel-4.14
obj-$(CONFIG_SND_SOC_CODEC_AW87339) += aw87339/
# Tao.Ban@MULTIMEDIA.AUDIODRIVER.SMARTPA, 2022/01/11, add for aw882xx_v1.10.0 driver
obj-$(CONFIG_SND_SOC_CODEC_AW882XX) += aw882xx_v1.10.0/

obj-$(CONFIG_SND_SOC_AW87XXX) += awinic/
obj-$(CONFIG_SND_SOC_OPLUS_PA_MANAGER) += oplus_speaker_manager/oplus_speaker_manager.o

# Wang.kun@MULTIMEDIA.AUDIODRIVER.SMARTPA, 2020/12/16, add for aw882xx smartPA
obj-$(CONFIG_SND_SOC_CODEC_AW88264) += aw882xx/

# Jingjing.Jiang@PSW.MM.AudioDriver.feature, 2020/11/02, add for dbmdx soundtrigger
obj-$(CONFIG_SND_SOC_DBMDX) += dbmdx/

# wangkangming@MULTIMEDIA.AUDIODRIVER.SMARTPA，2022/04/06, move aw87418 driver folder to oplus/kernel-4.14
obj-$(CONFIG_SND_SOC_CODEC_AW87418) += aw87xxx/
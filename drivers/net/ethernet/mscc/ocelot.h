/* SPDX-License-Identifier: (GPL-2.0 OR MIT) */
/*
 * Microsemi Ocelot Switch driver
 *
 * Copyright (c) 2017 Microsemi Corporation
 */

#ifndef _MSCC_OCELOT_H_
#define _MSCC_OCELOT_H_

#include <linux/bitops.h>
#include <linux/etherdevice.h>
#include <linux/if_vlan.h>
#include <linux/phy.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/ptp_clock_kernel.h>
#include <linux/net_tstamp.h>

#include "ocelot_ana.h"
#include "ocelot_dev.h"
#include "ocelot_hsio.h"
#include "ocelot_ptp.h"
#include "ocelot_qsys.h"
#include "ocelot_rew.h"
#include "ocelot_sys.h"
#include "ocelot_qs.h"
#include "ocelot_dev_gmii.h"

#define PGID_AGGR    64
#define PGID_SRC     80

/* Reserved PGIDs */
#define PGID_CPU     (PGID_AGGR - 5)
#define PGID_UC      (PGID_AGGR - 4)
#define PGID_MC      (PGID_AGGR - 3)
#define PGID_MCIPV4  (PGID_AGGR - 2)
#define PGID_MCIPV6  (PGID_AGGR - 1)
#define PGID_MCRED   (PGID_CPU - 20)

#define OCELOT_BUFFER_CELL_SZ 60

#define OCELOT_STATS_CHECK_DELAY (2 * HZ)

#define IFH_LEN 4

/* Length for long prefix header used for frame injection/extraction */
#define XFH_LONG_PREFIX_LEN 32

struct frame_info {
	u32 len;
	u16 port;
	u16 vid;
	u8 cpuq;
	u8 tag_type;
};

#define IFH_INJ_BYPASS	BIT(31)
#define IFH_INJ_POP_CNT_DISABLE (3 << 28)

#define IFH_TAG_TYPE_C 0
#define IFH_TAG_TYPE_S 1

#define OCELOT_SPEED_2500 0
#define OCELOT_SPEED_1000 1
#define OCELOT_SPEED_100  2
#define OCELOT_SPEED_10   3

#define TARGET_OFFSET 24
#define REG_MASK GENMASK(TARGET_OFFSET - 1, 0)
#define REG(reg, offset) [reg & REG_MASK] = offset

#define REG_RESERVED_ADDR	0xffffffff
#define REG_RESERVED(reg)	REG(reg, REG_RESERVED_ADDR)

enum ocelot_target {
	ANA = 1,
	PTP,
	QS,
	QSYS,
	REW,
	SYS,
	S2,
	GCB,
	HSIO,
	TARGET_MAX,
};

enum ocelot_reg {
	ANA_ADVLEARN = ANA << TARGET_OFFSET,
	ANA_VLANMASK,
	ANA_PORT_B_DOMAIN,
	ANA_ANAGEFIL,
	ANA_ANEVENTS,
	ANA_STORMLIMIT_BURST,
	ANA_STORMLIMIT_CFG,
	ANA_ISOLATED_PORTS,
	ANA_COMMUNITY_PORTS,
	ANA_AUTOAGE,
	ANA_MACTOPTIONS,
	ANA_LEARNDISC,
	ANA_AGENCTRL,
	ANA_MIRRORPORTS,
	ANA_EMIRRORPORTS,
	ANA_FLOODING,
	ANA_FLOODING_IPMC,
	ANA_SFLOW_CFG,
	ANA_PORT_MODE,
	ANA_CUT_THRU_CFG,
	ANA_PGID_PGID,
	ANA_TABLES_ANMOVED,
	ANA_TABLES_MACHDATA,
	ANA_TABLES_MACLDATA,
	ANA_TABLES_STREAMDATA,
	ANA_TABLES_MACACCESS,
	ANA_TABLES_MACTINDX,
	ANA_TABLES_VLANACCESS,
	ANA_TABLES_VLANTIDX,
	ANA_TABLES_ISDXACCESS,
	ANA_TABLES_ISDXTIDX,
	ANA_TABLES_ENTRYLIM,
	ANA_TABLES_PTP_ID_HIGH,
	ANA_TABLES_PTP_ID_LOW,
	ANA_TABLES_STREAMACCESS,
	ANA_TABLES_STREAMTIDX,
	ANA_TABLES_SEQ_HISTORY,
	ANA_TABLES_SEQ_MASK,
	ANA_TABLES_SFID_MASK,
	ANA_TABLES_SFIDACCESS,
	ANA_TABLES_SFIDTIDX,
	ANA_MSTI_STATE,
	ANA_OAM_UPM_LM_CNT,
	ANA_SG_ACCESS_CTRL,
	ANA_SG_CONFIG_REG_1,
	ANA_SG_CONFIG_REG_2,
	ANA_SG_CONFIG_REG_3,
	ANA_SG_CONFIG_REG_4,
	ANA_SG_CONFIG_REG_5,
	ANA_SG_GCL_GS_CONFIG,
	ANA_SG_GCL_TI_CONFIG,
	ANA_SG_STATUS_REG_1,
	ANA_SG_STATUS_REG_2,
	ANA_SG_STATUS_REG_3,
	ANA_PORT_VLAN_CFG,
	ANA_PORT_DROP_CFG,
	ANA_PORT_QOS_CFG,
	ANA_PORT_VCAP_CFG,
	ANA_PORT_VCAP_S1_KEY_CFG,
	ANA_PORT_VCAP_S2_CFG,
	ANA_PORT_PCP_DEI_MAP,
	ANA_PORT_CPU_FWD_CFG,
	ANA_PORT_CPU_FWD_BPDU_CFG,
	ANA_PORT_CPU_FWD_GARP_CFG,
	ANA_PORT_CPU_FWD_CCM_CFG,
	ANA_PORT_PORT_CFG,
	ANA_PORT_POL_CFG,
	ANA_PORT_PTP_CFG,
	ANA_PORT_PTP_DLY1_CFG,
	ANA_PORT_PTP_DLY2_CFG,
	ANA_PORT_SFID_CFG,
	ANA_PFC_PFC_CFG,
	ANA_PFC_PFC_TIMER,
	ANA_IPT_OAM_MEP_CFG,
	ANA_IPT_IPT,
	ANA_PPT_PPT,
	ANA_FID_MAP_FID_MAP,
	ANA_AGGR_CFG,
	ANA_CPUQ_CFG,
	ANA_CPUQ_CFG2,
	ANA_CPUQ_8021_CFG,
	ANA_DSCP_CFG,
	ANA_DSCP_REWR_CFG,
	ANA_VCAP_RNG_TYPE_CFG,
	ANA_VCAP_RNG_VAL_CFG,
	ANA_VRAP_CFG,
	ANA_VRAP_HDR_DATA,
	ANA_VRAP_HDR_MASK,
	ANA_DISCARD_CFG,
	ANA_FID_CFG,
	ANA_POL_PIR_CFG,
	ANA_POL_CIR_CFG,
	ANA_POL_MODE_CFG,
	ANA_POL_PIR_STATE,
	ANA_POL_CIR_STATE,
	ANA_POL_STATE,
	ANA_POL_FLOWC,
	ANA_POL_HYST,
	ANA_POL_MISC_CFG,
	PTP_MISC_CFG = PTP << TARGET_OFFSET,
	PTP_CLK_ADJ_CFG,
	PTP_CLK_ADJ_FRQ,
	PTP_PIN_INTR,
	PTP_PIN_INTR_ENA,
	PTP_INTR_IDENT,
	PTP_SYS_CLK_CFG,
	PTP_CUR_NSF,
	PTP_CUR_NSEC,
	PTP_CUR_SEC_LSB,
	PTP_CUR_SEC_MSB,
	PTP_PIN_CFG,
	PTP_TOD_SEC_MSB,
	PTP_TOD_SEC_LSB,
	PTP_TOD_NSEC,
	PTP_NSF,
	PTP_PIN_WF_HIGH_PERIOD,
	PTP_PIN_WF_LOW_PERIOD,
	QS_XTR_GRP_CFG = QS << TARGET_OFFSET,
	QS_XTR_RD,
	QS_XTR_FRM_PRUNING,
	QS_XTR_FLUSH,
	QS_XTR_DATA_PRESENT,
	QS_XTR_CFG,
	QS_INJ_GRP_CFG,
	QS_INJ_WR,
	QS_INJ_CTRL,
	QS_INJ_STATUS,
	QS_INJ_ERR,
	QS_INH_DBG,
	QSYS_PORT_MODE = QSYS << TARGET_OFFSET,
	QSYS_SWITCH_PORT_MODE,
	QSYS_STAT_CNT_CFG,
	QSYS_EEE_CFG,
	QSYS_EEE_THRES,
	QSYS_IGR_NO_SHARING,
	QSYS_EGR_NO_SHARING,
	QSYS_SW_STATUS,
	QSYS_EXT_CPU_CFG,
	QSYS_PAD_CFG,
	QSYS_CPU_GROUP_MAP,
	QSYS_QMAP,
	QSYS_ISDX_SGRP,
	QSYS_TIMED_FRAME_ENTRY,
	QSYS_TFRM_MISC,
	QSYS_TFRM_PORT_DLY,
	QSYS_TFRM_TIMER_CFG_1,
	QSYS_TFRM_TIMER_CFG_2,
	QSYS_TFRM_TIMER_CFG_3,
	QSYS_TFRM_TIMER_CFG_4,
	QSYS_TFRM_TIMER_CFG_5,
	QSYS_TFRM_TIMER_CFG_6,
	QSYS_TFRM_TIMER_CFG_7,
	QSYS_TFRM_TIMER_CFG_8,
	QSYS_RED_PROFILE,
	QSYS_RES_QOS_MODE,
	QSYS_RES_CFG,
	QSYS_RES_STAT,
	QSYS_EGR_DROP_MODE,
	QSYS_EQ_CTRL,
	QSYS_EVENTS_CORE,
	QSYS_QMAXSDU_CFG_0,
	QSYS_QMAXSDU_CFG_1,
	QSYS_QMAXSDU_CFG_2,
	QSYS_QMAXSDU_CFG_3,
	QSYS_QMAXSDU_CFG_4,
	QSYS_QMAXSDU_CFG_5,
	QSYS_QMAXSDU_CFG_6,
	QSYS_QMAXSDU_CFG_7,
	QSYS_PREEMPTION_CFG,
	QSYS_CIR_CFG,
	QSYS_EIR_CFG,
	QSYS_SE_CFG,
	QSYS_SE_DWRR_CFG,
	QSYS_SE_CONNECT,
	QSYS_SE_DLB_SENSE,
	QSYS_CIR_STATE,
	QSYS_EIR_STATE,
	QSYS_SE_STATE,
	QSYS_HSCH_MISC_CFG,
	QSYS_TAG_CONFIG,
	QSYS_TAS_PARAM_CFG_CTRL,
	QSYS_PORT_MAX_SDU,
	QSYS_PARAM_CFG_REG_1,
	QSYS_PARAM_CFG_REG_2,
	QSYS_PARAM_CFG_REG_3,
	QSYS_PARAM_CFG_REG_4,
	QSYS_PARAM_CFG_REG_5,
	QSYS_GCL_CFG_REG_1,
	QSYS_GCL_CFG_REG_2,
	QSYS_PARAM_STATUS_REG_1,
	QSYS_PARAM_STATUS_REG_2,
	QSYS_PARAM_STATUS_REG_3,
	QSYS_PARAM_STATUS_REG_4,
	QSYS_PARAM_STATUS_REG_5,
	QSYS_PARAM_STATUS_REG_6,
	QSYS_PARAM_STATUS_REG_7,
	QSYS_PARAM_STATUS_REG_8,
	QSYS_PARAM_STATUS_REG_9,
	QSYS_GCL_STATUS_REG_1,
	QSYS_GCL_STATUS_REG_2,
	REW_PORT_VLAN_CFG = REW << TARGET_OFFSET,
	REW_TAG_CFG,
	REW_PORT_CFG,
	REW_DSCP_CFG,
	REW_PCP_DEI_QOS_MAP_CFG,
	REW_PTP_CFG,
	REW_PTP_DLY1_CFG,
	REW_RED_TAG_CFG,
	REW_DSCP_REMAP_DP1_CFG,
	REW_DSCP_REMAP_CFG,
	REW_STAT_CFG,
	REW_REW_STICKY,
	REW_PPT,
	SYS_COUNT_RX_OCTETS = SYS << TARGET_OFFSET,
	SYS_COUNT_RX_UNICAST,
	SYS_COUNT_RX_MULTICAST,
	SYS_COUNT_RX_BROADCAST,
	SYS_COUNT_RX_SHORTS,
	SYS_COUNT_RX_FRAGMENTS,
	SYS_COUNT_RX_JABBERS,
	SYS_COUNT_RX_CRC_ALIGN_ERRS,
	SYS_COUNT_RX_SYM_ERRS,
	SYS_COUNT_RX_64,
	SYS_COUNT_RX_65_127,
	SYS_COUNT_RX_128_255,
	SYS_COUNT_RX_256_1023,
	SYS_COUNT_RX_1024_1526,
	SYS_COUNT_RX_1527_MAX,
	SYS_COUNT_RX_PAUSE,
	SYS_COUNT_RX_CONTROL,
	SYS_COUNT_RX_LONGS,
	SYS_COUNT_RX_CLASSIFIED_DROPS,
	SYS_COUNT_TX_OCTETS,
	SYS_COUNT_TX_UNICAST,
	SYS_COUNT_TX_MULTICAST,
	SYS_COUNT_TX_BROADCAST,
	SYS_COUNT_TX_COLLISION,
	SYS_COUNT_TX_DROPS,
	SYS_COUNT_TX_PAUSE,
	SYS_COUNT_TX_64,
	SYS_COUNT_TX_65_127,
	SYS_COUNT_TX_128_511,
	SYS_COUNT_TX_512_1023,
	SYS_COUNT_TX_1024_1526,
	SYS_COUNT_TX_1527_MAX,
	SYS_COUNT_TX_AGING,
	SYS_RESET_CFG,
	SYS_SR_ETYPE_CFG,
	SYS_VLAN_ETYPE_CFG,
	SYS_PORT_MODE,
	SYS_FRONT_PORT_MODE,
	SYS_FRM_AGING,
	SYS_STAT_CFG,
	SYS_SW_STATUS,
	SYS_MISC_CFG,
	SYS_REW_MAC_HIGH_CFG,
	SYS_REW_MAC_LOW_CFG,
	SYS_TIMESTAMP_OFFSET,
	SYS_CMID,
	SYS_PAUSE_CFG,
	SYS_PAUSE_TOT_CFG,
	SYS_ATOP,
	SYS_ATOP_TOT_CFG,
	SYS_MAC_FC_CFG,
	SYS_MMGT,
	SYS_MMGT_FAST,
	SYS_EVENTS_DIF,
	SYS_EVENTS_CORE,
	SYS_CNT,
	SYS_PTP_STATUS,
	SYS_PTP_TXSTAMP,
	SYS_PTP_NXT,
	SYS_PTP_CFG,
	SYS_RAM_INIT,
	SYS_CM_ADDR,
	SYS_CM_DATA_WR,
	SYS_CM_DATA_RD,
	SYS_CM_OP,
	SYS_CM_DATA,
	HSIO_PLL5G_CFG0 = HSIO << TARGET_OFFSET,
	HSIO_PLL5G_CFG1,
	HSIO_PLL5G_CFG2,
	HSIO_PLL5G_CFG3,
	HSIO_PLL5G_CFG4,
	HSIO_PLL5G_CFG5,
	HSIO_PLL5G_CFG6,
	HSIO_PLL5G_STATUS0,
	HSIO_PLL5G_STATUS1,
	HSIO_PLL5G_BIST_CFG0,
	HSIO_PLL5G_BIST_CFG1,
	HSIO_PLL5G_BIST_CFG2,
	HSIO_PLL5G_BIST_STAT0,
	HSIO_PLL5G_BIST_STAT1,
	HSIO_RCOMP_CFG0,
	HSIO_RCOMP_STATUS,
	HSIO_SYNC_ETH_CFG,
	HSIO_SYNC_ETH_PLL_CFG,
	HSIO_S1G_DES_CFG,
	HSIO_S1G_IB_CFG,
	HSIO_S1G_OB_CFG,
	HSIO_S1G_SER_CFG,
	HSIO_S1G_COMMON_CFG,
	HSIO_S1G_PLL_CFG,
	HSIO_S1G_PLL_STATUS,
	HSIO_S1G_DFT_CFG0,
	HSIO_S1G_DFT_CFG1,
	HSIO_S1G_DFT_CFG2,
	HSIO_S1G_TP_CFG,
	HSIO_S1G_RC_PLL_BIST_CFG,
	HSIO_S1G_MISC_CFG,
	HSIO_S1G_DFT_STATUS,
	HSIO_S1G_MISC_STATUS,
	HSIO_MCB_S1G_ADDR_CFG,
	HSIO_S6G_DIG_CFG,
	HSIO_S6G_DFT_CFG0,
	HSIO_S6G_DFT_CFG1,
	HSIO_S6G_DFT_CFG2,
	HSIO_S6G_TP_CFG0,
	HSIO_S6G_TP_CFG1,
	HSIO_S6G_RC_PLL_BIST_CFG,
	HSIO_S6G_MISC_CFG,
	HSIO_S6G_OB_ANEG_CFG,
	HSIO_S6G_DFT_STATUS,
	HSIO_S6G_ERR_CNT,
	HSIO_S6G_MISC_STATUS,
	HSIO_S6G_DES_CFG,
	HSIO_S6G_IB_CFG,
	HSIO_S6G_IB_CFG1,
	HSIO_S6G_IB_CFG2,
	HSIO_S6G_IB_CFG3,
	HSIO_S6G_IB_CFG4,
	HSIO_S6G_IB_CFG5,
	HSIO_S6G_OB_CFG,
	HSIO_S6G_OB_CFG1,
	HSIO_S6G_SER_CFG,
	HSIO_S6G_COMMON_CFG,
	HSIO_S6G_PLL_CFG,
	HSIO_S6G_ACJTAG_CFG,
	HSIO_S6G_GP_CFG,
	HSIO_S6G_IB_STATUS0,
	HSIO_S6G_IB_STATUS1,
	HSIO_S6G_ACJTAG_STATUS,
	HSIO_S6G_PLL_STATUS,
	HSIO_S6G_REVID,
	HSIO_MCB_S6G_ADDR_CFG,
	HSIO_HW_CFG,
	HSIO_HW_QSGMII_CFG,
	HSIO_HW_QSGMII_STAT,
	HSIO_CLK_CFG,
	HSIO_TEMP_SENSOR_CTRL,
	HSIO_TEMP_SENSOR_CFG,
	HSIO_TEMP_SENSOR_STAT,
	GCB_SOFT_RST = GCB << TARGET_OFFSET,
	S2_CORE_UPDATE_CTRL = S2 << TARGET_OFFSET,
	S2_CORE_MV_CFG,
	S2_CACHE_ENTRY_DAT,
	S2_CACHE_MASK_DAT,
	S2_CACHE_ACTION_DAT,
	S2_CACHE_CNT_DAT,
	S2_CACHE_TG_DAT,
};

enum ocelot_regfield {
	ANA_ADVLEARN_VLAN_CHK,
	ANA_ADVLEARN_LEARN_MIRROR,
	ANA_ANEVENTS_FLOOD_DISCARD,
	ANA_ANEVENTS_MSTI_DROP,
	ANA_ANEVENTS_ACLKILL,
	ANA_ANEVENTS_ACLUSED,
	ANA_ANEVENTS_AUTOAGE,
	ANA_ANEVENTS_VS2TTL1,
	ANA_ANEVENTS_STORM_DROP,
	ANA_ANEVENTS_LEARN_DROP,
	ANA_ANEVENTS_AGED_ENTRY,
	ANA_ANEVENTS_CPU_LEARN_FAILED,
	ANA_ANEVENTS_AUTO_LEARN_FAILED,
	ANA_ANEVENTS_LEARN_REMOVE,
	ANA_ANEVENTS_AUTO_LEARNED,
	ANA_ANEVENTS_AUTO_MOVED,
	ANA_ANEVENTS_DROPPED,
	ANA_ANEVENTS_CLASSIFIED_DROP,
	ANA_ANEVENTS_CLASSIFIED_COPY,
	ANA_ANEVENTS_VLAN_DISCARD,
	ANA_ANEVENTS_FWD_DISCARD,
	ANA_ANEVENTS_MULTICAST_FLOOD,
	ANA_ANEVENTS_UNICAST_FLOOD,
	ANA_ANEVENTS_DEST_KNOWN,
	ANA_ANEVENTS_BUCKET3_MATCH,
	ANA_ANEVENTS_BUCKET2_MATCH,
	ANA_ANEVENTS_BUCKET1_MATCH,
	ANA_ANEVENTS_BUCKET0_MATCH,
	ANA_ANEVENTS_CPU_OPERATION,
	ANA_ANEVENTS_DMAC_LOOKUP,
	ANA_ANEVENTS_SMAC_LOOKUP,
	ANA_ANEVENTS_SEQ_GEN_ERR_0,
	ANA_ANEVENTS_SEQ_GEN_ERR_1,
	ANA_TABLES_MACACCESS_B_DOM,
	ANA_TABLES_MACTINDX_BUCKET,
	ANA_TABLES_MACTINDX_M_INDEX,
	QSYS_TIMED_FRAME_ENTRY_TFRM_VLD,
	QSYS_TIMED_FRAME_ENTRY_TFRM_FP,
	QSYS_TIMED_FRAME_ENTRY_TFRM_PORTNO,
	QSYS_TIMED_FRAME_ENTRY_TFRM_TM_SEL,
	QSYS_TIMED_FRAME_ENTRY_TFRM_TM_T,
	SYS_RESET_CFG_CORE_ENA,
	SYS_RESET_CFG_MEM_ENA,
	SYS_RESET_CFG_MEM_INIT,
	GCB_SOFT_RST_SWC_RST,
	ANA_TABLES_STREAMDATA_SFID_0,
	ANA_TABLES_STREAMDATA_SFID_VALID_0,
	ANA_TABLES_SFIDTIDX_SFID_INDEX_0,
	ANA_SG_ACCESS_CTRL_CONFIG_CHANGE_0,
	ANA_SG_ACCESS_CTRL_SGID_0,
	ANA_SG_CONFIG_REG_3_GATE_ENABLE_0,
	QSYS_TAS_PARAM_CFG_CTRL_PORT_NUM_0,
	QSYS_GCL_STATUS_REG_1_GCL_ENTRY_NUM_0,
	QSYS_GCL_CFG_REG_1_GATE_STATE_0,
	QSYS_GCL_CFG_REG_1_GCL_ENTRY_NUM_0,
	QSYS_TAS_PARAM_CFG_CTRL_CONFIG_CHANGE_0,
	QSYS_TAG_CONFIG_ENABLE_0,
	SYS_STAT_CFG_STAT_VIEW_0,
	REGFIELD_MAX
};

struct ocelot_multicast {
	struct list_head list;
	unsigned char addr[ETH_ALEN];
	u16 vid;
	u16 ports;
};

struct ocelot_port;

enum ocelot_ptp_pins {
	ALT_PPS_PIN	= 1,
	EXT_CLK_PIN,
	ALT_LDST_PIN,
	TOD_ACC_PIN
};

struct ocelot_stat_layout {
	u32 offset;
	char name[ETH_GSTRING_LEN];
};

struct ocelot {
	struct device *dev;

	struct regmap *targets[TARGET_MAX];
	struct regmap_field *regfields[REGFIELD_MAX];
	const u32 *const *map;
	const struct ocelot_stat_layout *stats_layout;
	unsigned int num_stats;

	u8 base_mac[ETH_ALEN];

	struct net_device *hw_bridge_dev;
	u16 bridge_mask;
	u16 bridge_fwd_mask;

	struct workqueue_struct *ocelot_owq;

	int shared_queue_sz;

	u8 num_phys_ports;
	u8 num_cpu_ports;
	u8 cpu_port_id;
	struct ocelot_port **ports;

	u32 *lags;

	/* Keep track of the vlan port masks */
	u32 vlan_mask[VLAN_N_VID];

	struct list_head multicast;

	/* Workqueue to check statistics for overflow with its lock */
	struct mutex stats_lock;
	u64 *stats;
	struct delayed_work stats_work;
	struct workqueue_struct *stats_queue;
	struct workqueue_struct *ocelot_wq;
	struct work_struct irq_handle_work;

	struct list_head skbs;

	void (*port_pcs_init)(struct ocelot_port *port);
	struct net_device *cpu_port_ndev;

	struct ptp_clock_info ptp_caps;
	struct ptp_clock *clock;
	int phc_index;
};

struct ocelot_skb {
	struct list_head head;
	struct sk_buff *skb;
	u8 tstamp_id;
	u8 tx_port;
};

struct ocelot_port {
	struct net_device *dev;
	struct ocelot *ocelot;
	struct phy_device *phy;
	struct device_node *portnp;
	void __iomem *regs;
	u8 chip_port;
	/* Keep a track of the mc addresses added to the mac table, so that they
	 * can be removed when needed.
	 */
	struct list_head mc;

	/* Ingress default VLAN (pvid) */
	u16 pvid;

	/* Egress default VLAN (vid) */
	u16 vid;

	u8 vlan_aware;

	u64 *stats;
	phy_interface_t phy_mode;

	/* cpu frame injection handler */
	netdev_tx_t (*cpu_inj_handler)(struct sk_buff *skb,
				       struct net_device *dev);
	void *cpu_inj_handler_data;
	struct hwtstamp_config hwtstamp_config;
	bool tx_tstamp;
	bool rx_tstamp;
	u8 tstamp_id;
};

u32 __ocelot_read_ix(struct ocelot *ocelot, u32 reg, u32 offset);
#define ocelot_read_ix(ocelot, reg, gi, ri) __ocelot_read_ix(ocelot, reg, reg##_GSZ * (gi) + reg##_RSZ * (ri))
#define ocelot_read_gix(ocelot, reg, gi) __ocelot_read_ix(ocelot, reg, reg##_GSZ * (gi))
#define ocelot_read_rix(ocelot, reg, ri) __ocelot_read_ix(ocelot, reg, reg##_RSZ * (ri))
#define ocelot_read(ocelot, reg) __ocelot_read_ix(ocelot, reg, 0)

void __ocelot_write_ix(struct ocelot *ocelot, u32 val, u32 reg, u32 offset);
#define ocelot_write_ix(ocelot, val, reg, gi, ri) __ocelot_write_ix(ocelot, val, reg, reg##_GSZ * (gi) + reg##_RSZ * (ri))
#define ocelot_write_gix(ocelot, val, reg, gi) __ocelot_write_ix(ocelot, val, reg, reg##_GSZ * (gi))
#define ocelot_write_rix(ocelot, val, reg, ri) __ocelot_write_ix(ocelot, val, reg, reg##_RSZ * (ri))
#define ocelot_write(ocelot, val, reg) __ocelot_write_ix(ocelot, val, reg, 0)

void __ocelot_rmw_ix(struct ocelot *ocelot, u32 val, u32 mask, u32 reg,
		     u32 offset);
#define ocelot_rmw_ix(ocelot, val, m, reg, gi, ri) __ocelot_rmw_ix(ocelot, val, m, reg, reg##_GSZ * (gi) + reg##_RSZ * (ri))
#define ocelot_rmw_gix(ocelot, val, m, reg, gi) __ocelot_rmw_ix(ocelot, val, m, reg, reg##_GSZ * (gi))
#define ocelot_rmw_rix(ocelot, val, m, reg, ri) __ocelot_rmw_ix(ocelot, val, m, reg, reg##_RSZ * (ri))
#define ocelot_rmw(ocelot, val, m, reg) __ocelot_rmw_ix(ocelot, val, m, reg, 0)

u32 ocelot_port_readl(struct ocelot_port *port, u32 reg);
void ocelot_port_writel(struct ocelot_port *port, u32 val, u32 reg);

int ocelot_regfields_init(struct ocelot *ocelot,
			  const struct reg_field *const regfields);
struct regmap *ocelot_io_init(struct ocelot *ocelot, struct resource *res);

#define ocelot_field_write(ocelot, reg, val) regmap_field_write((ocelot)->regfields[(reg)], (val))
#define ocelot_field_read(ocelot, reg, val) regmap_field_read((ocelot)->regfields[(reg)], (val))

int ocelot_init(struct ocelot *ocelot);
void ocelot_deinit(struct ocelot *ocelot);
int ocelot_chip_init(struct ocelot *ocelot);
int felix_chip_init(struct ocelot *ocelot);
int ocelot_probe_port(struct ocelot *ocelot, u8 port,
		      void __iomem *regs,
		      struct phy_device *phy);

#ifdef CONFIG_MSCC_FELIX_SWITCH_PTP_CLOCK
int felix_ptp_init(struct ocelot *ocelot);
void felix_ptp_remove(struct ocelot *ocelot);
int felix_ptp_gettime(struct ptp_clock_info *ptp, struct timespec64 *ts);
#else
static inline int felix_ptp_init(struct ocelot *ocelot) { return 0; }
static inline void felix_ptp_remove(struct ocelot *ocelot) { }
static inline int felix_ptp_gettime(struct ptp_clock_info *ptp,
				    struct timespec64 *ts) { return 0; }
#endif

extern struct notifier_block ocelot_netdevice_nb;

#endif

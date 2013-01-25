/**
 * =====================================================================================
 *       @file  metrics.h
 *      @brief  
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  08/24/2010 05:23:36 PM 
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2010, TaoMee.Inc, ShangHai.
 *
 *     @author  luis (程龙), luis@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#ifndef H_METRICS_H_2010_08_24
#define H_METRICS_H_2010_08_24


/* CMDs currently supported */
#define ETHTOOL_GSET		0x00000001 /* Get settings. */
#ifndef SIOCETHTOOL
#define SIOCETHTOOL     0x8946
#endif

/* This should work for both 32 and 64 bit userland. */
struct ethtool_cmd {
	uint32_t cmd;
	uint32_t supported;	    /* Features this interface supports */
	uint32_t advertising;	/* Features this interface advertises */
	uint16_t speed;		    /* The forced speed, 10Mb, 100Mb, gigabit */
	uint8_t	 duplex;		/* Duplex, half or full */
	uint8_t	 port;		    /* Which connector port */
	uint8_t	 phy_address;
	uint8_t	 transceiver;	/* Which transceiver to use */
	uint8_t	 autoneg;	    /* Enable or disable autonegotiation */
	uint8_t	 mdio_support;
	uint32_t maxtxpkt;	    /* Tx pkts before generating tx int */
	uint32_t maxrxpkt;	    /* Rx pkts before generating rx int */
	uint16_t speed_hi;
	uint8_t	 eth_tp_mdix;
	uint8_t	 reserved2;
	uint32_t lp_advertising;/* Features the link partner advertises */
	uint32_t reserved[2];
};

int metric_init();

int pkts_in_func(c_value * val);
int pkts_out_func(c_value * val);
int bytes_in_func(c_value * val);
int eth0_recv_func(c_value * val);
int eth1_recv_func(c_value * val);
int bytes_out_func(c_value * val);
int eth0_send_func(c_value * val);
int eth1_send_func(c_value * val);
int mem_total_func(c_value * val);
int mem_free_func(c_value * val);
int mem_buffers_func(c_value * val);
int mem_cached_func(c_value * val);
int swap_free_func(c_value * val);
int swap_total_func(c_value * val);
int disk_total_func(c_value * val);
int disk_space_usage_func(c_value * val);
int disk_free_func(c_value * val);
int part_max_used_func(c_value * val);
int boottime_func(c_value * val);
int sys_clock_func(c_value * val);
int machine_type_func(c_value * val);
int os_name_func(c_value * val);
int os_release_func(c_value * val);
int file_system_func(c_value * val);
int proc_run_func(c_value * val);
int proc_total_func(c_value * val);
int load_one_func(c_value * val);
int load_five_func(c_value * val);
int load_fifteen_func(c_value * val);
int cpu_num_func(c_value * val);
int cpu_speed_func(c_value * val);
int cpu_user_func(c_value * val);
int cpu_nice_func(c_value * val);
int cpu_system_func(c_value * val);
int cpu_idle_func(c_value * val);
int cpu_aidle_func(c_value * val);
int cpu_wio_func(c_value * val);
int cpu_intr_func(c_value * val);
int cpu_sintr_func(c_value * val);
int inside_ip_func(c_value * val);
int outside_ip_func(c_value * val);
int cpu_info_func(c_value * val);
int os_version_func(c_value * val);
int server_pattern_func(c_value * val);
int mem_count_func(c_value * val);
int mem_size_func(c_value * val);
int ipmi_func(c_value * val);
int server_tag_func(c_value * val);
int system_time_offset_func(c_value * val);
int eth0_speed_func(c_value * val);
int eth1_speed_func(c_value * val);

#endif //H_METRICS_H_2010_08_24

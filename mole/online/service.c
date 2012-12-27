#include <getopt.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <signal.h>
#include <libtaomee/timer.h>
#include <libtaomee/conf_parser/config.h>
#include <libtaomee/tm_dirty/tm_dirty.h>
#include "shmq.h"
#include "tcp.h"
#include "central_online.h"
#include "mole_card.h"
#include "communicator.h"
#include <libtaomee/log.h>
#include "event.h"
#include "exclu_things.h"
#include "item.h"
#include "sprite.h"
#include "npc.h"
#include "dll.h"
#include "map.h"
#include "proto.h"
#include "protoheaders.h"
#include "tasks.h"
#include "dbproxy.h"
#include "mibiserver.h"
#include "gold_bean_serv.h"
#include "service.h"
#include "picsrv.h"
#include "login.h"
#include "magic_code.h"
#include "mole_question.h"
#include "mole_class.h"
#include "cache_svr.h"
#include "gift.h"
#include "mole_candy.h"
#include "npc_event.h"
#include "rand_itm.h"
#include "mcast.h"
#include "lamu_skill.h"
#include "profession_work.h"
#include "add_sth_rand.h"
#include "swap.h"
#include "game_bonus.h"
#include "race_bonus.h"
#include "im_message.h"
#include "friendship_box.h"
#include "lahm_classroom.h"
#include "charity_drive.h"
#include "mail.h"
#include "mole_pasture.h"
#include "chris_cake.h"
#include "mole_angel.h"
#include "mole_dungeon.h"
#include "angry_lahm.h"
#include "hide_and_seek.h"
#include "tv_question.h"
#include "angel_fight.h"
#include "fight_prize.h"
#include "play_drum.h"
#include "final_boss_2011.h"
#include "mole_cutepig.h"
#include "hallowmas.h"
#include "xhx_server.h"
#include "charitable_party.h"
#include "year_feast.h"
#include "lanterns_day.h"
#include "flower_party.h"

static int stampsvr_fd;
static struct sockaddr_in stampsvr_addr;
//
char* security_key;
char* signature_key;
fd_array_session_t fds;
config_cache_t config_cache;

DirtyWordHandle* dirtyword_hdl;

char* statistic_logfile;

void notify_stamp_svr(uint32_t id)
{
	protocol_t proto;
	init_proto_head(&proto, PROTO_UPDATE_STAMP, sizeof proto);
	proto.id = htonl(id);
	sendto(stampsvr_fd, &proto, sizeof proto, 0, &stampsvr_addr, sizeof stampsvr_addr);
}

void send_chatlog(int cmd, const sprite_t* sender, userid_t rcver_id, const void* content, int len)
{
	static uint8_t buf[4096];

	int i = 0;
	PKG_H_UINT32(buf, 1, i);
	PKG_H_UINT32(buf, sender->id, i);
	PKG_H_UINT32(buf, rcver_id, i);
	PKG_H_UINT32(buf, config_cache.bc_elem->online_id, i);
	PKG_H_UINT32(buf, GET_MAP_TYPE(sender->tiles->id), i);
	PKG_H_UINT32(buf, (uint32_t)(sender->tiles->id), i);
	PKG_H_UINT32(buf, get_now_tv()->tv_sec, i);
	if (len > 1024) {
		len = 1024;
	}
	PKG_H_UINT32(buf, len, i);
	PKG_STR(buf, content, i, len);

	send_request_to_chatlog_server(cmd, 0, i, buf, rcver_id);
	
}

void send_chat_monitor(int cmd, const sprite_t* sender, userid_t rcver_id, const void* content, int len)
{
	static uint8_t buf[4096];

	int i = 0;
	PKG_H_UINT32(buf, 1, i);
	PKG_H_UINT32(buf, sender->id, i);
	PKG_H_UINT32(buf, rcver_id, i);
	PKG_H_UINT32(buf, config_cache.bc_elem->online_id, i);
	PKG_H_UINT32(buf, GET_MAP_TYPE(sender->tiles->id), i);
	PKG_H_UINT32(buf, (uint32_t)(sender->tiles->id), i);
	PKG_H_UINT32(buf, get_now_tv()->tv_sec, i);
	if (len > 1024) {
		len = 1024;
	}
	PKG_H_UINT32(buf, len, i);
	PKG_STR(buf, content, i, len);

	send_request_to_chat_monitor_server(cmd, 0, i, buf, rcver_id);
}


static inline int
create_udp_socket (struct sockaddr_in *p, const char* ip, const char *port)
{
	bzero (p, sizeof (*p));
	p->sin_family = AF_INET;
	p->sin_port = htons (config_get_intval (port, 0));
	if (inet_pton (AF_INET, config_get_strval(ip), &p->sin_addr) <= 0)
		return -1;
	return socket (PF_INET, SOCK_DGRAM, 0);
}

static inline void
free_fdsess(void* fdsess)
{
	g_slice_free1(sizeof(fdsession_t), fdsess);
}

int handle_init(bind_config_elem_t* bc_elem)
{
	setup_timer();
	localtime_r(&now.tv_sec, &tm_cur);
	security_key = config_get_strval("security_key");
	signature_key = config_get_strval("signature_key");
	config_cache.idle_timeout = config_get_intval("idle_timeout", 10);
	config_cache.bc_elem      = bc_elem;

	fds.cn = g_hash_table_new_full(g_int_hash, g_int_equal, 0, free_fdsess);

	stampsvr_fd = create_udp_socket(&stampsvr_addr, "stamp_server_ip", "stamp_server_port");
	stampsvr_tcp_fd = connect_to_svr(config_get_strval("stamp_server_ip"), config_get_intval("stamp_server_port", 0), 65535, 1);
	chatlog_fd  = create_udp_socket(&chatlog_addr, "chatlog_svr_ip", "chatlog_svr_port");
	udp_dbserver_fd  = create_udp_socket(&udp_dbserver_addr, "udp_dbserver_ip", "udp_dbserver_port");
	monitor_server_fd  = create_udp_socket(&monitor_server_addr, "monitor_server_ip", "monitor_server_port");
	udp_submit_fd  = create_udp_socket(&udp_submit_addr, "udp_submit_ip", "udp_submit_port");
	if ((stampsvr_fd == -1) || (chatlog_fd == -1) || (udp_dbserver_fd == -1) || (monitor_server_fd == -1)
		|| (udp_submit_fd == -1)) {
		exit(0);
	}


    chat_monitor_fd  = create_udp_socket(&chat_monitor_addr, "chat_monitor_ip", "chat_monitor_port");

	couponserver_fd = connect_to_svr(config_get_strval("coupon_server_ip"), config_get_intval("coupon_server_port", 0), 65535, 1);
    DEBUG_LOG("HANDLE INIT CONNECT TO COUPON %d", couponserver_fd);
	mibi_buy_bean_fd = connect_to_svr(config_get_strval("mibi_buy_bean_ip"), config_get_intval("mibi_buy_bean_port", 0), 65535, 1);
	goldbean_serv_fd = connect_to_svr(config_get_strval("goldbean_serv_ip"), config_get_intval("goldbean_serv_port", 0), 65535, 1);
	getbean_serv_fd = connect_to_svr(config_get_strval("getbean_serv_ip"), config_get_intval("getbean_serv_port", 0), 65535, 1);
	mc_server_fd = connect_to_svr(config_get_strval("mc_server_ip"), config_get_intval("mc_server_port", 0), 65535, 1);
	magicserver_fd = connect_to_svr(config_get_strval("magicserver_ip"), config_get_intval("magicserver_port", 0), 65535, 1);
	xhx_server_fd = connect_to_svr(config_get_strval("xhx_switch_ip"), config_get_intval("xhx_switch_port", 0), 65535, 1);
	ticket_serv_fd = connect_to_svr(config_get_strval("ticket_serv_ip"), config_get_intval("ticket_serv_port", 0), 65535, 1);

	connect_to_central_online_timely(0, 0);

	if (init_communicator() == -1) {
		return -1;
	}
	if (config_get_strval("addr_mcast_ip")) {
		if (create_addr_mcast_socket() == 0) {
			send_addr_mcast_pkg(addr_mcast_1st_pkg);
		} else {
		return -1;
		}
	}

	init_sprites();
	init_outside_petmap();

	if ( (load_items(config_get_strval("items_conf")) == -1)
		|| (load_exchange_items(config_get_strval("exchange_conf")) == -1)
		|| (load_games(config_get_strval("games_conf")) == -1)
		|| (load_maps(config_get_strval("maps_conf")) == -1)
		|| (load_xml_holidays("./conf/holidays.xml") == -1)
		|| (load_events(config_get_strval("events_conf")) == -1)
		|| load_exclusive_things(config_get_strval("exclusive_things_conf"))
		|| load_xmlconf("./conf/tasks.xml", load_task_config)
		|| load_xmlconf("./conf/skill.xml", load_skill_config)
		|| load_xmlconf("./conf/lahm_task.xml", load_lahm_task)
		|| load_cards("./conf/cards.xml")
		|| load_new_cards("./conf/new_cards.xml")
		|| load_questions("./conf/questions.xml")
		|| (load_gifts("./conf/gifts.xml") == -1)
		|| (load_candy("./conf/candy.xml") == -1)
		|| load_class_questions("./conf/class_questions.xml")
		|| load_xmlconf(PIC_SRV_CONF, load_picsrv_config)
		|| load_xmlconf(TUYA_SRV_CONF, load_tuyasrv_config)
		|| (load_npc_tasks("./conf/npc_tasks.xml") == -1)
		|| (load_rand_items("./conf/rand_type.xml") == -1)
		|| (load_profession_work("./conf/profession_work.xml") == -1)
		|| (load_rand_conf("./conf/rand.xml") == -1)
		|| (load_exchange_conf("./conf/swap.xml") == -1)
		|| (load_game_bonus("./conf/games_bonus.xml") == -1)
		|| (load_race_bonus("./conf/race_bonus.xml") == -1)
		|| (load_friendbox_config("./conf/friendbox.xml") == -1)
		|| (load_lahm_students_conf("./conf/lahm_students.xml") == -1)
		|| (load_lahm_lessons_conf("./conf/lahm_lessons.xml") == -1)
		|| (load_lahm_classroom_bonus("./conf/lahm_classroom_bonus.xml") == -1)
		|| (load_lahm_lesson_questions_conf("./conf/lahm_lesson_questions.xml") == -1)
		|| (load_donate_item_config("./conf/donateitem.xml") == -1)
		|| (load_animal_skills_conf("./conf/animal_skill.xml") == -1)
		|| (load_rain_egg_map_pos_conf("./conf/animal_rain_egg.xml") == -1)
#ifndef TW_VER
		|| (load_mail_conf("./conf/mails.xml") == -1)
		|| (load_tv_questions("./conf/tv_question.xml", 1) == -1)
		|| (load_tv_questions("./conf/tv_question1.xml", 2) == -1)
		|| (load_tv_questions("./conf/tv_question2.xml", 3) == -1)
#else
		|| (load_mail_conf("./conf/mails_tw.xml") == -1)
#endif
        || (load_classroom_fellowship_conf("./conf/lahm_classroom_fellowship.xml") == -1)
        || (load_weekend_bonus("./conf/weekend_bonus.xml") == -1)
        || (load_td_role_info("./conf/TDInfo.xml") == -1)
        || (load_tdtask_all() == -1)
        || (load_angel_honor_bonus("./conf/angle_honor_bonus.xml") == -1)
		|| (load_angel_compose_material_conf("./conf/angel_compose.xml") == -1)
		|| (load_af_skill("./conf/af_skills.xml") == -1)
        || (load_prize_conf("./conf/af_prize.xml") == -1)
        || (load_prize_type_conf("./conf/af_prize_type.xml") == -1)
        || (load_barrier_conf("./conf/levelinfo.xml") == -1)
        || (load_af_task("./conf/af_task.xml") == -1)
		|| (dungeon_load_all_map_info() == -1)
        || (al_load_conf_file() == -1)
		|| (cutepig_load_config_file() == -1)
		|| (load_cutepig_explor_mine_conf("./conf/cutepig_explor_mine.xml") == -1)
		|| (load_map_mine_rand_conf("./conf/cutepig_mine_rand.xml") == -1)
		|| (mw_load_friendbox_config("./conf/mw_friendbox.xml") == -1)
		|| load_dlls() ) {
		return -1;
	}
	if(init_default_login_maps() == -1){
		ERROR_RETURN(("Failed to load default login maps!"), -1);
	}

	init_npc_system(config_get_strval("npc_conf"));
	if (init_proc_msg() == -1) {
		return -1;
	}

	//map_t* mp = get_map(1);
	//ADD_TIMER_EVENT(mp, blackcat_car, 0, now.tv_sec + 30);
	
	init_year_feast_timer();
	charparty_timer_initialize();
	ld_timer_initialize();
	fp_game_initialize();
	cheer_team_init();
	return 0;
}

int handle_fini()
{
	int i;

	for (i = 0; i < epi.maxfd; i++) {
		if (epi.fds[i].type == REMOTE_TYPE_FD && epi.fds[i].cb.sendlen > 0)
			return 0;
	}

	destroy_timer();
	destroy_events();
	destroy_exclusive_things();
	unload_maps();
	unload_items();
	fini_sprites();
	fini_communicator();
	unload_dlls();
//	releaseDirtyWordHandle(dirtyword_hdl);

	g_hash_table_destroy(fds.cn);

	return 1;
}

int shm_ctl_block_push(struct shm_queue *q, int fd, int type, int del_node)
{
	shm_block_t mb;
	int err = 0;

	fdsession_t* fdsess = get_fdsess(fd);
	if (!fdsess) {
		return -1;
	}

	mb.id     = fdsess->id;
	mb.length = sizeof(shm_block_t);
	mb.type   = type;
	mb.fd     = fd;

	if (type == FIN_BLOCK)
		err = handle_close(fd, 1, del_node);

	if (!err)
		return shmq_push(q, &mb, NULL);
	else
		return err;
}

int handle_input(const char *buffer, int rcvlen, int fd, int is_conn)
{
	int len, v;
	const char* policy   = "<policy-file-request/>";
	const char* response = "<?xml version=\"1.0\"?>"
				"<!DOCTYPE cross-domain-policy SYSTEM \"/xml/dtds/cross-domain-policy.dtd\">"
				"<cross-domain-policy>"
				"<site-control permitted-cross-domain-policies=\"all\"/>"
				"<allow-access-from domain=\"*\" to-ports=\"*\" />"
				"</cross-domain-policy>";

	if (rcvlen < 4) return 0;
	v = *(uint32_t *)buffer;
	if (is_conn) {
		if (rcvlen == 23 && !memcmp(buffer, policy, 23)) {
			net_send(fd, (const uint8_t*)response, strlen(response) + 1, 1);
			epi.fds[fd].cb.recvlen = 0;

			TRACE_LOG("Policy Req [%s] Received, Rsp [%s] Sent", policy, response);
			return 0;
		}
		len = ntohl(v);
		v = sizeof(protocol_t);
	} else {
		len = v;
		v = sizeof(server_proto_t);
	}

	if ((len > PAGESIZE) || (len < v)) {
		return -1;
	}

	return len;
}

int handle_process(uint8_t* recvbuf, int rcvlen, int fd, int is_conn)
{
	int err = 0;
	sprite_t* p;

	if (is_conn) {
		fdsession_t* fdsess = get_fdsess(fd);
		if (fdsess) {

//		    DEBUG_LOG("handle_process parse_protocol %d pid %u", is_conn, getpid());
		    
			fdsess->last_tm = now.tv_sec;
			if ( (err = parse_protocol(recvbuf, rcvlen, fdsess)) )
				shm_ctl_block_push(&(config_cache.bc_elem->sendq), fd, FIN_BLOCK, 1);
		}
	} else {
		if ( (err = worker_handle_net(fd, recvbuf, rcvlen, &p)) && p )
			del_sprite_conn(p, 1);
	}

	return 0;
}

int handle_close(int fd, int is_conn, int del_node)
{
	if (!is_conn) {
		int home_fd_idx = -1;
		if (fd == proxysvr_fd) {
			send_err_to_monitor(0, 0, 5);
			ERROR_LOG ("dbproxy connection closed, fd=%d", fd);
			proxysvr_fd = -1;
		} else if (fd == central_online_fd) {
			ERROR_LOG("central online connection closed, fd=%d", fd);
			central_online_fd = -1;
			connect_to_central_online_timely(0, 0);
		} else if (fd == magicserver_fd) {
			ERROR_LOG("magicserver connection closed, fd=%d", fd);
			magicserver_fd = -1;
		} /*else if (fd == mibiserver_fd) {
			ERROR_LOG("mibiserver connection closed, fd=%d", fd);
			mibiserver_fd = -1;
		}*/ else if (fd == couponserver_fd) {
			ERROR_LOG("couponserver connection closed, fd=%d", fd);
			couponserver_fd = -1;
		} else if (fd == mibi_buy_bean_fd) {
			ERROR_LOG("mibi buy gold bean server connection closed, fd=%d", fd);
			mibi_buy_bean_fd = -1;
		} else if (fd == goldbean_serv_fd) {
			ERROR_LOG("goldbean server connection closed, fd=%d", fd);
			goldbean_serv_fd = -1;
		} else if (fd == getbean_serv_fd) {
			ERROR_LOG("getbean server connection closed, fd=%d", fd);
			getbean_serv_fd = -1;
		} else if (fd == mc_server_fd) {
			ERROR_LOG("mc server connection closed, fd=%d", fd);
			mc_server_fd = -1;
		} else if (fd == stampsvr_tcp_fd) {
			ERROR_LOG("cache server connection closed, fd=%d", fd);
			stampsvr_tcp_fd = -1;
		} else if ((home_fd_idx = home_server_fd_idx(fd)) != -1) {
			ERROR_LOG("home server connection closed, fd=%d", fd);
			home_serv_fd[home_fd_idx] = -1;
		} else if (fd == school_bar_fd) {
			ERROR_LOG("school bar connection closed, fd=%d", fd);
			school_bar_fd = -1;
		} else if (fd == im_fd) {
			ERROR_LOG("im connection closed, fd=%d", fd);
			im_fd = -1;
		} else if (fd == xhx_server_fd) {
			ERROR_LOG("xiao hua xian connection closed, fd=%d", fd);
			xhx_server_fd = -1;
		} else if (fd == ticket_serv_fd) {
			ERROR_LOG("VIP month ticket connection closed, fd=%d", fd);
			ticket_serv_fd = -1;
		}
	} else {
		fdsession_t* fdsess = get_fdsess(fd);
		if (!fdsess) {
			ERROR_RETURN( ("connection %d had already been closed", fd), -1 );
		}

		assert(fds.count > 0);
		del_sprite_by_fd(fd, 1, del_node);
		remove_fdsess(fd);
	}

	return 0;
}

int handle_open(const shm_block_t* mb)
{
	fdsession_t* fdsess = get_fdsess(mb->fd);
	if (fdsess || (mb->length != (sizeof(shm_block_t) + sizeof(struct skinfo))) ) {
		ERROR_RETURN(("handle_open OPEN_BLOCK, fd=%d length=%d last=%u",
				mb->fd, mb->length, (uint32_t)fdsess->last_tm), -1);
	} else {
		fdsess = g_slice_alloc(sizeof *fdsess);
		fdsess->fd = mb->fd;
		fdsess->id = mb->id;
		fdsess->remote_port = *(uint16_t *)mb->data;
		fdsess->remote_ip = *(uint32_t *)&mb->data[2];
		fdsess->last_tm = *(uint16_t *)&mb->data[6];
		add_fdsess(fdsess);
		//TRACE_LOG("open connection, fd=%d, id=%u", mb->fd, mb->id);
	}

	return 0;
}

/**
 * run_worker_process
 *   @bc - bind config
 *   @bc_elem_idx - index bc->configs
 *   @n_inited_bc - number of inited bc->configs
 *
 */
void run_worker_process(bind_config_t* bc, int bc_elem_idx, int n_inited_bc)
{
	bind_config_elem_t* bc_elem = &(bc->configs[bc_elem_idx]);

	char prefix[8] = { 0 };
	int len = snprintf(prefix, 5, "%d", bc_elem->online_id);
	prefix[len] = '_';


    log_init_ex( config_get_strval("log_dir"),
                config_get_intval("log_level", log_lvl_trace),
                config_get_intval("log_size", 1<<30),
                config_get_intval("max_log_files", 100),
                prefix ,
                config_get_intval("log_save_next_file_interval_min", 0) );



	statistic_logfile = config_get_strval("statistic_logfile");
	// release resources inherited from parent process
	close_shmq_pipe(bc, n_inited_bc, 1);
	shmq_destroy(bc_elem, n_inited_bc);
	net_exit(); // TODO - might be problem here

	daemon_set_title("%s-%d", prog_name, bc_elem->online_id);

	net_init(MAXFDS, 100);
	do_add_conn(bc_elem->recvq.pipe_handles[0], PIPE_TYPE_FD, 0, 0);

	if (handle_init(bc_elem) != 0 ) {
		kill(getppid(),SIGKILL);
		boot_log(-1,0,"handle_init() ==> load config files");
		goto fail;
	}

#ifdef GPROF_DBG
				char child_dir[128] = {0};
				snprintf(child_dir, 64, "%s%d", "./child_dir/", bc_elem->online_id);
				DEBUG_LOG("cur:%s", get_current_dir_name());
				DEBUG_LOG("child_dir:%s", child_dir);
				if(-1 == chdir(child_dir))
					DEBUG_LOG("chdir error");
#endif

	while ( !stop || !handle_fini() ) {
		net_loop(100, PAGESIZE, 0);
	}

fail:
	do_destroy_shmq(bc_elem);
	net_exit();
	free_argv();
	free(prog_name);
	free(current_dir);
	exit(0);
}

void restart_child_process(bind_config_elem_t* bc_elem)
{
	close(bc_elem->recvq.pipe_handles[1]);
	do_del_conn(bc_elem->sendq.pipe_handles[0], 2);
	do_destroy_shmq(bc_elem);

	shmq_create(bc_elem);

	bind_config_t* bc = get_bind_conf();
	int i = get_bind_conf_idx(bc_elem);
	pid_t pid;

	if ( (pid = fork ()) < 0 ) {
		CRIT_LOG("fork failed: %s", strerror(errno));
	} else if (pid > 0) { //parent process
		close_shmq_pipe(bc, i, 0);
		do_add_conn(bc_elem->sendq.pipe_handles[0], PIPE_TYPE_FD, 0, bc_elem);
	} else { //child process
		run_worker_process(bc, i, bc->bind_num);
	}
}

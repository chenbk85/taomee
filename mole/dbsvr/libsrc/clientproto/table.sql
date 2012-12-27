	select userid,hex(vip) vip , hex(flag) flag ,birthday birth,hex(passwd) passwd,petcolor pcolor,
		 petbirthday pbirth, nick,xiaomee,exp,
		strong,iq,charm,Ol_count Olcount,Ol_today Oltoday,Ol_last,Ol_time, 
		mobile,email, hex(friendlist) flist,  hex(blacklist) blist, hex(homeattirelist) halist ,
		hex(noused_homeattirelist) nhalist,hex(homemap) homemap,hex(msglist) msglit
		from   USER_00.t_user_00
		where userid=420000;

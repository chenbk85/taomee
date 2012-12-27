#!/usr/bin/env python
# -*- coding: utf-8 -*-
import mypub 
import sys
import os 
import socket 
import time 
import struct 
from mypub import *

g_seq=0

CMD_SYSMSG=10003
CMD_LOGIN=1001
CMD_TALK=302
CMD_MOVE=303
CMD_ACTION=305
CMD_THROW_ITEM=306
CMD_USER_INFO=401
CMD_JOIN_LEAVE=402
CMD_CHANGE_ATTIRE=503
CMD_MOLE_BLOCK=5503
CMD_MOLE_BLOCK_DUDE=5504
CMD_MOLE_SL_CLOTH=1125
CMD_SALARY=1330
CMD_GET_LARM_COUNT=212
CMD_SET_LARM_TAST=1205
CMD_GET_LARM_TAST=1204
CMD_GET_TAST_LIST = 216
CMD_EXCHANGE_PASSWORD = 1497
CMD_SET_LARM_SKILL = 1127
CMD_CALL_SL = 251
CMD_DUIHUAN = 1522
CMD_SET_WISH = 967
CMD_GET_WISH = 968
CMD_SET_LARM_SKILLS = 1217
CMD_FOLLOW_PET = 215
CMD_GET_SKILL_GOODS = 1988
CMD_FEED_DOG = 1990
CMD_RETURN_DOG = 1989
CMD_GET_DOG_MESSAGE = 1991
CMD_SET_LARM_HOT_SKILL = 1218
CMD_SET_HELP_HEDGEPIG = 1219
CMD_GET_ARCHITECT_EXP = 919
CMD_COMPLETE_NPC_TASK = 2202
CMD_ACCEPT_NPC_TASK = 2201
CMD_BUY_ITEM = 2601
CMD_LARM_USE_SKILL = 1212
CMD_CAPTURE_ANIMALS = 1992
#CMD_RECEIVE_GIFT = 1994
CMD_CHOOSE_CHAIRMAN = 1994
CMD_SELECT_CHAIRMAN = 2008
CMD_INCREASE_NPC_SCORE = 1996
CMD_GET_CERTIFICATE = 997
CMD_CREAT_BUILDING = 996
CMD_SELECT_GRID_NUM = 1000
CMD_GET_GRID_MESSAGE = 995
CMD_AMUSE_CAT = 2011
CMD_GET_FOOD = 2012
CMD_SHARE_FOOD = 2013
CMD_EXCHANGE_GOODS = 2014
CMD_PLAYER_ENTER_MARKET = 4001
CMD_PLAYER_TRADE_BUY = 4007
class Dealmsg: 
    def __init__(self):
        #comid =>调用的函数
        self.cmdmap={
            CMD_SYSMSG:self.sysmsg,
            CMD_LOGIN:self.login,
            CMD_TALK:self.talk,
            CMD_MOVE:self.move,
            CMD_ACTION:self.action,
            CMD_THROW_ITEM:self.throw_item,
            CMD_USER_INFO:self.user_info,
            CMD_JOIN_LEAVE:self.join_leave,
            CMD_CHANGE_ATTIRE:self.change_attire,
            CMD_MOLE_BLOCK:self.mole_selectBlockCount,
            CMD_MOLE_BLOCK_DUDE:self.mole_blockDude,
            CMD_MOLE_SL_CLOTH:self.mole_SLcloth,
            CMD_SALARY:self.get_smc_salary,
            CMD_GET_LARM_COUNT:self.get_larmCount,
            CMD_SET_LARM_TAST:self.set_larm_tast,
            CMD_GET_LARM_TAST:self.get_larm_tast,
            CMD_GET_TAST_LIST:self.get_tastlist,
            CMD_EXCHANGE_PASSWORD:self.exchange_password,
            CMD_SET_LARM_SKILL:self.set_larm_skill,
            CMD_CALL_SL:self.call_SL,
            CMD_DUIHUAN:self.duihuan,
            CMD_SET_WISH:self.set_wish,
            CMD_GET_WISH:self.get_wish,
            CMD_SET_LARM_SKILLS:self.set_larm_skills,
            CMD_FOLLOW_PET:self.follow_pet,
            CMD_GET_SKILL_GOODS:self.get_skill_goods,
            CMD_FEED_DOG:self.feed_dog,
            CMD_RETURN_DOG:self.return_dog,
            CMD_GET_DOG_MESSAGE:self.get_dog_message,
            CMD_SET_LARM_HOT_SKILL:self.set_larm_hot_skill,
            CMD_SET_HELP_HEDGEPIG:self.set_help_hedgepig,
            CMD_GET_ARCHITECT_EXP:self.get_architect_exp,
            CMD_COMPLETE_NPC_TASK:self.complete_NPC_task,
            CMD_ACCEPT_NPC_TASK:self.accept_NPC_task,
            CMD_BUY_ITEM:self.buy_item,
            CMD_LARM_USE_SKILL:self.larm_use_skill,
            CMD_CAPTURE_ANIMALS:self.capture_animals,
            #CMD_RECEIVE_GIFT:self.receive_gift,
            CMD_CHOOSE_CHAIRMAN:self.choose_chairman,
            CMD_SELECT_CHAIRMAN:self.select_chairman,
            CMD_INCREASE_NPC_SCORE:self.increase_npc_score,
            CMD_GET_CERTIFICATE:self.get_certificate,
            CMD_CREAT_BUILDING:self.creat_building,
            CMD_SELECT_GRID_NUM:self.select_grid_num,
            CMD_GET_GRID_MESSAGE:self.get_grid_message,
            CMD_AMUSE_CAT:self.amuse_cat,
            CMD_GET_FOOD:self.get_food,
            CMD_SHARE_FOOD:self.share_food,
            CMD_EXCHANGE_GOODS:self.exchange_goods,
            CMD_PLAYER_ENTER_MARKET:self.player_enter_market,
            CMD_PLAYER_TRADE_BUY:self.player_trade_buy,
        };

    def change_attire(self,result,primsg):
        #换上或脱下
        userid,attireidcount=struct.unpack(">LL",primsg[0:8]);
        dmsg={ }
        dmsg["userid"]=userid ;
        dmsg["attireidcount"]=attireidcount;
        #print "换上或脱下:用户",auserid ,"物品个数:",attireidcount;
        for i in range(attireidcount):
            attireid,flag=struct.unpack(">LB",primsg[8+i*5:8+5+i*5]) ;
            dmsg["attireid"+str(i)]=attireid;
            dmsg["flag"+str(i)]=flag;

        self.ex_change_attire(dmsg);    

    def action(self,result,primsg):
        userid, actionid,flag =struct.unpack(">LLB",pri_msg);
        dmsg={}
        dmsg.pop("userid",userid );
        dmsg.pop("actionid",actionid);
        dmsg.pop("flag",flag );
        #print "动作:用户",auserid ,"动作:",action_list[actionid], "flag:", flag   ;
        self.ex_action(dmsg);    
        pass

    def ex_change_attire(self, dmsg ):
        print dmsg 
        pass

    def ex_login(self, dmsg ):
        print dmsg 
    def ex_talk(self, dmsg ):
        print dmsg 
        pass
    def ex_join_leave(self, dmsg ):
        print dmsg 
        pass
    def ex_action(self, dmsg ):
        print dmsg 
        pass
    def ex_move(self,dmsg ):
        print dmsg 
        pass


    def user_info(self,result,primsg):
        #用户信息
        auserid, anick =struct.unpack(">L16s",primsg[0:20] );
        attireidcount,=struct.unpack(">B",primsg[111:112]);
        print "用户信息?!:",auserid,"昵称:",anick ,"装扮个数:",attireidcount
        for i in range(attireidcount):
            attireid,=struct.unpack(">L",primsg[112+i*4:112+4+i*4]) ;
            print "\t 装扮ID:",attireid 
        print "未解包:"
        print print_hex_16(primsg[20:111] );



    def ex_sysmsg(self, dmsg ):
        print dmsg 
        pass
    def ex_action(self, dmsg ):
        print dmsg 
        pass


    def user_info(self,result,primsg):
        #用户信息
        auserid, anick =struct.unpack(">L16s",primsg[0:20] );
        attireidcount,=struct.unpack(">B",primsg[111:112]);
        print "用户信息?!:",auserid,"昵称:",anick ,"装扮个数:",attireidcount
        for i in range(attireidcount):
            attireid,=struct.unpack(">L",primsg[112+i*4:112+4+i*4]) ;
            print "\t 装扮ID:",attireid 
        print "未解包:"
        print print_hex_16(primsg[20:111] );



    def login(self,result,primsg):
        #用户登入
        userid, nick =struct.unpack(">L16s",primsg[0:20] );
        attireidcount,=struct.unpack(">B",primsg[122:123]);
        #print "登入 用户:",auserid,"昵称:",anick, "装扮个数", attireidcount;
        exp,power,iq,charm,xiaomee =struct.unpack(">LLLLL",primsg[44:64]);
        #print "经验:", exp, "武力:",power,"智力:",iq,"魅力:",charm,"摩尔豆:",xiaomee

        dmsg={ }
        dmsg["userid"]=userid ;
        dmsg["nick"]=nick;
        dmsg["exp"]=exp;
        dmsg["power"]=power;
        dmsg["iq"]=iq;
        dmsg["charm"]=charm;
        dmsg["xiaomee"]=xiaomee;

        for i in range(attireidcount):
            attireid,=struct.unpack(">L",primsg[123+i*4:123+4+i*4]) ;
            #print "\t 装扮ID:",attireid 
            dmsg["attireid_"+str(i) ]=attireid;
        
        #print "未解包:"
        #print print_hex_16(primsg[20:122],20 );
        self.ex_login(dmsg);
        pass

    def throw_item(self,result,primsg):
        auserid, attireid , x, y =struct.unpack(">LLLL",primsg );
        print "投弹:用户",auserid,"投掷物品",attireid, "x", x ,"y",y;
        pass
    def talk(self,result,primsg):
        userid,nick =struct.unpack(">L16s",primsg[0:20] );
        #print "聊天 ",auserid,anick, "内容:", primsg[28:];
        dmsg={ }
        dmsg["userid"]=userid ;
        dmsg["nick"]=nick;
        dmsg["msg"]=primsg[28:];
        self.ex_talk(dmsg);

        pass
    def join_leave(self,result,primsg):
            flag , userid =struct.unpack(">LL",primsg );
            dmsg={ }
            dmsg["flag"]=flag;
            dmsg["userid"]=userid;
            self.ex_join_leave (dmsg);
            pass
    def move(self,result,primsg):
            #走路 
            userid, x , y, z =struct.unpack(">LLLL",primsg );
            dmsg={ }
            dmsg["userid"]=userid ;
            dmsg["x"]=x;
            dmsg["y"]=y;
            self.ex_move(dmsg);
            pass

    def choose_chairman(self, result, primsg):
            """屋委会选择会长"""
            print "result:",result;
            pass

    def select_chairman(self, result, primsg):
            """获取用户支持哪个NPC做屋委会会长"""
            print "result:",result;
            if len(primsg)!=0:
                type = struct.unpack(">L",primsg);
                print type;
            pass
        
    def increase_npc_score(self, result, primsg):
            """玩游戏增加NPC人气"""
            print "result:",result;
            if len(primsg) != 0:
                score = struct.unpack(">L",primsg[0:4]);
                npc = struct.unpack(">L",primsg[4:8]);
                itemid = struct.unpack(">L",primsg[8:12]);
                print "score:",score;
                print "npc:",npc;
                print "itemid:",itemid;
            pass
        
    def get_certificate(self,result,primsg):
            """获得土地使用证书"""
            print "result:",result;
            if len(primsg) != 0:
                money = struct.unpack(">L",primsg);
                print "money:",money;
            pass
    def sysmsg(self,result,primsg):
            print  "deal:sysmsg" 
            print "<消息> 来自:", primsg[12:12+16] , "内容:", primsg[44:]  
            #print  print_hex_16(primsg)
            pass


    def dealmsg(self,comid,result,primsg):
            print  "1dealmsg:", result,":", len(primsg)
            func=self.cmdmap.get(comid)
            if(func):
                func(result,primsg)
            else:
                print "未处理:", comid, result
                print  print_hex_16(primsg)
                pass

    def mole_blockDude(self,result,primsg):
            """摩尔搬石头"""
            if len(primsg) != 0:
                itmId = struct.unpack(">L",primsg)
                print "itemId:",itmId
            else:
               print "error..."

    def mole_selectBlockCount(self,result,primsg):
            """查询摩尔每日搬石头的次数和总次数"""
            #print "shelly"
            #for i in range(2):
            #    a1 = struct.unpack(">L",primsg[i*4:i*4+4])
            #    print a1
            Day_num, Total_num =  struct.unpack(">LL",primsg)
            print Day_num, Total_num
        
    def mole_SLcloth(self,result,primsg):
            """获得超拉战袍"""
            if len(primsg) != 0:
                itmId = struct.unpack(">L",primsg)
                print itmId
            else:
                print "error..."
            pass
        
    def get_architect_exp(self, result, primsg):
            """获得建筑师的经验值"""
            print "result:",result;OB
            if len(primsg) != 0:
                architect_exp = struct.unpack(">L",primsg);
                #print "primsg:",primsg;
            pass

    def accept_NPC_task(self, result, primsg):
            """接NPC任务"""
            print "result:",result;
            if len(primsg) != 0:
                for i in range(8):
                    list = struct.unpack(">L",primsg[i*4:i*4+4]);
                    print list;
            pass
        
    def complete_NPC_task(self, result, primsg):
            """提交任务"""
            print "result:",result;
            if len(primsg) != 0:
                pri_len = len(primsg) - 4;
                out_info = struct.unpack(">L"+str(pri_len)+"s",primsg);
                taskid, submsg=out_info;
                print "taskid:",taskid;
                skillcnt = struct.unpack(">L",submsg[0:4]);
                print "skillcnt:",skillcnt;
                sc = int(skillcnt[0]);
                for i in range(sc):
                    list = struct.unpack(">L",submsg[i*4+4:i*4+8]);
                    print list;

                itemcnt = struct.unpack(">L",submsg[sc*4+4:sc*4+8]);
                print "itemcnt:",itemcnt;
                ic = int(itemcnt[0]);
                for i in range(ic):
                    list = struct.unpack(">LL",submsg[sc*4+4+i*8+4:sc*4+4+i*8+12]);
                    print list;

                clothescnt = struct.unpack(">L",submsg[sc*4+ic*8+8:sc*4+ic*8+12]);
                print "clothescnt:",clothescnt;
                cc = int(clothescnt[0]);
                for i in range(cc):
                    list = struct.unpack(">LL",submsg[sc*4+ic*8+8+i*8+4:sc*4+ic*8+8+i*8+12]);
                    print list;
            pass

    def buy_item(self, result, primsg):
            """买物品"""
            print "result:",result;
            if len(primsg) != 0:
                list = struct.unpack(">LLL",primsg);
                print list;

    def get_larmCount(self,result,primsg):
            """查询拉姆id"""
            count = struct.unpack(">L",primsg[4:8])
            print len(primsg)
            a = struct.unpack(">L",primsg[87:91])
            print a
            #for i in range(0,count[0]):
            pass

    def set_larm_tast(self,result,primsg):
            """设置拉姆任务状态"""
            print "result:",result,"primsg:",primsg
            pass
        
    def get_larm_tast(self,result,primsg):
            """查询拉姆任务状态"""
            state = struct.unpack(">L",primsg)
            print "larm tast state:",state
            pass
    def set_larm_skills(self, result, primsg):
            """设置拉姆技能系"""
            print "result:",result
            if len(primsg) != 0:
               skill_type = struct.unpack(">L",primsg)
               print "skill type:", skill_type
            pass
    def larm_use_skill(self, result, primsg):
            """拉姆变身，使用技能"""
            print "result:",result;
            if len(primsg) != 0:
                userid= struct.unpack(">L",primsg[0:4]);
                petid = struct.unpack(">L",primsg[4:8]);
                action = struct.unpack(">L",primsg[8:12]);
                print "userid:",userid;
                print "petid:",petid;
                print "action:",action;
            pass

    def capture_animals(self, result, primsg):
            """抓皮皮和小豆芽"""
            print "result:",result;
            if len(primsg) != 0:
                itemid = struct.unpack(">L",primsg);
                print "itemid:",itemid
            pass

    def receive_gift(self, result, primsg):
            """领取礼包"""
            print "result:",result;
            print "primsg:",primsg;
            pass

    def get_tastlist(self, result, primsg):
            """查询任务列表"""
            print "result: ", result
            for i in range (len(primsg)):
                a1 = struct.unpack(">B", primsg[i])
                print "tast %d:"%i,a1
            pass
        
    def get_smc_salary(self,result,primsg):
            """查询SMC工资"""
            for i in range(51):
                a1 = struct.unpack(">L",primsg[i*4:i*4+4])
                print a1
            pass
    def exchange_password(self, result, primsg):
            """兑换时空密码"""
            print "result:",result
            if len(primsg) != 0:
                records = struct.unpack(">L", primsg[0:4] )
                print "records:",records
                itmid =  struct.unpack(">L",primsg[4:8])
                print itmid
            pass
    def set_larm_skill(self, result, primsg):
            """设置拉姆技能"""
            print "result:",result
            if len(primsg) != 0:
                type = struct.unpack(">L",primsg[0:4])
                skill = struct.unpack(">L",primsg[4:8])
            print "type:",type
            print "skill:",skill
            pass
        
    def set_help_hedgepig(self, result, primsg):
            """设置拯救刺猬"""
            print "result:", result
            if len(primsg) != 0:
                itemid = struct.unpack(">L",primsg)
                print "itemid:", itemid
            pass

    def get_skill_goods(self, result, primsg):
            """领取技能物品的大礼包"""
            print "result:",result
            print "primsg:",primsg
            pass

    def creat_building(self, result, primsg):
            """创建建筑物"""
            print "result:",result;
            if len(primsg) != 0:
                roomid = struct.unpack(">L",primsg);
                print "roomid:",roomid;
            pass

    def select_grid_num(self,result,primsg):
            """查询格子数"""
            print "result:",result;
            print "primsg:",primsg;
            if len(primsg) != 0:
                start = struct.unpack(">L",primsg[0:4]);
                count = struct.unpack(">L",primsg[4:8]);
                print "start:",start;
                print "gridCount:",count;
            pass

    def get_grid_message(self, result, primsg):
        """获取格子信息"""
        print "result:",result;
        print len(primsg);
        k = 0
        e = 0
        if len(primsg) != 0:
            for j in range (0,4):
                for i in range (0,4):
                    e = k + 8;
                    k = k + 4; 
                    a1 = struct.unpack(">L",primsg[k:e]);
                    print a1
                name = struct.unpack(">16s",primsg[e:e+16]);
                k = e + 12;
                print "name:",name;
                print 
        pass

    def feed_dog(self, result, primsg):
            """喂机器狗"""
            print "result:",result
            if len(primsg) != 0:
                end_time = struct.unpack(">L",primsg)
                print "end_time:",end_time
            pass
        
    def return_dog(self, result, primsg):
            """退还机器狗"""
            print "result:",result
            print "primsg:",primsg
            pass

    def amuse_cat(self, result, primsg):
            """引逗猫"""
            print "result:",result;
            if len(primsg) != 0:
                itemid = struct.unpack(">L",primsg);
                print "itemid:",itemid;
            pass

    def get_food(self, result, primsg):
        """在场景中得到美食"""
        print "result:",result;
        if len(primsg) != 0:
            itemid = struct.unpack(">L",primsg);
            print "itemid:",itemid;
        pass

    def share_food(self,result,primsg):
        """分享美食"""
        print "result:",result;
        print len(primsg)
        pass

    def exchange_goods(self, result, primsg):
        """美誉值兑换东西"""
        print "result:",result;
        print len(primsg);
        pass

    def player_enter_market(self, result, primsg):
        """玩家进入交易市场 """
        print "result: ", result;
        marketid = struct.unpack(">L",primsg);
        print "marketid:", marketid
        pass

    def player_trade_buy(self, result, primsg):
        """玩家交易 买卖"""
        print "result: ", result;
        if len(primsg) != 0:
            out_info = struct.unpack(">LLLLLLL",primsg);
            type,itemid,uniqueid,cnt,duration,attirelv,leftcoins=out_info;
            print "type:",type,"itemid:",itemid,"uniqueid:",uniqueid,"cnt:",cnt
            print "duration:",duration,"attirelv:",attirelv
            print "leftcoins:",leftcoins
        pass

    def get_dog_message(self, result, primsg):
            """得到机器狗帮做做事的信息"""
            print "result:", result
            dogid = struct.unpack(">L",primsg[0:4])
            Remain_time = struct.unpack(">L",primsg[4:8])
            Plant_water_count = struct.unpack(">L",primsg[8:12])
            Insect_kill_count = struct.unpack(">L",primsg[12:16])
            Animal_water_count = struct.unpack(">L",primsg[16:20])
            Animal_catch = struct.unpack(">L",primsg[20:24])
            print "dogid:",dogid
            print "Remain_time:",Remain_time
            print "Plant_water_count:",Plant_water_count
            print "Insect_kill_count:",Insect_kill_count
            print "Animal_water_count:",Animal_water_count
            print "Animal_catch:",Animal_catch
            pass

    def set_larm_hot_skill(self, result, primsg):
            """设置拉姆热点技能"""
            print "result:",result
            hot1 = struct.unpack(">B",primsg[0:1])
            hot2 = struct.unpack(">B",primsg[1:2])
            hot3 = struct.unpack(">B",primsg[2:3])
            print "hot1:",hot1
            print "hot2:",hot2
            print "hot3:",hot3
            pass

    def call_SL(self, result, primsg):
            """召唤超拉"""
            print "result:",result
            pass
    def follow_pet(self, result, primsg):
            """带上拉姆"""
            print "result:", result
            pass
        
    def duihuan(self, result, primsg):
            """冲值券兑换物品"""
            print "result:",result
            if len(primsg) != 0:
                records = struct.unpack(">L",primsg[0:4])
                print "records:",records
                itmid = struct.unpack(">L".primsg[4:8])
                print "itemId:",itmid
            #pass

    def select_shenqi_password(self, result, primsg):
            """查询神奇密码信息"""
            print "result:",result
            if len(primsg) != 0:
                max_select = struct.unpack(">D", primsg[0:2] )
                used_num =  struct.unpack(">D", primsg[2:4] )
                itm_num =  struct.unpack(">D", primsg[4:6] )
                for i in itm_num:
                    itmid =  struct.unpack(">D", primsg[i*4+6:i*4+12] )
                    print itmid
            pass
        
    def set_wish(self, result, primsg):
            """设置元宵节愿望"""
            print "result:", result
            print "primsg:", primsg
            pass

    def get_wish(self, result, primsg):
            """得到元宵节心愿"""
            print "result:",result
            if len(primsg) != 0:
               print primsg
            pass

    def funny_pets(self, result, primsg):
            """逗宠物"""
            userid, petid, action = struct.unpack(">LLL",primsg)
            hungry = struct.unpack(">B",primsg[16])
            thirsty = struct.unpack(">B",primsg[17])
            dirty  = struct.unpack(">B",primsg[18])
            spirit  = struct.unpack(">B",primsg[19])
            print "userid:",userid,"petid:",petid
            print "hungry:",hungry,"thirsty:",thirsty,"dirty:",dirty,"spirit:",spirit
            pass

class proto: 
    def getsock(self):
            return self.sock

    def getheaderlen(self):
            return self.headerlen
    def __init__(self,ip,port,dm):
            self.headerlen=18
            self.ip=ip;
            self.port=port
            self.uid=0
            self.role_tm=0
            self.seq=0
            self.dm=dm
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)     
            self.getsock().connect((self.ip,self.port))     
            self.recvbuf=""
            #设置为非阻塞连接    
            self.sock.setblocking(0)
            #print "init succ"
    def setblocking(self, flag):
            self.sock.setblocking(flag)

    def pack(self,cmdid,userid,primsg ):
            #mk header
            global g_seq
            result=0
            msglen=self.getheaderlen()+len(primsg)
            g_seq=g_seq+self.seq
            headermsg=struct.pack(">LHLLL", msglen,cmdid,userid,g_seq,result);
            return  headermsg+primsg      
            pass

    def login(self,userid,passwd,regflag=0):
            #登入    
            channel=0
            self.uid=userid
            imgid="0000000000000000"
            verifycode="000000"
            primsg=struct.pack(">32sLLL16s6s",mypub.get_md5(mypub.get_md5(passwd)),channel,6,regflag,imgid,verifycode);
            sendbuf=self.pack(103,userid,primsg); 
            self.getsock().send(sendbuf) 
            pass

    def get_online_list(self,startid,endid):
            #得到 online 列表
            primsg=struct.pack(">LLL",startid,endid,0 );
            sendbuf=self.pack(106,self.uid,primsg); 
            self.getsock().send(sendbuf) 
            pass
    def online_login(self,userid,key,roletm,version):
            #online 登入    
            self.userid=userid;
            self.seq=1
            primsg=struct.pack(">16sLL",key,roletm,version);
            sendbuf=self.pack(1001,self.userid,primsg);
            self.getsock().send(sendbuf) 
            pass

    def buy_item(self,itemid,count ):
            """购买"""
            primsg=struct.pack(">LL",itemid,count );
            sendbuf=self.pack(2601,self.userid,primsg); 
            self.getsock().send(sendbuf) 
            pass

    def buy_costdou(self,count):
            """支付摩尔豆"""
            primsg = struct.pack(">L",count)
            sendbuf = self.pack(1386,self.userid,primsg)
            self.getsock().send(sendbuf)
            pass
        
    def set_wish(self, wish_content):
        """设置元宵节心愿"""
        primsg = struct.pack (">360s",wish_content)
        sendbuf = self.pack (967, self.userid, primsg)
        self.getsock().send(sendbuf)
        pass

    def get_wish(self):
        """得到元宵节心愿"""
        sendbuf = self.pack(968, self.userid, "")
        self.getsock().send(sendbuf)
        pass
    def get_certificate(self):
        """获得土地使用证书"""
        sendbuf = self.pack(997,self.userid,"");
        self.getsock().send(sendbuf);
        pass
    def capture_animals(self,itemid):
        """抓皮皮和小豆芽"""
        primsg = struct.pack(">L",itemid);
        sendbuf = self.pack(1992, self.userid, primsg);
        self.getsock().send(sendbuf);
        pass

    def receive_gift(self,index):
        """领取礼包"""
        primsg = struct.pack(">L", index);
        sendbuf = self.pack(1994, self.userid, primsg);
        self.getsock().send(sendbuf);
        pass

    def mole_blockDude(self):
        """摩尔搬石头"""
        sendbuf = self.pack(5504,self.userid,"") 
        self.getsock().send(sendbuf)
        pass

    def feed_dog(self, dogid, itemid, count):
        """喂机器狗"""
        primsg = struct.pack(">LLL", dogid, itemid, count)
        sendbuf = self.pack(1990, self.userid, primsg)
        self.getsock().send(sendbuf)
        pass

    def return_dog(self, dogid):
        """退还机器狗"""
        primsg = struct.pack(">L", dogid)
        sendbuf = self.pack(1989, self.userid, primsg)
        self.getsock().send(sendbuf)
        pass

    def amuse_cat(self):
        """引逗猫"""
        sendbuf = self.pack(2011, self.userid, "");
        self.getsock().send(sendbuf);
        pass

    def get_food(self,itemid):
        """在场景中得到美食"""
        primsg = struct.pack(">L", itemid);
        sendbuf = self.pack(2012, self.userid, primsg);
        self.getsock().send(sendbuf);
        pass

    def share_food(self, Count, itemid, count):
        """分享美食"""
        primsg = struct.pack(">LLL",Count,itemid,count);
        sendbuf = self.pack(2013, self.userid, primsg);
        self.getsock().send(sendbuf);
        pass
    
    def exchange_goods(self,itemid,count):
        """美誉值兑换东西"""
        primsg= struct.pack(">LL",itemid,count);
        sendbuf = self.pack(2014,self.userid,primsg);
        self.getsock().send(sendbuf);
        pass

    def player_enter_market(self,marketid):
        """玩家进入交易市场"""
        primsg = struct.pack(">L",marketid);
        sendbuf = self.pack(4001,self.userid,primsg);
        self.getsock().send(sendbuf);
        pass

    def player_trade_buy(self,shopid,grid,itemid,uniqueid,cnt,coins):
        """玩家交易 买卖"""
        primsg = struct.pack(">LLLLLL",shopid,grid,itemid,uniqueid,cnt,coins);
        sendbuf = self.pack(4007,self.userid,primsg);
        self.getsock().send(sendbuf);
        pass

    def get_dog_message(self, dogid):
        """得到机器狗帮做做事的信息"""
        primsg   = struct.pack(">L", dogid)
        sendbuf = self.pack(1991, self.userid, primsg)
        self.getsock().send(sendbuf)
        pass

    def set_larm_hot_skill(self, larmid, hot1, hot2, hot3):
        """设置拉姆热点技能"""
        primsg = struct.pack(">LLLL", larmid, hot1, hot2, hot3)
        sendbuf = self.pack(1218, self.userid, primsg)
        self.getsock().send(sendbuf)
        pass
    def get_skill_goods(self):
        """领取技能物品的大礼包"""
        sendbuf = self.pack(1988, self.userid, "")
        self.getsock().send(sendbuf)
        pass

    def mole_selectBlockCount(self):
        """摩尔搬石头的次数查询"""
        sendbuf = self.pack(5503,self.userid,"")
        self.getsock().send(sendbuf)
        pass
        
    def mole_SLcloth(self):
        """领取超拉战袍"""
        sendbuf = self.pack(1125,self.userid,"")
        self.getsock().send(sendbuf)
        pass
    
    def choose_chairman(self, type):
        """屋委会选择会长"""
        primsg = struct.pack(">L",type);
        sendbuf = self.pack(1994, self.userid, primsg);
        self.getsock().send(sendbuf);
        pass

    def select_chairman(self):
        """获取用户支持哪个NPC做屋委会会长"""
        sendbuf = self.pack(2008,self.userid,"");
        self.getsock().send(sendbuf);
        pass

    def increase_npc_score(self, score):
        """玩游戏增加NPC人气"""
        primsg = struct.pack(">L",score);
        sendbuf = self.pack(1996, self.userid, primsg);
        self.getsock().send(sendbuf);
        pass

    def get_smc_salary(self):
        """SMC工资查询"""
        sendbuf = self.pack(1330,self.userid,"")
        self.getsock().send(sendbuf)
        pass

    def creat_building(self, name, type):
        """创建建筑物"""
        primsg = struct.pack(">16sL",name,type);
        sendbuf = self.pack(996, self.userid, primsg);
        self.getsock().send(sendbuf);
        pass

    def select_grid_num(self):
        """查询格子数"""
        sendbuf = self.pack(1000, self.userid, "");
        self.getsock().send(sendbuf);
        pass

    def get_grid_message(self, mapid, maptype, grid):
        """拉取格子信息"""
        primsg = struct.pack(">LLL", mapid, maptype, grid);
        sendbuf = self.pack(995, self.userid, primsg);
        self.getsock().send(sendbuf);
        pass

    def set_larm_tast(self,larmId,tastId,tastState):
        """设置拉姆任务状态"""
        primsg = struct.pack(">LLL",larmId,tastId,tastState)
        sendbuf = self.pack(1205,self.userid,primsg)
        self.getsock().send(sendbuf)
        pass

    def get_larm_tast(self,larmId,tastId):
        """查询拉姆任务状态"""
        primsg = struct.pack(">LL",larmId,tastId)
        sendbuf = self.pack(1204,self.userid,primsg)
        self.getsock().send(sendbuf)
        pass
    def set_larm_skills(self,larmid,type):
        """设置拉姆技能系"""
        primsg = struct.pack(">LL", larmid, type)
        sendbuf = self.pack(1217, self.userid, primsg)
        self.getsock().send(sendbuf)
        pass
    def larm_use_skill(self,petid,type):
        """拉姆变身，使用技能"""
        primsg = struct.pack(">LL",petid,type);
        sendbuf = self.pack(1212, self.userid, primsg);
        self.getsock().send(sendbuf);
        pass

    def pet_sports(self,sportstype ,score):
        """拉姆运动会:设置分数"""
        primsg=struct.pack(">LL",sportstype ,score );
        sendbuf=self.pack(1135,self.userid,primsg); 
        self.getsock().send(sendbuf) 
        pass

    def follow_pet(self,petid,flag=1 ):
        """带上拉姆"""
        primsg=struct.pack(">LL",petid,flag);
        sendbuf=self.pack(215,self.userid,primsg); 
        self.getsock().send(sendbuf) 
        pass
    
    def duihuan(self, itemId):
        """冲值券兑换物品"""
        primsg = struct.pack(">L",itemId)
        sendbuf = self.pack(1522,self.userid,primsg)
        self.getsock().send(sendbuf)
        pass
    
    def get_jy_pet_status(self,count,uid1,uid2 ):
        """检查拉姆和小屋状态"""
        primsg=struct.pack(">LLL",count,uid1,uid2);
        sendbuf=self.pack(611,self.userid,primsg); 
        self.getsock().send(sendbuf) 
        pass

    def get_simply_userinfo(self,uid):
        """检查拉姆和小屋状态"""
        primsg=struct.pack(">L",uid);
        sendbuf=self.pack(203,self.userid,primsg); 
        self.getsock().send(sendbuf) 
        pass

    def accept_NPC_task(self,taskid):
        """接NPC任务"""
        primsg = struct.pack(">L",taskid);
        sendbuf = self.pack(2201, self.userid, primsg);
        self.getsock().send(sendbuf);
        pass
    
    def complete_NPC_task(self, taskid, select):
        """提交任务"""
        primsg = struct.pack(">LL",taskid, select);
        sendbuf = self.pack(2202, self.userid, primsg);
        self.getsock().send(sendbuf);
        pass

    def get_five_rock(self):
        """检查拉姆和小屋状态"""
        primsg="";
        sendbuf=self.pack(1383,self.userid,""); 
        self.getsock().send(sendbuf) 
        pass

    def set_five_rock(self, loc):
        """检查拉姆和小屋状态"""
        primsg=struct.pack(">L",loc);
        sendbuf=self.pack(1384,self.userid,primsg); 
        self.getsock().send(sendbuf) 
        pass

    def set_help_hedgepig(self):
        """设置拯救刺猬"""
        sendbuf = self.pack(1219, self.userid, "");
        self.getsock().send(sendbuf)
        pass
    
    def get_architect_exp(self):
        """获得建筑师的经验值"""
        sendbuf = self.pack(919, self.userid, "");
        self.getsock().send(sendbuf);
        pass

    def get_larmCount(self,userid,larmid,type):
        """查询拉姆"""
        primsg = struct.pack(">LLB",userid,larmid,type)
        sendbuf = self.pack(212,self.userid,primsg)
        self.getsock().send(sendbuf)
        pass
    def call_SL(self):
        """召唤超拉"""
        sendbuf = self.pack(251, self.userid, "")
        self.getsock().send(sendbuf)
        pass

    def exchange_password(self, password):
        """兑换时空密码"""
        primsg = struct.pack(">32s",password)
        sendbuf = self.pack(1497, self.userid, primsg)
        self.getsock().send(sendbuf)
        pass
    
    def set_larm_skill(self, larmid, type, level):
        """设置拉姆技能"""
        primsg = struct.pack(">LLL",larmid, type, level)
        sendbuf = self.pack(1127, self.userid, primsg)
        self.getsock().send(sendbuf)
        pass


    def check_bag(self,itemid):
        primsg=struct.pack(">L",itemid);
        sendbuf=self.pack(513,self.userid,primsg);
        self.getsock().send(sendbuf)
        print "check_bag"
        mypub.print_hex_16(sendbuf);
        pass

    def sell_goods(self,itemid,price, count, session):
        primsg=struct.pack(">LLL8s",itemid,price,count,session);
        sendbuf=self.pack(514,self.userid,primsg);
        self.getsock().send(sendbuf)
        print "selling goods........"
        mypub.print_hex_16(sendbuf );    
        pass



    def throw_item(self,itemid,x,y):
        """投掷东西"""
        primsg=struct.pack(">LLL",itemid,x,y );
        sendbuf=self.pack(306,self.userid,primsg); 
        self.getsock().send(sendbuf) 
        pass

    def gen_rand_item(self,itemid):
        primsg=struct.pack(">L",itemid );
        sendbuf=self.pack(320,self.userid,primsg); 
        #print mypub.print_hex_16(sendbuf);
        self.getsock().send(sendbuf) 
        pass

    def get_rand_item(self,posid):
        primsg=struct.pack(">LL",posid,0);
        sendbuf=self.pack(318,self.userid,primsg); 
        self.getsock().send(sendbuf) 
        pass

    def move(self,x,y ):
        """走动"""
        primsg=struct.pack(">LLL",x,y,0);
        sendbuf=self.pack(303,self.userid,primsg); 
        self.getsock().send(sendbuf) 
        pass
    def action(self, actionid, flag=0 ):
        """动作"""
        primsg=struct.pack(">LB",actionid,flag );
        sendbuf=self.pack(305,self.userid,primsg); 
        self.getsock().send(sendbuf) 
        pass

    def talk(self,msg ):
        """走动"""
        msglen=len(msg)
        primsg=struct.pack(">LL"+str(msglen)+"s",0,msglen,msg);
        sendbuf=self.pack(302,self.userid,primsg); 
        #print mypub.print_hex_16(sendbuf);
        self.getsock().send(sendbuf) 
        pass

    def cd_map(self,mapid):
        primsg="";
        sendbuf=self.pack(402,self.userid,primsg); 
        print mypub.print_hex_16(sendbuf);
        self.getsock().send(sendbuf) 

        primsg=struct.pack(">LL",mapid,0);
        sendbuf=self.pack(401,self.userid,primsg); 
        print mypub.print_hex_16(sendbuf);
        self.getsock().send(sendbuf) 
        pass

    def patient_sit(self,x ):
        """走动"""
        primsg=struct.pack(">L",x);
        sendbuf=self.pack(1502,self.userid,primsg); 
        self.getsock().send(sendbuf) 
        pass
    def patient_quit(self ):
        """走动"""
#        primsg=struct.pack(">L",x);
        sendbuf=self.pack(1504,self.userid,""); 
        self.getsock().send(sendbuf) 
        pass
    def get_pk_list(self ):
        """拉取pk记录"""
        #primsg=struct.pack(">L",x);
        sendbuf=self.pack(1464,self.userid,""); 
        self.getsock().send(sendbuf) 
        pass
    def get_cnt_list(self,start,end ):
        """拉取pk记录"""
        primsg=struct.pack(">LL",start, end);
        sendbuf=self.pack(10104,self.userid,""); 
        self.getsock().send(sendbuf) 
        pass
    def get_itms_cnt(self,count ):
        """拉取pk记录"""
        primsg=struct.pack(">L",count);
        sendbuf=self.pack(1915,self.userid,primsg); 
        self.getsock().send(sendbuf) 
        pass

    def get_tastlist(self,userid):
        """获取任务列表"""
        primsg = struct.pack( ">L",userid)
        sendbuf = self.pack( 216, self.userid, primsg)
        self.getsock().send(sendbuf)
        pass

    def getrecvmsg(self):
        """得到一个完整报文"""
        tmp_recvfbuf=""
        try :
            tmp_recvfbuf= self.getsock().recv(4096)  
            if ( tmp_recvfbuf  =="" ):
                print "socket close.."
                sys.exit(1)    
        except socket.error, msg:
            if(msg[0]<>11 ):
                sys.exit(1)     
            pass
        self.recvbuf=self.recvbuf+tmp_recvfbuf    
        #得到报文        
        if (len( self.recvbuf)<=4):
            return False 
        else:
            #得到长度
            recvmsg_arr=struct.unpack(">L",self.recvbuf[0:4]);
            recvmsg_len=recvmsg_arr[0];
            if (recvmsg_len>len(self.recvbuf)):
                return False 
                

            returnmsg=self.recvbuf[0:recvmsg_len];
            self.recvbuf=self.recvbuf[recvmsg_len:];

            pri_len=recvmsg_len- self.getheaderlen();
            return struct.unpack(">LHLLL"+str(pri_len)+"s", returnmsg);
    def dealmsg(self,need_cmd):
        while True:
            out_info=self.getrecvmsg();
            if (out_info):
                proto_len,cmdid,userid,seq,result, pri_msg=out_info;
                #print proto_len, cmdid, result, userid , mypub.print_hex_16(pri_msg,0 );
                if (cmdid==need_cmd):
                    self.dm.dealmsg(cmdid,result,pri_msg );
                    break
                else:
                    print "cmdid : ", cmdid, "need_cmd", need_cmd
        pass

def mole_login(loginser_ip,loginser_port,onlineid,userid, passwd, roletm, version,dm):
    p=proto(loginser_ip,loginser_port,dm)
    p.login(userid, passwd);
    p.setblocking(1)
    login_out=p.getrecvmsg();
    #print  print_hex_16( login_out[5])
    key=login_out[5][4:20]
    #得到online列表  
    p.get_online_list(onlineid,onlineid+10)
    out_msg=p.getrecvmsg();
    #print  print_hex_16( out_msg[5])
    online_id,user_count=struct.unpack(">LL",out_msg[5][4:12]);
    print "online_id", online_id, "user_count", user_count
    online_ip= out_msg[5][12:28].split('\0');
    online_port,=struct.unpack(">h",out_msg[5][28:30]);
    print "online_ip: %s" % online_ip
    print "online_port: ", online_port

    p=proto(online_ip[0] ,online_port, dm)
    p.online_login(userid, key, roletm, version);
    return p
    pass


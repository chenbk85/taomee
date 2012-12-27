#! /usr/bin/env python 

from xml.dom import minidom
import protocol
import string
import struct
import time
import thread
import mypub 

#global len_param #store 
global cmd_idss # store cmds' ids
global intervals # store intervals
global login_userid # user id for logining to server
global login_passwd # passwd


class XmlParser:
    
    def __init__(self, file):
        self.loadSource(file)
    
    def load(self, source):
        """load XML input source, return parsed XML document
        - a filename of a local XML file ("~/diveintopython/common/py/kant.xml")
        """
        try:
            sock = open(source)
        except IOError, (errno, strerror):

            print "I/O error(%s): %s" % (errno, strerror)

        xmldoc = minidom.parse(sock).documentElement
        sock.close()
        return xmldoc

    def loadSource(self, source):
        """load source"""
        self.source = self.load(source)

def parse_xml(source):
    p=XmlParser(source)
    xmldoc =p.source
    #print xmldoc.toxml()
    
    # get login user info 
    login_user_list = xmldoc.getElementsByTagName('login_user')
    global login_userid
    login_userid = login_user_list[0].attributes["userid"].value
    global login_passwd
    login_passwd = login_user_list[0].attributes["passwd"].value

    cmdlist = xmldoc.getElementsByTagName('cmd')
    num_cmd = cmdlist.length # the number of packages waiting for  sending

    global cmd_ids 
    cmd_ids=[] #initialization
    global intervals #initialization
    intervals=[]
 
    dict=[]

    if num_cmd>0:
        for m in range(0, num_cmd):
                cmd1 = cmdlist[m] #cmd node 
                #print cmd1.nodeName #1

                cmd_ids.append(cmd1.attributes["id"].value)
                intervals.append(cmd1.attributes["interval"].value)
                #print intervals
                nodeList = cmd1.childNodes
                paramsNode = nodeList[1]

                paramList = paramsNode.childNodes
                paramListLen = paramList.length

                # serialize the params 
                pair={}
                i=1 #jump over the first text node
                j=0
                format=""
                while i<paramListLen:
                    param = paramList[i]
                    name = param.attributes["Name"].value
                    type = param.attributes["Type"].value
                    len = param.attributes["Len"].value
                    value = param.attributes["Value"].value
#                    print name, type, len, value

                    # add more format here!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                    if (type=="string"):
                        arr1 = [len, 's']
                        format = string.join(arr1, '')
#                        format = format.encode("utf-8")
                    elif(type=="int"):
                        format = 'L'
                    elif (type=="uchar"):
                        format = 'B'
                    pair[j]=[format, value]
                    i = i+2 #jump over the text node between tags
                    j = j+1
                #end while
                
                dict.append(pair)
#                global len_param
#                len_param = j

        #end for

    else:
        print "No command found"
        exit(1)
    #end if
    #print cmd_ids, intervals, dict
    return dict
    

def capture_message(protocol_instance, *args):
    while 1:
        out_info=protocol_instance.getrecvmsg()	
        if(out_info):
            proto_len,version,cmdid,userid,result, pri_msg=out_info;
            print cmdid, result, userid  , mypub.print_hex_16(pri_msg,0 );



def do_send_package(source):
    dict=parse_xml(source)
    num_cmds = len(dict)
    

    return_msg = ""
#    return_msg = return_msg +"<DIV><img src=12.jpg></DIV>"
    return_msg = return_msg + "<p>begin logining...</p>"
    p=protocol.mole_online("10.1.1.5",7777 ,4, int(login_userid), str(login_passwd)) #login on login server
    time.sleep(1)
    print "exec"
    
    while 1:
        out_info=p.getrecvmsg()	
        if(out_info):
            proto_len,version,cmdid,userid,result, pri_msg=out_info;
            return_msg = return_msg + "<p>Cmdid: " + str(cmdid) + "     Result: " + str(result)+"</p>"
        else:
            break

#    thread.start_new_thread(capture_message, (p,))
    
    # print the message we capture        

#    print num_cmds
    

    
            # now it's time to excute these commands
    return_msg = return_msg + "<HR style=\"FILTER: alpha(opacity=100,finishopacity=0,style=1)\" width=\"80%\" color=\#987cb9 SIZE=3 align=LEFT>" + "<p>Begin sending packages...</p>"
    for m in range(0, num_cmds):
        pair=dict[m].values()
        num_param = len(pair)
#        print pair, num_param

        primsg=""
        for i in range(0, num_param):
            format='>'+pair[i][0]
#                print pair[i][0],pair[i][1]
            value = str(pair[i][1])
            if (pair[i][0]=='L' or pair[i][0]=='B'):
                value = int(pair[i][1])
#                elif pair[i][0]=='B':
#                   value = 

                #print value
 #               format = format.encode("utf-8")
                
                primsg= primsg + struct.pack(format, value)

        
        sendbuf = p.pack(int(cmd_ids[m]), int(login_userid), primsg)
        p.get_socket().send(sendbuf)
        time.sleep(float(intervals[m]))

        while 1: #wait for the corresponding msg
            out_info=p.getrecvmsg()	
            if(out_info):
                proto_len,version,cmdid,userid,result, pri_msg=out_info
                if pri_msg==primsg and cmdid==cmd_ids[m]: #make sure the received package is the same with the sended one
                    return_msg = return_msg + "<p>recving package %s ...</p>" %m
                    return_msg = return_msg + "<p>Cmdid: " + str(cmdid) + "     Result: " + str(result)+"</p>"
                    break
                else: # ignore the package
                    continue
            else:
                    return_msg = return_msg+"<HR style=\"FILTER: alpha(opacity=100,finishopacity=0,style=1)\" width=\"80%\" color=\#987cb9 SIZE=3 align=LEFT> "+"<p>Send pacakge %s error, Cmdid: %s</p>" %(m, cmd_ids[m])
                    return_msg = return_msg+"<p>sockect disconnected</p>"
                    if m==(num_cmds-1):
                        break
                    return_msg = return_msg+"<p>login again...</p>"
                    p=protocol.mole_online("10.1.1.5",7777 ,4, int(login_userid), str(login_passwd)) #login on login server
                    time.sleep(1)
                    
                    while 1:
                        out_info=p.getrecvmsg()	
                        if(out_info):
                            proto_len,version,cmdid,userid,result, pri_msg=out_info;
                            return_msg = return_msg + "<p>Cmdid: " + str(cmdid) + "     Result: " + str(result)+"</p>"
                        else:
                            break
            
                    return_msg = return_msg + "<HR style=\"FILTER: alpha(opacity=100,finishopacity=0,style=1)\" width=\"80%\" color=\#987cb9 SIZE=3 align=LEFT> " +"<p>Send packages again...</p>"
                    break

    
    return """\
<html><body>
%s
</body></html>
""" % return_msg

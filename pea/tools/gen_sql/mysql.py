#!/usr/bin/env python
# -*- coding: utf-8 -*-
import MySQLdb as mdb
import sys
import optparse


def main(argv):
    p = optparse.OptionParser(
            description='do mysql operation',
            prog='mysql',
            version='%prog 0.1 ',
            usage='%prog module ' );
    options, arguments = p.parse_args()
    for m in arguments:
        exec ("import %s" % m)
        print m


    con = None

    try:

        con = mdb.connect('10.1.1.64', 'root', '123', 'test');
        #con = mdb.connect('10.1.1.131', 'pea', 'pea@pwd', 'mysql');

        cur = con.cursor()


        for m in arguments:
            exec ("%s.sql(cur)" % m)

    except mdb.Error, e:

        print "Error %d: %s" % (e.args[0],e.args[1])
        sys.exit(1)





    finally:    

        if con:    
            con.close()



if "__main__" == __name__:
    main(sys.argv)
    pass

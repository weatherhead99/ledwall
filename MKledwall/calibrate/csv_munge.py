# -*- coding: utf-8 -*-
"""
Created on Sun Aug  7 21:17:54 2016

@author: danw
"""

import csv

import os

os.chdir("good_data")
fls = [i for i in os.listdir(os.curdir) if ".csv" in i]


addrs = ["108.64.0." + str(a) for a  in [80,81,82,83,84,85]]


f_out = "master.csv"

port = 17200

headings = ["Number", "IP" , "Port", "Address", "X","Y"]
#global, IP, port, address, x, y

with open(f_out,"w") as outf:
    globalidx = 0
    writer = csv.writer(outf)
    writer.writerow(headings)
    for a,f in zip(addrs,fls):
        with open(f) as fl:
            print "input file: " + f
            print("ip addr:") + a
            reader = csv.reader(fl)
            
            for row in reader:
                idx = row[0]
                x = row[1]
                y = row[2]
                
                writer.writerow([globalidx,a,port,idx,x,y])
                globalidx += 1
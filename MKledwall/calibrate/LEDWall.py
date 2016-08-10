# -*- coding: utf-8 -*-
"""
Created on Sun Aug  7 15:16:20 2016

@author: danw
"""


import socket
import struct
import time

class LEDWallUDP(object):
    LED_WALL_PORT= 17200
    STRUCT_FMT= "HBBHHi"
    def __init__(self,target_ip,numleds):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        #self.sock.setsockopt(socket.SOL_SOCKET,socket.SO_REUSEADDR, 1)
        self.sock.bind(('',self.LED_WALL_PORT))        
  
        self.numleds = numleds
        self.target_ip = target_ip
    
    def makeheader(self):
        
        #return struct.pack(self.STRUCT_FMT, self.numleds, 1, 1, 0, 0, 0)
        return "".join([chr(0)] * 12)
        
    def set_leds(self,leds):
        hdr = self.makeheader()
        self.sock.sendto(hdr + str(leds), (self.target_ip,self.LED_WALL_PORT))
        
    def set_leds_wait_reply(self,leds,timeout=0.2):
        hdr = self.makeheader()
        
        start = time.clock()
        self.sock.sendto(hdr + str(leds), (self.target_ip,self.LED_WALL_PORT))
        
        self.sock.settimeout(timeout)
        
        
        
        data,addr = self.sock.recvfrom(12)
        end  = time.clock()
            
        return (end-start)
        
    def __del__(self):
        self.sock.close()

# -*- coding: utf-8 -*-
"""
Created on Sat Aug  6 22:26:44 2016

@author: danw
"""
from LEDWall import *
      
if __name__ == "__main__":
    
    a = LEDWallUDP("100.64.0.82",250)
    
    p2 = "".join([chr(255) for i in xrange(250*3)])
    time = a.set_leds(p2)
    
    
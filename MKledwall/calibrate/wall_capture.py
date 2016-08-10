# -*- coding: utf-8 -*-
"""
Created on Fri Aug  5 23:04:51 2016

@author: danw
"""

import cv2 
import numpy as np
import matplotlib.pyplot as plt



import numpy as np
from LEDWall import *

TIMEOUT=2.
RETRIES = 5

def wallsingle(wall,addr,col=[100,100,100]):
    s = [0] * 250 * 3
    s[addr*3] = col[0]
    s[addr*3+1] = col[1]
    s[addr*3+2] = col[2]
    
    p = "".join([chr(j) for j in s])
    tm = wall.set_leds_wait_reply(p,TIMEOUT)
    return tm

def wallfill(wall,col=[100,0,0]):
    s=  col * 250 
    p = "".join([chr(j) for j in s])
    tm = wall.set_leds_wait_reply(p,TIMEOUT)
    return tm
        

def wallblank(wall):
    s = [0] * 250 * 3
    p = "".join([chr(j) for j in s]) 
    tm = wall.set_leds_wait_reply(p,TIMEOUT)
    return tm


import time

def capture_images(cap,wall,addr,delay=0.05):
    tm = wallblank(wall)
    ret,dark = cap.read()    
    tm += wallsingle(wall,addr,col=[150,0,0])
    time.sleep(delay*2)
    ret, im = cap.read()
    
    return dark,im,tm
    
    
from skimage.feature import blob_dog


import matplotlib.pyplot as plt
import scipy.ndimage
def locate_blob(im):
    im2 = scipy.ndimage.gaussian_filter(im,0.3)
    blobs  = blob_dog(im2,max_sigma=20, threshold = 0.3)
    return blobs
    
    
def locate_blob_by_max(im,blur_radius=9):
    blurim = cv2.GaussianBlur(im,(blur_radius,blur_radius),0)
    minval,maxval,minloc,maxloc = cv2.minMaxLoc(blurim)
    
    return maxloc, maxval

import csv
def savevals(xs,ys,vals,fname):
    with open(fname,"w") as f:
        wrt = csv.writer(f)
        for ind,(x,y,v) in enumerate(zip(xs,ys,vals)):
            wrt.writerow([ind,x,y,v])


import contextlib

@contextlib.contextmanager
def cv2capture(idx):
    print("starting video capture")
    cap = cv2.VideoCapture(1)    
    yield cap
    print("releasing video capture")
    cap.release()    
    


if __name__ == "__main__":
    
    with cv2capture(1) as cap:

         addrs = [84,85] #run 2
         NUM_LEDS = 250
         
     
         cs = ["red","green","blue","yellow","cyan","orange"]
         citer = iter(cs)
         fig,ax = plt.subplots(1,1) 
         
         ret,globdarkim = cap.read()
         try:
             for ind,a in enumerate(addrs):
                 s = LEDWallUDP("100.64.0." + str(a),NUM_LEDS)
                 
                 for i in xrange(RETRIES):                     
                     try:
                         wallblank(s)
                         break
                     except socket.timeout as err:
                         print "blank failed, retrying"
                         continue
                 
                 print "SS: " + str(ind)
                 xs = []
                 ys = []
                 vals = []
                 ims = []
                 times = []
    
                 for i in xrange(250):
                     tries = 0
                     print(i)
                     captured = False
                     while(tries < RETRIES):
                         try:
                             darkim, im1 ,tm = capture_images(cap,s,i)
                             captured = True
                             break
                         except socket.timeout as err:
                             print("pixel timeout, retrying")
                             tries += 1
                             continue
                     if not captured:                             
                         print("pixel timeout, skipping")
                         xs.append(0)
                         ys.append(0)
                         vals.append(0)
                         times.append(0)
                         continue
                     
                     sub = cv2.subtract(im1,darkim)
                     (x,y),maxval = locate_blob_by_max(sub[:,:,0])
                     xs.append(x)
                     ys.append(y)
                     ims.append(im1)
                     cv2.circle(im1,(x,y),10,(0,0,255))
                     cv2.imshow("frame",im1)
                     
                     cv2.waitKey(20)
                     times.append(tm)
                     vals.append(maxval)
                     

                     for i in xrange(RETRIES):                     
                         try:
                             wallblank(s)
                             break
                         except socket.timeout as err:
                             print "blank failed, retrying"
                             continue
                
                 ax.imshow(globdarkim)
                 ax.scatter(xs,ys,c=next(citer))
             
                 savevals(xs,ys,vals, "SS_"+str(ind)+".csv")
                 ims = np.array(ims)
                 np.save("ims_"+str(ind) ,ims)
                 np.save("darkim",globdarkim)
                 s.sock.close()
                 del s
                 time.sleep(5)
         except socket.error:
            
            raise 
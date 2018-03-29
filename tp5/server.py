#!/usr/bin/env python
import os, time, sys

f2sName = '/tmp/f2s_tp5'
#if not os.path.exists(f2sName):
 #  os.mkfifo(f2sName)
f2s = open(f2sName,'w')

rqt = raw_input("Enter your request: ")
f2s.write(rqt)
f2s.flush()

f2s.close()

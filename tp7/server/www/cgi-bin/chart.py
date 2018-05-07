#!/usr/bin/env python

'''
PERI TP7
Nicolas Phan
Bryan Aggoun

This CGI script generates a google chart link that displays the 'nbsamples'
most recent Arduino temperature samples stored in the database.
'''

import time as ltime

nbsamples = 10
lines = open("/home/pi/aggoun-phan/my_db.dat", "r").readlines()

# Google Charts link header
html="""
<h3> Chart : </h3>
<img src="http://chart.googleapis.com/chart?
cht=lxy
&chs=250x200
&chd=t:"""

# Parsing the nbsamples'th last line
[val,time] = lines[-nbsamples].split(";");
vals = val
times = time;
time_min = int(time); # Knowing the min value is useful for chart scaling
# Parsing all the following lines until the end of the file
for line in lines[-nbsamples + 1:] :
	[val,time] = line.split(";");
	vals += "," + val
	times += "," + time;
html += times + "|" + vals
time_max = int(time); # Useful for chart scaling

# Google Charts link footer
#time_min_sec = int(ltime.strftime("%S", ltime.localtime(time_min)));
#time_max_sec = int(ltime.strftime("%S", ltime.localtime(time_max)));
#time_min_sec = datetime.utcfromtimestamp(time_min).strftime('%S');
#time_max_sec = datetime.utcfromtimestamp(time_max).strftime('%S');
html += """&chxt=x,y
&chds=%d,%d,%d,%d
&chxr=0,%d,%d,1|1,%d,%d
&chxs=0,0000ff,11,0,lt|1,0000ff,10,1,lt"
 alt="Chart" /><br />
<b>Samples taken at %s</b>
""" % (time_min, time_max, 0, 600, 0, time_max - time_min, 0, 600,
ltime.strftime("%D %H:%M:%S", ltime.localtime(time_min)))

print html

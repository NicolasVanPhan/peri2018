#!/usr/bin/env python

nbsamples = 10
lines = open("/home/pi/aggoun-phan/my_db.dat", "r").readlines()

# Google Charts link header
html="""
<h3> Chart : </h3>
<img src="http://chart.googleapis.com/chart?
cht=lxy
&chs=250x200
&chd=t:"""

# Parsing the nbsamples'th line from the end
[val,time] = lines[-nbsamples].split(";");
vals = val
times = time;
time_min = int(time); # Knowing the min value is useful for chart scaling
# Parsing all the following lines until the end
for line in lines[-nbsamples + 1:] :
	[val,time] = line.split(";");
	vals += "," + val
	times += "," + time;
html += times + "|" + vals
time_max = int(time); # Useful for chart scaling

# Google Charts link footer
html += """&chxt=x,y
&chds=%d,%d,%d,%d
&chxr=0,%d,%d,5|1,%d,%d
&chxs=0,0000ff,11,0,lt|1,0000ff,10,1,lt"
 alt="Les choses, parce que la base" />
""" % (time_min, time_max, 300, 600, time_min, time_max, 300, 600)

print html

import csv 
import matplotlib.pyplot as plt 

fname  ="/home/test3/Downloads/iphone/odometry/outside1.txt"
csvfile = open(fname, newline="")

x = []
y = []
spamreader = csv.reader(csvfile, delimiter=' ', quotechar='"')
for row in spamreader:
    x.append(float(row[7]))
    y.append(float(row[3]))
csvfile.close()

plt.plot(x,y)
plt.show()

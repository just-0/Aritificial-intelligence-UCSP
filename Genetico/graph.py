import csv
import matplotlib.pyplot as plt
 
def readf(filename):
    x = []
    y = []
    with open(filename, mode ='r')as file:
        csvFile = csv.reader(file)
        for lines in csvFile:
                x.append(int(lines[0]))
                y.append(float(lines[1]))
    return x, y


x, y = readf("max.csv")
plt.plot(x, y, label="Max value")

x, y = readf("means.csv")
plt.plot(x, y, label="Mean")

#x, y = readf("total.csv")
#plt.plot(x, y)
  
plt.legend(loc="lower right")

plt.xlabel('Generation')
plt.ylabel('Value')
  
plt.title('Generation growth')

plt.show()

import numpy as np
import matplotlib.pyplot as mtp

#Load txt files with positions (removed first lines with zeros)
ca = np.loadtxt('leg.txt')
le = np.loadtxt('camera.txt')



X = ca[:, 0]
Y = ca[:, 1]
hom = np.ones_like((ca[:, 0]))

Xl = le[:,0]
Yl = le[:,1]
solve = []

A = np.zeros((2*len(X),6))
b = ca.reshape(2 * len(X), 1)


for a in range(len(X)):
    A[2 * a][0] = Xl[a]
    A[2 * a][1] = Yl[a]
    A[2 * a][2] = 1
    A[2 * a][3] = 0
    A[2 * a][4] = 0
    A[2 * a][5] = 0
    #A[2 * a][6] = -X[a]*Xl[a]
    #A[2 * a][7] = -X[a] * Yl[a]
    A[2 * a+1][0] = 0
    A[2 * a+1][1] = 0
    A[2 * a+1][2] = 0
    A[2 * a+1][3] = Xl[a]
    A[2 * a+1][4] = Yl[a]
    A[2 * a+1][5] = 1
    #A[2 * a+1][6] = -Y[a] * Xl[a]
    #A[2 * a+1][7] = -Y[a] * Yl[a]


solve=np.linalg.pinv(A).dot(b)

solve=np.append(solve,[0,0,1])

#transform matrix
solve=solve.reshape((3,3))


print(solve)

a = np.array([Xl, Yl, hom])
res = np.zeros_like((a))

a=a.T
res=res.T
diff=0

for i in range(len(a)):
    res[i]=np.matmul(solve, a[i])
    diff=diff+np.sqrt(((X[i] - res[i,0]) ** 2) + ((Y[i] - res[i,1]) ** 2))


diff=diff/len(a)
print(diff)



mtp.plot(X,Y,label='Camera')

mtp.plot(res[:,0], res[:,1], label="Sensor")
mtp.legend()
mtp.grid(True)
mtp.show()


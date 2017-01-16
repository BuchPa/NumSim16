import collections
import math

error = [0.0, 0.0, 0.0]

# 0: ID
# 1: Re
# 2: t
# 3-4: x,y of p1
# 5-7: u,v,p of p1
# 8-9: x,y of p2
# 10-12: u,v,p of p2
# 13-14: x,y of p3
# 15-17: u,v,p of p3

with open("../CSV/eq_50.csv") as file:
	for line in file:
		cells = line.split(",")
		t = float(cells[2])
		if t != 50.0:
			continue
		error[0] += float(cells[5])

	error[0] = error[0] / 50

with open("../CSV/eq_100.csv") as file:
	for line in file:
		cells = line.split(",")
		t = float(cells[2])
		if t != 50.0:
			continue
		error[1] += float(cells[5])

	error[1] = error[1] / 100

with open("../CSV/eq_200.csv") as file:
	for line in file:
		cells = line.split(",")
		t = float(cells[2])
		if t != 50.0:
			continue
		error[2] += float(cells[5])

	error[2] = error[2] / 200

error[0] = abs(error[0] - error[2])
error[1] = abs(error[1] - error[2])
error[2] = abs(error[2] - error[2])

with open("conv_u_p1", "w") as file:
	file.write(str(50) + "\t" + str(error[0]) + "\n")
	file.write(str(100) + "\t" + str(error[1]) + "\n")
	file.write(str(200) + "\t" + str(error[2]) + "\n")
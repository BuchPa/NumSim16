import collections
import math

mean = {}

# 0: ID
# 1: Re
# 2: t
# 3-4: x,y of p1
# 5-7: u,v,p of p1
# 8-9: x,y of p2
# 10-12: u,v,p of p2
# 13-14: x,y of p3
# 15-17: u,v,p of p3

with open("../CSV/mc_2000.csv") as file:
	for line in file:
		cells = line.split(",")
		t = float(cells[2])
		if t != 50.0:
			continue
		mean[float(cells[1])] = [float(cells[5]), float(cells[6]), float(cells[10]), \
			float(cells[11]), float(cells[15]), float(cells[16])]

mean = collections.OrderedDict(sorted(mean.items()))

with open("u_over_reynolds_p1", "w") as file:
	for re in mean:
		file.write(str(re) + "\t" + str(mean[re][0]) + "\n")

with open("v_over_reynolds_p1", "w") as file:
	for re in mean:
		file.write(str(re) + "\t" + str(mean[re][1]) + "\n")

with open("u_over_reynolds_p2", "w") as file:
	for re in mean:
		file.write(str(re) + "\t" + str(mean[re][2]) + "\n")

with open("v_over_reynolds_p2", "w") as file:
	for re in mean:
		file.write(str(re) + "\t" + str(mean[re][3]) + "\n")

with open("u_over_reynolds_p3", "w") as file:
	for re in mean:
		file.write(str(re) + "\t" + str(mean[re][4]) + "\n")

with open("v_over_reynolds_p3", "w") as file:
	for re in mean:
		file.write(str(re) + "\t" + str(mean[re][5]) + "\n")
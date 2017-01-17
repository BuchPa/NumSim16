import collections
import math

re_bin = []
u_bin = []
nr_bins = 12
N = 2000
u_lower_limit = [0.009, -0.055, -0.06]
u_upper_limit = [0.018, -0.03, -0.046]
u_step = [
	(u_upper_limit[0] - u_lower_limit[0]) / nr_bins,
	(u_upper_limit[1] - u_lower_limit[1]) / nr_bins,
	(u_upper_limit[2] - u_lower_limit[2]) / nr_bins,
]
re_lower_limit = 999.0
re_upper_limit = 2001.0
re_step = (re_upper_limit - re_lower_limit) / nr_bins

# 0: ID
# 1: Re
# 2: t
# 3-4: x,y of p1
# 5-7: u,v,p of p1
# 8-9: x,y of p2
# 10-12: u,v,p of p2
# 13-14: x,y of p3
# 15-17: u,v,p of p3

for k in range(0, 3):
	u_bin.append([])
	for i in range(0, nr_bins + 1):
		u_bin[k].append(0)

for i in range(0, nr_bins + 1):
	re_bin.append(0)

with open("../CSV/mc_2000.csv") as file:
	for line in file:
		cells = line.split(",")
		t = float(cells[2])
		if t != 50.0:
			continue

		u = [float(cells[5]), float(cells[10]), float(cells[15])]
		re = float(cells[1])

		# Bin the u value
		for k in range(0, 3):
			for i in range(0, nr_bins + 1):
				if u[k] >= u_lower_limit[k] + i * u_step[k] and u[k] < u_lower_limit[k] + (i+1) * u_step[k]:
					u_bin[k][i] += 1

		# Bin the Re value
		for i in range(0, nr_bins + 1):
			if re >= re_lower_limit + i * re_step and re < re_lower_limit + (i+1) * re_step:
				re_bin[i] += 1

with open("hist_u_p1", "w") as file:
	for i in range(0, nr_bins + 1):
		middle = 0.5 * (u_lower_limit[0] + i * u_step[0] + (i+1) * u_step[0])
		file.write(str(middle) + "\t" + str(u_bin[0][i]) + "\n")

with open("hist_u_p2", "w") as file:
	for i in range(0, nr_bins + 1):
		middle = 0.5 * (u_lower_limit[1] + i * u_step[1] + (i+1) * u_step[1])
		file.write(str(middle) + "\t" + str(u_bin[1][i]) + "\n")

with open("hist_u_p3", "w") as file:
	for i in range(0, nr_bins + 1):
		middle = 0.5 * (u_lower_limit[2] + i * u_step[2] + (i+1) * u_step[2])
		file.write(str(middle) + "\t" + str(u_bin[2][i]) + "\n")

with open("hist_re", "w") as file:
	for i in range(0, nr_bins + 1):
		middle = 0.5 * (re_lower_limit + i * re_step + (i+1) * re_step)
		file.write(str(middle) + "\t" + str(re_bin[i]) + "\n")
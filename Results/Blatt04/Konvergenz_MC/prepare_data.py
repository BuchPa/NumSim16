import collections
import math

overall_mean = [0.0, 0.0, 0.0]
error = {0:[0.0, 0.0, 0.0]}
theory = {0:[0.0, 0.0, 0.0]}
N = 2000

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
		overall_mean[0] += float(cells[5])
		overall_mean[1] += float(cells[10])
		overall_mean[2] += float(cells[15])

	for i in range(0, 3):
		overall_mean[i] = overall_mean[i] / N

	file.seek(0) # Reset to beginning of file

	i = 1
	for line in file:
		cells = line.split(",")
		t = float(cells[2])
		if t != 50.0:
			continue
		if i not in error:
			error[i] = [0.0, 0.0, 0.0]
		error[i][0] = float(cells[5]) + error[i-1][0]
		error[i][1] = float(cells[10]) + error[i-1][1]
		error[i][2] = float(cells[15]) + error[i-1][2]
		i += 1

	error = collections.OrderedDict(sorted(error.items()))

	for i in range(1, N+1):
		error[i][0] = abs(error[i][0] / i - overall_mean[0])
		error[i][1] = abs(error[i][1] / i - overall_mean[1])
		error[i][2] = abs(error[i][2] / i - overall_mean[2])

	for i in range(1, N+1):
		theory[i] = [0.0, 0.0, 0.0]
		theory[i][0] = 0.01/math.sqrt(i)
		theory[i][1] = 0.01/math.sqrt(i)
		theory[i][2] = 0.01/math.sqrt(i)

with open("conv_u_p1", "w") as file:
	for i in range(1, N+1):
		file.write(str(i) + "\t" + str(error[i][0]) + "\t" + str(theory[i][0]) + "\n")

with open("conv_u_p2", "w") as file:
	for i in range(1, N+1):
		file.write(str(i) + "\t" + str(error[i][1]) + "\t" + str(theory[i][1]) + "\n")

with open("conv_u_p3", "w") as file:
	for i in range(1, N+1):
		file.write(str(i) + "\t" + str(error[i][2]) + "\t" + str(theory[i][2]) + "\n")
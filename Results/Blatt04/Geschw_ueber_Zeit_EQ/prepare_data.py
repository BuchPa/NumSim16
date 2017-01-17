import collections
import math

mean = {}
variance = {}
Ns = [50, 100, 200]

# 0: ID
# 1: Re
# 2: t
# 3-4: x,y of p1
# 5-7: u,v,p of p1
# 8-9: x,y of p2
# 10-12: u,v,p of p2
# 13-14: x,y of p3
# 15-17: u,v,p of p3
for N in Ns:
	with open("../CSV/eq_" + str(N) + ".csv") as file:
		for line in file:
			cells = line.split(",")
			t = float(cells[2])
			if t not in mean:
				mean[t] = [0.0, 0.0, 0.0]
			mean[t][0] += float(cells[5])
			mean[t][1] += float(cells[10])
			mean[t][2] += float(cells[15])

		for j in mean:
			mean[j][0] /= N
			mean[j][1] /= N
			mean[j][2] /= N

		file.seek(0) # Reset to file beginning

		for line in file:
			cells = line.split(",")
			t = float(cells[2])
			if t not in variance:
				variance[t] = [0.0, 0.0, 0.0]
			variance[t][0] += math.pow(float(cells[5]) - mean[t][0], 2.0)
			variance[t][1] += math.pow(float(cells[10]) - mean[t][1], 2.0)
			variance[t][2] += math.pow(float(cells[15]) - mean[t][2], 2.0)

		for j in variance:
			for i in range(0, 3):
				# sigma is the squareroot of the variance
				variance[j][i] = math.sqrt(variance[j][i] / (N - 1))

	mean = collections.OrderedDict(sorted(mean.items()))

	with open("eq_" + str(N) + "_u_over_time_p1", "w") as file:
		for j in mean:
			file.write(str(j) + "\t" + str(mean[j][0]) + "\t" + str(variance[j][0]) + "\n")

	with open("eq_" + str(N) + "_u_over_time_p2", "w") as file:
		for j in mean:
			file.write(str(j) + "\t" + str(mean[j][1]) + "\t" + str(variance[j][1]) + "\n")

	with open("eq_" + str(N) + "_u_over_time_p3", "w") as file:
		for j in mean:
			file.write(str(j) + "\t" + str(mean[j][2]) + "\t" + str(variance[j][2]) + "\n")
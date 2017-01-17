plot "eq_50_u_over_time_p1" using 1:2 title "mean of u at point (120,5)" with lines, "" using 1:2:3 title "standard deviation of u" with yerrorbars
set xlabel "time t"
set ylabel "velocity u"
set term png
set output "eq_50_u_over_time_p1.png"
replot
set term x11

plot "eq_50_u_over_time_p2" using 1:2 title "mean of u at point (64,64)" with lines, "" using 1:2:3 title "standard deviation of u" with yerrorbars
set term png
set output "eq_50_u_over_time_p2.png"
replot
set term x11

plot "eq_50_u_over_time_p3" using 1:2 title "mean of u at point (5,120)" with lines, "" using 1:2:3 title "standard deviation of u" with yerrorbars
set term png
set output "eq_50_u_over_time_p3.png"
replot
set term x11

plot "eq_100_u_over_time_p1" using 1:2 title "mean of u at point (120,5)" with lines, "" using 1:2:3 title "standard deviation of u" with yerrorbars
set term png
set output "eq_100_u_over_time_p1.png"
replot
set term x11

plot "eq_100_u_over_time_p2" using 1:2 title "mean of u at point (64,64)" with lines, "" using 1:2:3 title "standard deviation of u" with yerrorbars
set term png
set output "eq_100_u_over_time_p2.png"
replot
set term x11

plot "eq_100_u_over_time_p3" using 1:2 title "mean of u at point (5,120)" with lines, "" using 1:2:3 title "standard deviation of u" with yerrorbars
set term png
set output "eq_100_u_over_time_p3.png"
replot
set term x11

plot "eq_200_u_over_time_p1" using 1:2 title "mean of u at point (120,5)" with lines, "" using 1:2:3 title "standard deviation of u" with yerrorbars
set term png
set output "eq_200_u_over_time_p1.png"
replot
set term x11

plot "eq_200_u_over_time_p2" using 1:2 title "mean of u at point (64,64)" with lines, "" using 1:2:3 title "standard deviation of u" with yerrorbars
set term png
set output "eq_200_u_over_time_p2.png"
replot
set term x11

plot "eq_200_u_over_time_p3" using 1:2 title "mean of u at point (5,120)" with lines, "" using 1:2:3 title "standard deviation of u" with yerrorbars
set term png
set output "eq_200_u_over_time_p3.png"
replot
set term x11
plot "u_over_time_p1" using 1:2 title "mean of u at point (120,5)" with lines, "" using 1:2:3 title "standard deviation of u" with yerrorbars
set xlabel "time t"
set ylabel "velocity u"
set term png
set output "u_over_time_p1.png"
replot
set term x11

plot "u_over_time_p2" using 1:2 title "mean of u at point (120,5)" with lines, "" using 1:2:3 title "standard deviation of u" with yerrorbars
set xlabel "time t"
set ylabel "velocity u"
set term png
set output "u_over_time_p2.png"
replot
set term x11

plot "u_over_time_p3" using 1:2 title "mean of u at point (120,5)" with lines, "" using 1:2:3 title "standard deviation of u" with yerrorbars
set xlabel "time t"
set ylabel "velocity u"
set term png
set output "u_over_time_p3.png"
replot
set term x11

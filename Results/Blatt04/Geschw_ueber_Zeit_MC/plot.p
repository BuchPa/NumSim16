plot "u_over_time_p1" using 1:2 title "u at point (120,5)" with lines, "" using 1:2:3 title "" with yerrorbars
set term png
set output "u_over_time_p1.png"
replot
set term x11

plot "u_over_time_p2" using 1:2 title "u at point (64,64)" with lines, "" using 1:2:3 title "" with yerrorbars
set term png
set output "u_over_time_p2.png"
replot
set term x11

plot "u_over_time_p3" using 1:2 title "u at point (5,120)" with lines, "" using 1:2:3 title "" with yerrorbars
set term png
set output "u_over_time_p3.png"
replot
set term x11

plot "v_over_time_p1" using 1:2 title "v at point (120,5)" with lines, "" using 1:2:3 title "" with yerrorbars
set term png
set output "v_over_time_p1.png"
replot
set term x11

plot "v_over_time_p2" using 1:2 title "v at point (64,64)" with lines, "" using 1:2:3 title "" with yerrorbars
set term png
set output "v_over_time_p2.png"
replot
set term x11

plot "v_over_time_p3" using 1:2 title "v at point (5,120)" with lines, "" using 1:2:3 title "" with yerrorbars
set term png
set output "v_over_time_p3.png"
replot
set term x11
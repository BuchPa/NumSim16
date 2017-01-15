plot "u_over_reynolds_p1" using 1:2 title "u at point (5,120) at t=50" with points
set term png
set output "u_over_reynolds_p1.png"
replot
set term x11

plot "v_over_reynolds_p1" using 1:2 title "v at point (5,120) at t=50" with points
set term png
set output "v_over_reynolds_p1.png"
replot
set term x11

plot "u_over_reynolds_p2" using 1:2 title "u at point (64,64) at t=50" with points
set term png
set output "u_over_reynolds_p2.png"
replot
set term x11

plot "v_over_reynolds_p2" using 1:2 title "v at point (64,64) at t=50" with points
set term png
set output "v_over_reynolds_p2.png"
replot
set term x11

plot "u_over_reynolds_p3" using 1:2 title "u at point (120,5) at t=50" with points
set term png
set output "u_over_reynolds_p3.png"
replot
set term x11

plot "v_over_reynolds_p3" using 1:2 title "v at point (120,5) at t=50" with points
set term png
set output "v_over_reynolds_p3.png"
replot
set term x11
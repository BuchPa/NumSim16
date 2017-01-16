plot "conv_u_p1" using 1:2 title "error of u at point (5,120)" with lines
set term png
set output "conv_u_p1.png"
replot
set term x11

plot "conv_v_p1" using 1:2 title "error of v at point (5,120)" with lines
set term png
set output "conv_v_p1.png"
replot
set term x11

plot "conv_u_p2" using 1:2 title "error of u at point (64,64)" with lines
set term png
set output "conv_u_p2.png"
replot
set term x11

plot "conv_v_p2" using 1:2 title "error of v at point (64,64)" with lines
set term png
set output "conv_v_p2.png"
replot
set term x11

plot "conv_u_p3" using 1:2 title "error of u at point (120,5)" with lines
set term png
set output "conv_u_p3.png"
replot
set term x11

plot "conv_v_p3" using 1:2 title "error of v at point (120,5)" with lines
set term png
set output "conv_v_p3.png"
replot
set term x11
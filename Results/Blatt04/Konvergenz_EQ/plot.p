plot "conv_u_p1" using 1:2 title "error of u at point (5,120)" with lines
set term png
set output "conv_u_p1.png"
replot
set term x11
set logscale xy
set xrange [1:2000]
set format y "10^{%L}"
set ylabel "Absolute error"
set xlabel "Number of samples"
plot "conv_u_p1" using 1:2 title "error of u at point (120,5)" with lines, \
     "conv_u_p1" using 1:3 title "O(1/sqrt(n))" with lines
set term png
set output "conv_u_p1.png"
replot
set term x11

set logscale xy
set xrange [1:2000]
set format y "10^{%L}"
set ylabel "Absolute error"
set xlabel "Number of samples"
plot "conv_u_p2" using 1:2 title "error of u at point (64,64)" with lines, \
     "conv_u_p2" using 1:3 title "O(1/sqrt(n))" with lines
set term png
set output "conv_u_p2.png"
replot
set term x11

set logscale xy
set xrange [1:2000]
set format y "10^{%L}"
set ylabel "Absolute error"
set xlabel "Number of samples"
plot "conv_u_p3" using 1:2 title "error of u at point (5,120)" with lines, \
     "conv_u_p3" using 1:3 title "O(1/sqrt(n))" with lines
set term png
set output "conv_u_p3.png"
replot
set term x11
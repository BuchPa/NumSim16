set logscale xy
set xrange [50:100]
set xtics (50,100)
set format y "10^{%L}"
set ylabel "Absolute error"
set xlabel "Number of samples"
plot "conv_u_p1" using 1:2 title "error of u at point (5,120)" with lines, \
     "conv_u_p1" using 1:3 title "O(h^1)=O(n^{-1})" with lines, \
     "conv_u_p1" using 1:4 title "O(h^2)=O(n^{-2})" with lines
set term png
set output "conv_u_p1.png"
replot
set term x11
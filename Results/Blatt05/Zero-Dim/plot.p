set yrange [0.0:10.0]
set xlabel "Time t"
set ylabel "Concentration c"
plot "output" using 1:2 title "predator c_1" with lines, "" using 1:3 title "prey c_2" with lines
set term png
set output "n2_explodiert.png"
replot
set term x11
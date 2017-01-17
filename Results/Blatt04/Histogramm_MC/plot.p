plot "hist_u_p1" using 1:2 title "number of samples within bin" with boxes
set xlabel "velocity u"
set ylabel "number of samples"
set term png
set output "hist_u_p1.png"
replot
set term x11

plot "hist_u_p2" using 1:2 title "number of samples within bin" with boxes
set term png
set output "hist_u_p2.png"
replot
set term x11

plot "hist_u_p3" using 1:2 title "number of samples within bin" with boxes
set term png
set output "hist_u_p3.png"
replot
set term x11

plot "hist_re" using 1:2 title "number of samples within bin" with boxes
set xlabel "reynolds number"
set term png
set output "hist_re.png"
replot
set term x11
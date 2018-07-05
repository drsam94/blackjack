#!/usr/bin/env gnuplot
set terminal pngcairo size 1024, 1024
stats 'blackjack.dat' using 1 name "Data"
set output 'histogram.png'
set key off
set border 5
set yzeroaxis
set boxwidth 1 absolute
set style fill solid 5.0 noborder
bin_width = 10
bin_number(x) = floor(x / bin_width)
rounded(x) = bin_width * (bin_number(x) + 0.5)
set label sprintf("m = %.3f", Data_mean) at graph 0.9,0.9 center font "Symbol,32"
set label sprintf("s = %.3f", Data_stddev) at graph 0.9,0.8 center font "Symbol,32"
plot 'blackjack.dat' using (rounded($1)):(1) smooth frequency with boxes


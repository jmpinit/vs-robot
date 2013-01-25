set title "Motor Velocity versus Encoder Ticks (per 1 second)"
set xlabel "motor v"
set ylabel "encoder ticks"
set multiplot

f(x) = a*x
fit f(x) 'motor_vs_encoder.dat' via a

plot "motor_vs_encoder.dat" with lines
plot f(x)
pause -1 "Hit any key to continue"

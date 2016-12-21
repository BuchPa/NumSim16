### Wrapping parameters from exercise sheet to Magrathea
# Parameters
iMax=160
jMax=32
xLength=5.0
yLength=1.0
tEnd=50.0
tau=0.5
deltaT=0.5
eps=0.001
omega=1.7
alpha=0.9
iterMax=500
Re=10000
output=./scenarios/free_sim

### Choose a scenario

## Karman vortex street (-> alpha = angle of the obsacle
##                      -> Choose Pi for appropriate pressure gradient)
scenario="Karman"
Pi=0.1

## Velocity driven channel (-> Ui is calculated such that pressure difference equals Pi)
# scenario="Channel"
# Pi=0.1
# Ui=$(bc <<< "${yLength}*${yLength}*0.125*${Re}*${Pi}/${xLength}")

## Pressure driven channel (-> Choose Pi for appropriate pressure gradient)
# scenario="PressureChannel"
# Pi=0.1

## Pressure driven channel with step (-> Choose Pi for appropriate pressure gradient)
# scenario="Step"
# Pi=0.1

### Execution
if [ $scenario = "Karman" ];
  then pre=4
elif [ $scenario = "Channel" ];
  then pre=1
elif [ $scenario = "PressureChannel" ];
  then pre=2
elif [ $scenario = "Step" ];
  then pre=3
else pre=1
fi
./Magrathea/magrathea -alpha $alpha -dt $deltaT -eps $eps -iter $iterMax -omg $omega -re $Re -tau $tau -tend $tEnd -length "${xLength}x${yLength}" -pressure $Pi -size "${iMax}x${jMax}" -speed "${Ui}x${Vi}" -o $output -pre $pre

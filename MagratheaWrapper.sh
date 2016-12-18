## Wrapping parameters from exercise sheet to Magrathea
# Parameters
iMax=160
jMax=32
xLength=5.0
yLength=1.0
tEnd=50
tau=0.5
deltaT=0.5
eps=0.001
omega=1.7
alpha=0.9
iterMax=500
Re=10000
Ui=1.0
Vi=0.0
Pi=0.0
output=./scenarios/free_sim
# scenario="Karmann"
scenario="Channel"
# scenario="PressureChannel"
# scenario="Step"

# Execution
if [ $scenario = "Karmann" ];
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
### This script eases the use of Magrathea, especially in relation to using it
### for the various excercises of the course for which the whole program was
### written.
###
### Quick usage:
### ./MagratheaWrapper.sh -n "driven_cavity" -t "normal" -m1500 -s167
### This will create a simulation package for the driven cavity scenario where
### the reynolds number is selected from a normal distribution with mean 1500
### and sigma 167.
###
### Using this script will overwrite the files "scenarios/free_sim.param" and
### "scenarios/free_sim.geom" with the generated simulation and geometry
### parameters.
###
### The following arguments are supported:
###
### n: Scenario name. Implemented are "Driven_cavity", "Karman", "Channel",
###    "PressureChannel" and "Step"
###
### t: The type of distribution from which the reynolds number should be
###    selected. Implemented are "normal" and "equi", which correspond to a
###    normal and a equidistant distribution. Both these require additional
###    arguments (see below). All other values of t default to a fixed value.
###
### m: The mean of the distribution of the reynolds number
###
### s: The sigma of the distribution of the reynolds number
###
### d: The number of steps for the equidistant distribution of the reynolds
###    number
###
### i: The current step number for the equidistant distribution of the reynolds
###    number. Steps should be numbered starting from zero.

### Default parameters
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
ReMean=1500
ReSigma=167
ReType=fixed
ReNrIter=200
ReIter=1
output=./scenarios/free_sim

### Check if the reynolds number should have a fixed value, be selected from a
### normal distribution or a value from a given equidistant distribution.
### We also check the scenario name here.
while getopts ":n:m:s:t:d:i:" opt; do
  case $opt in
    n)
      scenario="$OPTARG"
    ;;

    m)
      ReMean="$OPTARG"
    ;;

    s)
      ReSigma="$OPTARG"
    ;;

    t)
      ReType="$OPTARG"
    ;;

    d)
      ReNrIter="$OPTARG"
    ;;

    i)
      ReIter="$OPTARG"
    ;;

    \?)
      echo "Invalid option -$OPTARG" >&2
    ;;
  esac
done

### Calculate reynolds number for distribution type
if [ "$ReType" = "fixed" ]
  then
  Re=$ReMean
elif [ "$ReType" = "normal" ]
  then
  Re="$(./random/random $ReMean $ReSigma)"
elif [ "$ReType" = "equi" ]
  then
  step = 6 * $ReSigma / $ReNrIter
  Re = $ReMean - 3 * $ReSigma + $step * $ReIter
fi

### Set/calculate certain parameters for certain scenarios
if [ "$scenario" = "Karman" ]
  then
  pre=4
  Pi=0.1
elif [ "$scenario" = "Channel" ]
  then
  pre=1
  Pi=0.1
  Ui=$(bc <<< "${yLength}*${yLength}*0.125*${Re}*${Pi}/${xLength}")
elif [ "$scenario" = "PressureChannel" ]
  then
  pre=2
  Pi=0.1
elif [ "$scenario" = "Step" ]
  then
  pre=3
  Pi=0.1
elif [ "$scenario" = "DrivenCavity" ]
  then
  jMax=128
  iMax=128
  xLength=1.0
  yLength=1.0
  pre=6
else
  pre=1
fi

### All done, now execute Magrathea
./Magrathea/magrathea -alpha $alpha -dt $deltaT -eps $eps -iter $iterMax -omg $omega -re $Re -tau $tau -tend $tEnd -length "${xLength}x${yLength}" -pressure $Pi -size "${iMax}x${jMax}" -speed "${Ui}x${Vi}" -o $output -pre $pre

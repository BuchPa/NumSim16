### A utility script to do an uncertainty quantification with both Monte-Carlo
### and the prism rule.
###
### Example use:
### ./UQWrapper.sh -t "normal" -m1500 -s167 -d200
### would perform 200 simulations where the reynolds number each time is selec-
### ted from a normal distribution with mean 1500 and sigma 167.
###
### The following arguments are supported:
###
### t: The type of distribution from which the reynolds number should be
###    selected. Implemented are "normal" and "equi", which correspond to a
###    normal and a equidistant distribution. Both these require additional
###    arguments (see below).
###
### m: The mean of the distribution of the reynolds number
###
### s: The sigma of the distribution of the reynolds number
###
### d: The number of steps for the equidistant distribution of the reynolds
###    number or the number of randomly selected points for the normal
###    distribution.

end=200
mean=1500
sigma=167
type="normal"
i=0

while getopts ":m:s:t:d:" opt; do
  case $opt in
    m)
      mean="$OPTARG"
    ;;

    s)
      sigma="$OPTARG"
    ;;

    t)
      type="$OPTARG"
    ;;

    d)
      end="$OPTARG"
    ;;

    \?)
      echo "Invalid option -$OPTARG" >&2
    ;;
  esac
done

until [ $i -gt `expr $end - 1` ]
  do
  if [ $type = "normal" ]
    then
    ./MagratheaWrapper.sh -n \"DrivenCavity\" -m$mean -s$sigma -t \"normal\"
    ./build/NumSim
  elif [ $type = "equi" ]
    then
    ./MagratheaWrapper.sh -n \"DrivenCavity\" -m$mean -s$sigma -t \"equi\" -d$end -i$i
    ./build/NumSim
  fi
  i=`expr $i + 1`
done
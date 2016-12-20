# NumSim16
Numerische Simulation 2016/2017

## Requirements
* g++ for C++ version 11 or greater
* scons
* doxygen (documentation only)

## Installation
1. ```git clone https://github.com/BuchPa/NumSim16.git```
2. ```cd NumSim16```
3. ```mkdir VTK```
4. Compile the main program and the helper program Magrathea. See section "Compile" for instructions
5. ```doxygen doxyfile``` if you want source code documentation in a navigatable format like HTML. The created documentation can be found in the folder ```doc```.

## Compile
### Main program
1. ```scons debug=-1 -j #NP``` where #NP is replaced with the number of processors. Note that ```scons``` without any arguments can be used as well. The parameters then fall back to their default values. It is also not necessary to specify the number of processors at compile time. We assume this is merely a safety feature or may improve performance, though this is untested.

To clear all cached files of the build process, run the shell script ```clear_cmake.sh```, e.g. with ```./clear_cmake.sh```. Make sure to be in the project folder when executing the script.

### Magrathea
1. ```cd Magrathea```
2. ```make```
3. ```cd ..```

## Run
### Running the main program
1. ```./build/NumSim``` using the default scenario "free_sim"
2. ```./build/NumSim scenario <name>``` where ```<name>``` is replaced by the name of the scenario. The scenario names correspond to the file names in the folder scenarios. The following scenarios come prepared:
    * ```channel```: A simple channel flow where the left border inputs fluid with u velocities in a parabola
    * ```step```: A pressure driven channel flow where a step obstacle restricts the fluid to a narrower channel
        on the left side, causing a standing vortex to form
    * ```karman```: A simplified Karman vortex street, where an obstacle in the way of a channel flow causing vortex
        shedding to appear.
    * ```driven_cavity```: A driven cavity problem where the upper border has a fixed velocity and the other three
        borders are walls.

### Using Magrathea to create a customized scenario
The helper program Magrathea can be used to create a customized scenario without having to edit the geometry and parameter files manually. By default the scenario overwrites the existing scenario ```free_sim```. If you want to save a created scenario, simply copy the two files ```free_sim.geom``` and ```free_sim.param``` and rename them to something you'd like. You can then call the main program with your scenario name.

1. ```chmod +x MagratheaWrapper.sh``` Depending on your operating system, you will need to make the shell script ```MagratheaWrapper``` executable
2. ```./MagratheaWrapper.sh scenario <name>``` will create a geometry file from a list of prepared scenarios. This is helpful if your desired geometry is a variation on one of the existing scenarios. The implemented scenarios of Magrathea are: Channel, PressureChannel, Karman and Step. Channel and PressureChannel differ by how the channel flow is created by the boundary values. Channel uses a velocity based approach, while PressureChannel uses a pressure driven one.
3. You can modify the shell script and change the parameter values. Running the shell script again with the modified values will create a variation of the chosen scenario.

### Creating a geometry file manually
Since Magrathea only supports a given number of scenarios, you might need to create a geometry file manually in order to simulate your chosen problem. Let's have a look at an example geometry file:

```
size = 10 10
length = 1.000000 1.000000
velocity = 25.000000 0.000000
pressure = 0.000000
geometry = free
```

The size and length parameters determine the size of the grid and the length of the domain. The velocity and pressure parameters are used for boundary value calculation. The type of boundary will be determined by the flag field (see below), while the actual values are set here. The parameters velocity encodes the u velocity first, the v one second. The line ```geometry = free``` line is special. It says that everything that follows this line is the flag field.

The flag field determines how the domain looks like. That means what type of boundaries there are and if there are any obstacles inside the domain. Let's have a look at an example flag field:

```
##########
#........#
#........#
#........#
#........#
#........#
#........#
#........#
#........#
##########
```

Please note that the flag field should have the same size as determined by the size parameters. The implementation of the fluid simulation uses halo cells to represent the boundaries. So with a size of 10 by 10 the inner domain is actually 8 by 8 cells big. The cell width is calculated by the inner domain. So here each cell would be 1/8 of the domain length wide.

The dots represent fluid cells. These are basically empty space. The pound sign represent NOSLIP boundaries, which are also used for obstacles. NOSLIP means, that the fluid will stick to the boundary cells so that the velocity is zero exactly at the boundary line. The supported cell types are:
* '.' : Fluid
* '#' : Wall/Obstacle/NoSlip
* 'I' : General Inflow boarder
* 'H' : Horizontal Inflow boarder
* 'V' : Vertical Inflow boarder
* 'O' : Outflow
* '|' : Vertical Slip-boundary
* '-' : Horizontal Slip-boundary

## Parameters
The parameters used in the helper program Magrathea (technically the created geometry and parameter files) are as follows:
* iMax : The number of cells in horizontal direction
* jMax : The number of cells in vertical directions
* xLength : The length of the domain in horizontal direction
* yLength : The length of the domain in vertical direction
* re : The Reynolds number
* omg : Relaxation factor
* alpha : Upwind-Differencing factor
* dt : Maximum timestep (actual timestep is dynamic)
* tend : End time
* iterMax : Maximum number of iterations for the solver
* eps : Tolerance for pressure calculation iterations
* tau : "Safety" scaling factor for the timestep

## Testing subsystems
There are tests for the various subsystems of the program. See documentation of the main function for a complete list of them. You can execute the tests by executing the program with one of the test parameters. E.g. ```./numsim TEST_GRID``` performs the tests implemented for the Grid class.

## Disabling live visualisation
Run scons again with the parameter visu turned off. E.g. ```scons visu=0```. After that, execute the program like normal.

## Using the output
The output (as VTK files) can be visualized and analyzed by programs such as paraview. Since the parallel computation creates files for each processor and each frame, the files must be grouped together. Paraview can do this automatically when the correct node in the "Load" function is selected. The naming scheme of the (partial) VTK files is ```field_a_b```, where a is the frame number and b is the process number.

Therefore, to load all frames and stich the data together from each of the process subdomains, select the ```field``` node (usually at the top) when loading files with Paraview.

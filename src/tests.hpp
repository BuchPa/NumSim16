#include "typedef.hpp"
#include "geometry.hpp"
#include "parameter.hpp"

using namespace std;

/// Tests functions/methods of Compute.
void test_compute();

/// Tests functions/methods of Iterator.
///
/// @param comm Communicator* The communicator to use
void test_iterator(Communicator *comm);

/// Tests functions/methods of Geometry.
///
/// @param comm Communicator* The communicator to use
void test_geometry(Communicator *comm);

/// Tests functions/methods of Parameter.
void test_parameter();

/// Tests the interpolate method of Grid.
///
/// @param comm Communicator* The communicator to use
void test_interpolate(Communicator *comm);

/// Tests functions/methods of Grid.
///
/// @param comm Communicator* The communicator to use
void test_grid(Communicator *comm);

/// Tests the load functions of Parameter and Geometry.
///
/// @param param Parameter The Parameter instance to test
/// @param geom Geometry The Geometry instance to test
void test_load(const Parameter *param, const Geometry *geom);

/// Tests functions/methods of Solver.
///
/// @param geom Geometry The Geometry instance to test
/// @param comm Communicator* The communicator to use
void test_solver(const Geometry *geom, const Communicator *comm);
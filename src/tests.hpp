#include "typedef.hpp"
#include "geometry.hpp"
#include "parameter.hpp"

/// Tests functions/methods of Compute.
void test_compute();

/// Tests functions/methods of Iterator.
void test_iterator();

/// Tests functions/methods of Geometry.
void test_geometry();

/// Tests functions/methods of Parameter.
void test_parameter();

/// Tests the interpolate method of Grid.
void test_interpolate();

/// Tests functions/methods of Grid.
void test_grid();

/// Tests the load functions of Parameter and Geometry.
///
/// @param param Parameter The Parameter instance to test
/// @param geom Geometry The Geometry instance to test
void test_load(const Parameter *param, const Geometry *geom);

/// Tests functions/methods of Solver.
///
/// @param geom Geometry The Geometry instance to test
void test_solver(const Geometry *geom);
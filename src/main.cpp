/*
 *  Copyright (C) 2015   Malte Brunn
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "typedef.hpp"
#include "compute.hpp"
#include "geometry.hpp"
#include "parameter.hpp"
#include "visu.hpp"
#include "vtk.hpp"
#include "iterator.hpp"

#include <iostream> // getchar()

void test_compute() {
  printf("Testing Compute\n");
}

void test_iterator() {
  printf("Testing Iterator\n");
  Geometry geom;
  
  printf("Iterator Test:\n");
  Iterator it = Iterator(&geom);;
  it.TestRun(true);
  printf("\n");
  
  printf("Interior Iterator Test:\n");
  InteriorIterator init = InteriorIterator(&geom);
  init.TestRun(true);
  printf("\n");
  
  printf("BoundaryIterator Test Bottom:\n");
  BoundaryIterator boit = BoundaryIterator(&geom, 1);
  boit.SetBoundary(1);
  boit.TestRun(true);
  printf("\n");
  
  printf("BoundaryIterator Test Right:\n");
  boit.SetBoundary(2);
  boit.TestRun(true);
  printf("\n");
  
  printf("BoundaryIterator Test Top:\n");
  boit.SetBoundary(3);
  boit.TestRun(true);
  printf("\n");
  
  printf("BoundaryIterator Test Left:\n");
  boit.SetBoundary(4);
  boit.TestRun(true);
  printf("\n");
}

void test_geometry() {
  printf("Testing Geometry\n");

  // Testing default values
  Geometry geo;
  printf("Size %i,%i\n", geo.Size()[0], geo.Size()[1]);
  printf("Length %f,%f\n", geo.Length()[0], geo.Length()[1]);
  printf("Mesh %f,%f\n", geo.Mesh()[0], geo.Mesh()[1]);

  // Testing update function
  Grid tmp(&geo);
  real_t val = real_t(0.0);
  
  tmp.Initialize(val);
  printf("Init field:\n");
  tmp.Print();
  
  geo.Update_U(&tmp);
  printf("Update U:\n");
  tmp.Print();
  
  tmp.Initialize(val);
  printf("Init field:\n");
  tmp.Print();
  
  geo.Update_V(&tmp);
  printf("Update V:\n");
  tmp.Print();
  
  tmp.Initialize(val);
  printf("Init field:\n");
  tmp.Print();
  
  geo.Update_P(&tmp);
  printf("Update P:\n");
  tmp.Print();

  // Testing Load
  geo.Load("ex1_geometry");
  printf("Size %i (%i)\n", geo.Size()[0], 128);
  printf("Length %f (%f)\n", geo.Length()[0], 1.0);
}

void test_parameter() {
  printf("Testing Parameter\n");

  // Test loading and parsing of params
  Parameter p = Parameter();
  p.Load("ex1_parameter");

  printf("Re %f (1000.0)\n", p.Re());
  printf("IterMax %d (100)\n", p.IterMax());
}

void test_grid() {
  printf("Testing Grid\n");

  // Test interpolate
  const Geometry geo;
  Grid grid = Grid(&geo);
  Grid grid2 = Grid(&geo);
  Iterator it = Iterator(&geo);

  // Init grid with values
  // 3  4  5
  // 2  3  4
  // 0  1  3
  grid.Cell(it) = 0.0;
  it = it.Right();
  grid.Cell(it) = 1.0;
  it = it.Top();
  grid.Cell(it) = 3.0;
  it = it.Left();
  grid.Cell(it) = 2.0;
  it = it.Top();
  grid.Cell(it) = 3.0;
  it = it.Right();
  grid.Cell(it) = 4.0;
  it = it.Right();
  grid.Cell(it) = 5.0;
  it = it.Down();
  grid.Cell(it) = 4.0;
  it = it.Down();
  grid.Cell(it) = 3.0;
  it = it.Left().Top();

  printf("Interpolate: %f (%f)\n", grid.Interpolate({
    0.5 / (geo.Size()[0] - 2),
    0.5 / (geo.Size()[1] - 2)
  }), 1.5);

  // Test difference quotient of first order for the middle cell
  printf("Diff-Quot (1. order) middle\n");
  printf("%f (%f)\n", grid.dx_l(it), 1.0 / geo.Mesh()[0]);
  printf("%f (%f)\n", grid.dx_r(it), 1.0 / geo.Mesh()[0]);
  printf("%f (%f)\n", grid.dy_l(it), 2.0 / geo.Mesh()[1]);
  printf("%f (%f)\n", grid.dy_r(it), 1.0 / geo.Mesh()[1]);

  // Test difference quotient of second order for the middle cell
  printf("Diff-Quot (2. order) middle\n");
  printf("%f (%f)\n", grid.dxx(it), 0.0);
  printf("%f (%f)\n", grid.dyy(it), -1.0 / (geo.Mesh()[1] * geo.Mesh()[1]));

  // Test difference quotient of first order for a corner cell
  printf("Diff-Quot (1. order) corner\n");
  it = it.Down().Left();
  printf("%f (%f)\n", grid.dx_l(it), 0.0);
  printf("%f (%f)\n", grid.dx_r(it), 1.0 / geo.Mesh()[0]);
  printf("%f (%f)\n", grid.dy_l(it), 0.0);
  printf("%f (%f)\n", grid.dy_r(it), 2.0 / geo.Mesh()[1]);

  // Test difference quotient of second order for a corner cell
  printf("Diff-Quot (2. order) corner\n");
  printf("%f (%f)\n", grid.dxx(it), 1.0 / (geo.Mesh()[1] * geo.Mesh()[1]));
  printf("%f (%f)\n", grid.dyy(it), 2.0 / (geo.Mesh()[1] * geo.Mesh()[1]));

  // Init second grid for mixed-term difference quotients
  // 3  4  5
  // 2  3  4
  // 1  2  3
  it.First();
  grid2.Cell(it) = 1.0;
  it = it.Right();
  grid2.Cell(it) = 2.0;
  it = it.Top();
  grid2.Cell(it) = 3.0;
  it = it.Left();
  grid2.Cell(it) = 2.0;
  it = it.Top();
  grid2.Cell(it) = 3.0;
  it = it.Right();
  grid2.Cell(it) = 4.0;
  it = it.Right();
  grid2.Cell(it) = 5.0;
  it = it.Down();
  grid2.Cell(it) = 4.0;
  it = it.Down();
  grid2.Cell(it) = 3.0;
  it = it.Left().Top();

  // Test donor-cell difference quotients for center cell
  printf("DC-Quot center\n");
  printf("%f (%f)", grid.DC_vdu_y(it, 0.5, &grid2), 33.5 / (4.0 * geo.Mesh()[1]));
}

int main(int argc, char **argv) {
  // Printing stupid things to cheer the simpleminded user
  printf("             ███▄    █  █    ██  ███▄ ▄███▓  ██████  ██▓ ███▄ ▄███▓\n");
  printf("             ██ ▀█   █  ██  ▓██▒▓██▒▀█▀ ██▒▒██    ▒ ▓██▒▓██▒▀█▀ ██▒\n");
  printf("            ▓██  ▀█ ██▒▓██  ▒██░▓██    ▓██░░ ▓██▄   ▒██▒▓██    ▓██░\n");
  printf("            ▓██▒  ▐▌██▒▓▓█  ░██░▒██    ▒██   ▒   ██▒░██░▒██    ▒██ \n");
  printf("            ▒██░   ▓██░▒▒█████▓ ▒██▒   ░██▒▒██████▒▒░██░▒██▒   ░██▒\n");
  printf("            ░ ▒░   ▒ ▒ ░▒▓▒ ▒ ▒ ░ ▒░   ░  ░▒ ▒▓▒ ▒ ░░▓  ░ ▒░   ░  ░\n");
  printf("            ░ ░░   ░ ▒░░░▒░ ░ ░ ░  ░      ░░ ░▒  ░ ░ ▒ ░░  ░      ░\n");
  printf("               ░   ░ ░  ░░░ ░ ░ ░      ░   ░  ░  ░   ▒ ░░      ░   \n");
  printf("                     ░    ░            ░         ░   ░         ░   \n");
  printf("Numerische Simulationen 16/17 von\n   Etienne Ott\n   Moritz Schleicher\n   Patrick Buchfink\n\n");
  

  // Create parameter and geometry instances with default values
  Parameter param;
  Geometry geom;
  // Create the fluid solver
  Compute comp(&geom, &param);

#ifdef USE_DEBUG_VISU
  // Create and initialize the visualization
  Renderer visu(geom.Length(), geom.Mesh());
  visu.Init(800, 800);
#endif // USE_DEBUG_VISU

  // Check for specific test
  if (argc > 1) {
    if (strcmp(argv[1], "TEST_COMPUTE") == 0) {
      test_compute();
      return 0;
    }

    if (strcmp(argv[1], "TEST_ITERATOR") == 0) {
      test_iterator();
      return 0;
    }

    if (strcmp(argv[1], "TEST_GEOMETRY") == 0) {
      test_geometry();
      return 0;
    }

    if (strcmp(argv[1], "TEST_PARAMETER") == 0) {
      test_parameter();
      return 0;
    }

    if (strcmp(argv[1], "TEST_GRID") == 0) {
      test_grid();
      return 0;
    }
  }

  // Create a VTK generator
  VTK vtk(geom.Mesh(), geom.Size());

  const Grid *visugrid;
  bool run = true;

  visugrid = comp.GetVelocity();

  // Run the time steps until the end is reached
  while ((comp.GetTime() - param.Tend())<-param.Dt() && run) {
#ifdef USE_DEBUG_VISU
    // Render and check if window is closed
    switch (visu.Render(visugrid)) {
    case -1:
      run = false;
      break;
    case 0:
      visugrid = comp.GetVelocity();
      break;
    case 1:
      visugrid = comp.GetU();
      break;
    case 2:
      visugrid = comp.GetV();
      break;
    case 3:
      visugrid = comp.GetP();
      break;
    default:
      break;
    };
#endif // DEBUG_VISU
    
//     // Wait for user input to debug step-by-step
//     getchar();

    // Create a VTK File in the folder VTK (must exist)
    vtk.Init("VTK/field");
    vtk.AddField("Velocity", comp.GetU(), comp.GetV());
    vtk.AddScalar("Pressure", comp.GetP());
    vtk.Finish();

    // Run a few steps
    for (uint32_t i = 0; i < 9; ++i)
      comp.TimeStep(false);
    comp.TimeStep(true);
  }

  return 0;
}

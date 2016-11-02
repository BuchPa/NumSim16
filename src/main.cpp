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

#include <iostream> // getchar()

void test_compute() {
  printf("Testing Compute\n");
}

void test_iterator() {
  printf("Testing Iterator\n");
}

void test_geometry() {
  printf("Testing Geometry\n");

  // Testing default values
  Geometry geo;
  printf("Size %i,%i\n", geo.Size()[0], geo.Size()[1]);
  printf("Length %f,%f\n", geo.Length()[0], geo.Length()[1]);
  printf("Mesh %f,%f\n", geo.Mesh()[0], geo.Mesh()[1]);
}

void test_parameter() {
  printf("Testing Parameter\n");

  // Test loading and parsing of params
  Parameter p = Parameter();
  p.Load("test_parameter.txt");

  printf("Re %f (1.0)\n", p.Re());
  printf("IterMax %d (5)\n", p.IterMax());
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

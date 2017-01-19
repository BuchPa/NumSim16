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
#include "csv.hpp"
#include "vtk.hpp"
#include "iterator.hpp"
#include "solver.hpp"
#include "tests.hpp"

#include <iostream> // getchar()
#include <chrono> // time functions
#include <string> // string functions
#include <algorithm> // transform()
#include <fstream> // ifstream

using namespace std;

#define MEASURE_TIME true
#define OUTPUT_CSV false
#define OUTPUT_VTK true

/// Returns if the file exists and can be accessed.
///
/// @param name string The filename
/// @return bool If the file exists and is accessible
bool file_exists(string name) {
  ifstream f(name.c_str());
  return f.good();
}

/// The entry point into the simulation program. The following console para-
/// meters are implemented:
///
/// TEST_COMPUTE
/// TEST_ITERATOR
/// TEST_GEOMETRY
/// TEST_PARAMETER
/// TEST_GRID
/// TEST_INTERPOLATE
/// TEST_LOAD
/// TEST_SOLVER
///
/// Console parameters starting with TEST are meant to be used to test specific
/// subsystems of the programs.
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
  
  long start = 0, end = 0;

  if (MEASURE_TIME) {
    start = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::now().time_since_epoch()
    ).count();
  }

  // Create parameter and geometry instances with default values
  Parameter param;
  Geometry geom;
  
  // Check which scenario (if any) we want to simulate
  string scenarioName = "none";
  for (int i = 0; i < argc; i++) {
    string dc = argv[i];
    transform(dc.begin(), dc.end(), dc.begin(), ::tolower);

    if (
      dc == "scenario"
      && i < argc - 1
    ) {
      scenarioName = argv[i + 1];
      transform(scenarioName.begin(), scenarioName.end(),  scenarioName.begin(), ::tolower);
    }
  }

  // Check if scenario exist
  if (
    scenarioName != "none"
    && (
      !file_exists("scenarios/" + scenarioName + ".param")
      || !file_exists("scenarios/" + scenarioName + ".geom")
    )
  ) {
    throw runtime_error(std::string("Unknown scenario: " + scenarioName));
  }

  // Read parameter and geometry files
  if (scenarioName != "none") {
    param.Load(("scenarios/" + scenarioName + ".param").c_str());
    geom.Load(("scenarios/" + scenarioName + ".geom").c_str());
  } else {
    param.Load("scenarios/free_sim.param");
    geom.Load("scenarios/free_sim.geom");
  }
  
  // Create the fluid solver
  Compute comp(&geom, &param);

  #ifdef USE_DEBUG_VISU
  // Create and initialize the visualization
  Renderer visu(geom.Length(), geom.Mesh());
  visu.Init(800, 800 / (geom.Length()[0] / geom.Length()[1]));
  #endif // USE_DEBUG_VISU

  // Check for specific test
  char* test_case = NULL;
  for (int i=0; i<argc; i++){
    if(strstr(argv[i],"TEST") != NULL){
      test_case = argv[i];
      break;
    }
  }
  if (test_case != NULL) {
    if (strcmp(test_case, "TEST_COMPUTE") == 0) {
      test_compute();
      return 0;
    }

    if (strcmp(test_case, "TEST_ITERATOR") == 0) {
      test_iterator();
      return 0;
    }

    if (strcmp(test_case, "TEST_GEOMETRY") == 0) {
      test_geometry();
      return 0;
    }

    if (strcmp(test_case, "TEST_PARAMETER") == 0) {
      test_parameter();
      return 0;
    }

    if (strcmp(test_case, "TEST_GRID") == 0) {
      test_grid();
      return 0;
    }
    
    if (strcmp(test_case, "TEST_INTERPOLATE") == 0) {
      test_interpolate();
      return 0;
    }
    
    if (strcmp(test_case, "TEST_LOAD") == 0) {
      test_load(&param, &geom);
      return 0;
    }
    
    if (strcmp(test_case, "TEST_SOLVER") == 0) {
      test_solver(&geom);
      return 0;
    }
    
  }
  
  // Choose csv evaluation positions
  list<multi_real_t> csv_pos;
  csv_pos.push_back(multi_real_t({120.0/128.0, 5.0/128.0}));
  csv_pos.push_back(multi_real_t({64.0/128.0, 64.0/128.0}));
  csv_pos.push_back(multi_real_t({5.0/128.0, 120.0/128.0}));
  
  // Create a CSV generator
  CSV csv(param.Re(), csv_pos);
  
  // Create a VTK generator
  VTK vtk(geom.Mesh(), geom.Size());
  
  if (OUTPUT_CSV) {
    // Create file in the CSV folder (folder must exist)
    csv.Init("CSV/multirun");
  }

  const Grid *visugrid;
  bool run   = true;
  bool print = true;

  visugrid = comp.GetVelocity();

  // Run the time steps until the end is reached
  while ((param.Tend() - comp.GetTime() > DT_MIN) && run) {

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
      case 4:
        visugrid = comp.GetStream();
        break;
      case 5:
        visugrid = comp.GetVorticity();
        break;
      case 6:
        visugrid = comp.GetC();
        break;
      default:
        break;
    };

    #endif // USE_DEBUG_VISU
    
    if (print){
      
      // Print CSV output in the folder CSV (must exist)
      if (OUTPUT_CSV) {
        // Add an entry to the CSV file (must exist)
        csv.AddEntry(comp.GetTime(), comp.GetU(), comp.GetV(), comp.GetP());
      }
      
      // Print VTK output in the folder VTK (must exist)
      if (OUTPUT_VTK) {
        vtk.Init("VTK/field");
        vtk.AddField("Velocity", comp.GetU(), comp.GetV());
        vtk.AddScalar("Pressure", comp.GetP());
        vtk.AddScalar("Stream", comp.GetStream());
        vtk.AddScalar("Vorticity", comp.GetVorticity());
        vtk.AddScalar("Substance", comp.GetC());
        vtk.Finish();
        
        // Create VTK File for particles of the streakline
        // in the folder VTK (must exist)
        if (comp.GetStreaklines()->size() > 0) {
          vtk.InitParticles("VTK/streaks");
          vtk.AddParticles("Streakline", comp.GetStreaklines());
          vtk.FinishParticles();
        }
        
        // Create VTK File for particles of the streakline
        if (comp.GetParticleTracing()->size() > 0) {
          vtk.InitParticles("VTK/traces");
          vtk.AddParticles("Trace", comp.GetParticleTracing());
          vtk.FinishParticles();
        }
      }
      
    } //end if (print)
    
    print = comp.TimeStep(true);
  }
  
  // Print CSV output in the folder CSV (must exist)
  if (OUTPUT_CSV) {
    // Add an entry to the CSV file (must exist)
    csv.AddEntry(comp.GetTime(), comp.GetU(), comp.GetV(), comp.GetP());
    
    // Finish CSV
    csv.Finish();
  }
  
  // Print VTK output in the folder VTK (must exist)
  if (OUTPUT_VTK) {
    vtk.Init("VTK/field");
    vtk.AddField("Velocity", comp.GetU(), comp.GetV());
    vtk.AddScalar("Pressure", comp.GetP());
    vtk.AddScalar("Stream", comp.GetStream());
    vtk.AddScalar("Vorticity", comp.GetVorticity());
    vtk.AddScalar("Substance", comp.GetC());
    vtk.Finish();
    
    // Create VTK File for particles of the streakline
    // in the folder VTK (must exist)
    if (comp.GetStreaklines()->size() > 0) {
      vtk.InitParticles("VTK/streaks");
      vtk.AddParticles("Streakline", comp.GetStreaklines());
      vtk.FinishParticles();
    }
    
    // Create VTK File for particles of the streakline
    if (comp.GetParticleTracing()->size() > 0) {
      vtk.InitParticles("VTK/traces");
      vtk.AddParticles("Trace", comp.GetParticleTracing());
      vtk.FinishParticles();
    }
  }
  
  printf("Multirun finished!\n");

  if (MEASURE_TIME) {
    end = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::now().time_since_epoch()
    ).count();
    printf("Overall run time (ms): %ld\n", end - start);
  }

  return 0;
}

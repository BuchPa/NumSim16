#include "typedef.hpp"
#include "tests.hpp"
#include "compute.hpp"
#include "geometry.hpp"
#include "parameter.hpp"
#include "visu.hpp"
#include "vtk.hpp"
#include "iterator.hpp"
#include "solver.hpp"

void test_compute() {
  mprintf("Testing Compute\n");
}

void test_iterator() {
  mprintf("Testing Iterator\n");
  Geometry geom;
  
  mprintf("Iterator Test:\n");
  Iterator it = Iterator(&geom);;
  it.TestRun(true);
  mprintf("\n");
  
  mprintf("Interior Iterator Test:\n");
  InteriorIterator init = InteriorIterator(&geom);
  init.TestRun(true);
  mprintf("\n");
  
  mprintf("BoundaryIterator Test Bottom:\n");
  BoundaryIterator boit = BoundaryIterator(&geom, 1);
  boit.SetBoundary(1);
  boit.TestRun(true);
  mprintf("\n");
  
  mprintf("BoundaryIterator Test Right:\n");
  boit.SetBoundary(2);
  boit.TestRun(true);
  mprintf("\n");
  
  mprintf("BoundaryIterator Test Top:\n");
  boit.SetBoundary(3);
  boit.TestRun(true);
  mprintf("\n");
  
  mprintf("BoundaryIterator Test Left:\n");
  boit.SetBoundary(4);
  boit.TestRun(true);
  mprintf("\n");
  
  mprintf("BoundaryIterator Test Corner Bottom Left:\n");
  Iterator cbl = boit.CornerBottomLeft();
  cbl.printNeighbours();
  mprintf("\n");
  
  mprintf("BoundaryIterator Test Corner Bottom Right:\n");
  Iterator cbr = boit.CornerBottomRight();
  cbr.printNeighbours();
  mprintf("\n");
  
  mprintf("BoundaryIterator Test Corner Top Left:\n");
  Iterator ctl = boit.CornerTopLeft();
  ctl.printNeighbours();
  mprintf("\n");
  
  mprintf("BoundaryIterator Test Corner Top Right:\n");
  Iterator ctr = boit.CornerTopRight();
  ctr.printNeighbours();
  mprintf("\n");
  
}

void test_geometry() {
  mprintf("Testing Geometry\n");

  // Testing default values
  Geometry geo;
  mprintf("Size %i,%i\n", geo.Size()[0], geo.Size()[1]);
  mprintf("Length %f,%f\n", geo.Length()[0], geo.Length()[1]);
  mprintf("Mesh %f,%f\n", geo.Mesh()[0], geo.Mesh()[1]);

  // Testing update function
  Grid tmp(&geo);
  real_t val = real_t(0.0);
  
  tmp.Initialize(val);
  mprintf("Init field:\n");
  tmp.Print();
  
  geo.Update_U(&tmp);
  mprintf("Update U:\n");
  tmp.Print();
  
  tmp.Initialize(val);
  mprintf("Init field:\n");
  tmp.Print();
  
  geo.Update_V(&tmp);
  mprintf("Update V:\n");
  tmp.Print();
  
  tmp.Initialize(val);
  mprintf("Init field:\n");
  tmp.Print();
  
  geo.Update_P(&tmp);
  mprintf("Update P:\n");
  tmp.Print();

  // Testing Load
  geo.Load("ex1_geometry");
  mprintf("Size %i (%i)\n", geo.Size()[0], 128);
  mprintf("Length %f (%f)\n", geo.Length()[0], 1.0);
}

void test_parameter() {
  mprintf("Testing Parameter\n");

  // Test loading and parsing of params
  Parameter p = Parameter();
  p.Load("ex1_parameter");

  mprintf("Re %f (1000.0)\n", p.Re());
  mprintf("IterMax %d (100)\n", p.IterMax());
}

void test_interpolate() {
  mprintf("Testing Interpolate\n");
  mprintf("  Cycle Iterator visually by pressing 'Return' in Visu...\n");

  // Test interpolate
  const Geometry geo;
  
  // Create grid
  const real_t faktor = 0.5;
  const multi_real_t offset = {
    faktor * geo.Mesh()[0],
    faktor * geo.Mesh()[1]
  };
  Grid grid(&geo, offset);
  
  // Create Iterator
  Iterator it(&geo);
  
  // Create and initialize the visualization
  Renderer visu(geo.Length(), geo.Mesh());
  visu.Init(800, 800);
  
  visu.Render(&grid, 0.0, 1.0);
  
  for(it.First(); it.Valid(); it.Next()){
    // Set current cell
    grid.Cell(it) = real_t(1.0);
    
    // Visualize field and wait for user to press return
    int key = 0;
    
    visu.Render(&grid, 0.0, 1.0);
    
    while((key != 10)&&(key!=-1)){
      key = visu.Check();
    }
    
    // Abort iteration if visu is closed
    if(key==-1){
      mprintf("  ... aborted iteration since visu was closed!\n");
      return;
    }else{
      mprintf("  ... Iterate Next ...\n");
    }
    
    // Reset and countinue
    grid.Cell(it) = real_t(0.0);
  }
  
  mprintf("  ... finished!\n");
  
}

void test_grid() {
  mprintf("Testing Grid\n");

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

  mprintf("Interpolate: %f (%f)\n", grid.Interpolate({
    0.5 / (geo.Size()[0] - 2),
    0.5 / (geo.Size()[1] - 2)
  }), 1.5);

  // Test difference quotient of first order for the middle cell
  mprintf("Diff-Quot (1. order) middle\n");
  mprintf("%f (%f)\n", grid.dx_l(it), 1.0 / geo.Mesh()[0]);
  mprintf("%f (%f)\n", grid.dx_r(it), 1.0 / geo.Mesh()[0]);
  mprintf("%f (%f)\n", grid.dy_l(it), 2.0 / geo.Mesh()[1]);
  mprintf("%f (%f)\n", grid.dy_r(it), 1.0 / geo.Mesh()[1]);

  // Test difference quotient of second order for the middle cell
  mprintf("Diff-Quot (2. order) middle\n");
  mprintf("%f (%f)\n", grid.dxx(it), 0.0);
  mprintf("%f (%f)\n", grid.dyy(it), -1.0 / (geo.Mesh()[1] * geo.Mesh()[1]));

  // Test difference quotient of first order for a corner cell
  mprintf("Diff-Quot (1. order) corner\n");
  it = it.Down().Left();
  mprintf("%f (%f)\n", grid.dx_l(it), 0.0);
  mprintf("%f (%f)\n", grid.dx_r(it), 1.0 / geo.Mesh()[0]);
  mprintf("%f (%f)\n", grid.dy_l(it), 0.0);
  mprintf("%f (%f)\n", grid.dy_r(it), 2.0 / geo.Mesh()[1]);

  // Test difference quotient of second order for a corner cell
  mprintf("Diff-Quot (2. order) corner\n");
  mprintf("%f (%f)\n", grid.dxx(it), 1.0 / (geo.Mesh()[1] * geo.Mesh()[1]));
  mprintf("%f (%f)\n", grid.dyy(it), 2.0 / (geo.Mesh()[1] * geo.Mesh()[1]));

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
  mprintf("DC-Quot center\n");
  mprintf("%f (%f)\n", grid.DC_vdu_y(it, 0.5, &grid2), 30.5 / (4.0 * geo.Mesh()[1]));
  mprintf("%f (%f)\n", grid.DC_udu_x(it, 0.5), 23.0 / (4.0 * geo.Mesh()[1]));
  mprintf("%f (%f)\n", grid2.DC_udv_x(it, 0.5, &grid), -29.0 / (4.0 * geo.Mesh()[0]));
  mprintf("%f (%f)\n", grid2.DC_vdv_y(it, 0.5), 18.0 / (4.0 * geo.Mesh()[1]));
}

void test_load(const Parameter *param, const Geometry *geom){
  mprintf("Re: %f\n", param->Re());
  mprintf("Omega: %f\n", param->Omega());
  mprintf("Alpha: %f\n", param->Alpha());
  mprintf("Dt: %f\n", param->Dt());
  mprintf("Tend: %f\n", param->Tend());
  mprintf("IterMax: %d\n", param->IterMax());
  mprintf("Eps: %f\n", param->Eps());
  mprintf("Tau: %f\n", param->Tau());
  
  mprintf("Size: (%d, %d)\n", geom->Size()[0], geom->Size()[1]);
  mprintf("Len:  (%f, %f)\n", geom->Length()[0], geom->Length()[1]);
  mprintf("Mesh: (%f, %f)\n", geom->Mesh()[0], geom->Mesh()[1]);
}

void test_solver(const Geometry *geom){
  multi_index_t size = geom->Size();
  Grid *grid = new Grid(geom);
  
  InteriorIterator init(geom);
  
  for(init.First(); init.Valid(); init.Next()){
    grid->Cell(init) = std::max(0.0,-(init.Pos()[0] - 0.25 * size[0]) *
                                     (init.Pos()[0] - 0.75 * size[0]) -
                                     (init.Pos()[1] - 0.25 * size[1]) *
                                     (init.Pos()[1] - 0.75 * size[1]) );
  }
  
  // Create Right hand side
  Grid rhs(geom, 0.0);
  
  // Create and initialize the visualization
  Renderer visu(geom->Length(), geom->Mesh());
  visu.Init(800, 800);
  real_t maxGrid = grid->AbsMax();
  
  // Create solver
  RedOrBlackSOR *solver = new RedOrBlackSOR(geom,  real_t(1.7));
  
  // Plot grid
  visu.Render(grid, 0.0, maxGrid);
  
  int key = 0;
  int iter = 0;
  while((key != 10)&&(key!=-1)){
    key = visu.Check();
    
    real_t res = solver->RedCycle(grid, &rhs);
    mprintf("RedRes: %f\n", res);
    res += solver->BlackCycle(grid, &rhs);
    
    mprintf("Iter:    %d\n", iter);
    mprintf("Max val: %f (%f)\n", grid->Max(), maxGrid);
    mprintf("Min val: %f (%f)\n", grid->Min(), 0.0);
    mprintf("Res:     %f\n", res);
    
    visu.Render(grid, 0.0, maxGrid);
    
    iter++;
  }
  
  delete grid;
  delete solver;
}

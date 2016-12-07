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
  
  printf("BoundaryIterator Test Corner Bottom Left:\n");
  Iterator cbl = boit.CornerBottomLeft();
  cbl.printNeighbours();
  printf("\n");
  
  printf("BoundaryIterator Test Corner Bottom Right:\n");
  Iterator cbr = boit.CornerBottomRight();
  cbr.printNeighbours();
  printf("\n");
  
  printf("BoundaryIterator Test Corner Top Left:\n");
  Iterator ctl = boit.CornerTopLeft();
  ctl.printNeighbours();
  printf("\n");
  
  printf("BoundaryIterator Test Corner Top Right:\n");
  Iterator ctr = boit.CornerTopRight();
  ctr.printNeighbours();
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

void test_interpolate() {
  printf("Testing Interpolate\n");
  printf("  Cycle Iterator visually by pressing 'Return' in Visu...\n");

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
      printf("  ... aborted iteration since visu was closed!\n");
      return;
    }else{
      printf("  ... Iterate Next ...\n");
    }
    
    // Reset and countinue
    grid.Cell(it) = real_t(0.0);
  }
  
  printf("  ... finished!\n");
  
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
  printf("%f (%f)\n", grid.DC_vdu_y(it, 0.5, &grid2), 30.5 / (4.0 * geo.Mesh()[1]));
  printf("%f (%f)\n", grid.DC_udu_x(it, 0.5), 23.0 / (4.0 * geo.Mesh()[1]));
  printf("%f (%f)\n", grid2.DC_udv_x(it, 0.5, &grid), -29.0 / (4.0 * geo.Mesh()[0]));
  printf("%f (%f)\n", grid2.DC_vdv_y(it, 0.5), 18.0 / (4.0 * geo.Mesh()[1]));
}

void test_load(const Parameter *param, const Geometry *geom){
  printf("Re: %f\n", param->Re());
  printf("Omega: %f\n", param->Omega());
  printf("Alpha: %f\n", param->Alpha());
  printf("Dt: %f\n", param->Dt());
  printf("Tend: %f\n", param->Tend());
  printf("IterMax: %d\n", param->IterMax());
  printf("Eps: %f\n", param->Eps());
  printf("Tau: %f\n", param->Tau());
  
  printf("Size: (%d, %d)\n", geom->Size()[0], geom->Size()[1]);
  printf("Len:  (%f, %f)\n", geom->Length()[0], geom->Length()[1]);
  printf("Mesh: (%f, %f)\n", geom->Mesh()[0], geom->Mesh()[1]);
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
  Solver *solver = new SOR(geom,  real_t(1.7));
  
  // Plot grid
  visu.Render(grid, 0.0, maxGrid);
  
  int key = 0;
  int iter = 0;
  while((key != 10)&&(key!=-1)){
    key = visu.Check();
    
    real_t res = solver->Cycle(grid, &rhs);
    
    printf("Iter:    %d\n", iter);
    printf("Max val: %f (%f)\n", grid->Max(), maxGrid);
    printf("Min val: %f (%f)\n", grid->Min(), 0.0);
    printf("Res:     %f\n", res);
    
    visu.Render(grid, 0.0, maxGrid);
    
    iter++;
  }
  
  
  
  delete grid;
  delete solver;
}

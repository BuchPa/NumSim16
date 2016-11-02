#include "compute.hpp"

#include "geometry.hpp"
#include "grid.hpp"
#include "iterator.hpp"
#include "parameter.hpp"
#include "solver.hpp"

#include <cmath>

/// Creates a compute instance with given geometry and parameter
//  @param geom   Current geometry
//  @param param  Current parameters
Compute::Compute(const Geometry *geom, const Parameter *param)
    : _geom(geom), _param(param){
  
  // Calculate offsets
  multi_real_t offset_u;
  multi_real_t offset_v;
  multi_real_t offset_p;
  
  offset_u[0] = geom->Mesh()[0];
  offset_u[1] = geom->Mesh()[1]/2.0;
  
  offset_v[0] = geom->Mesh()[0]/2.0;
  offset_v[1] = geom->Mesh()[1];
  
  offset_p[0] = geom->Mesh()[0]/2.0;
  offset_p[1] = geom->Mesh()[1]/2.0;
  
  // Create and init velocity / pressure field
  _u = new Grid(geom, offset_u);
  _v = new Grid(geom, offset_v);
  _p = new Grid(geom, offset_p);
  
//   //TEST -->
//   Iterator it(_geom);
//   real_t i = 1;
//   while(it.Valid()){
//     _u->Cell(it) = i;
//     i++;
//     it.Next();
//   }
//   //TEST --^
  
  // Create and init other fields
  _F   = new Grid(geom);
  _G   = new Grid(geom);
  
  _rhs = new Grid(geom);
  
  _tmp = new Grid(geom);
  
  // Init time step
  _t = 0.0;
  
  //Init _solver
  _solver = new SOR(_geom,_param->Omega());
  //TODO Init _dtlimit
  //TODO Init _epslimit
}

/// Deletes all grids and solver
Compute::~Compute() {
  delete _u;
  delete _v;
  delete _p;
  
  delete _F;
  delete _G;
  
  delete _rhs;
  
  delete _tmp;
  
  delete _solver;
}

/// Returns the simulated time in total
const real_t &Compute::GetTime() const {
  return _t;
}

/// Returns the pointer to U
const Grid *Compute::GetU() const {
  return _u;
}

/// Returns the pointer to V
const Grid *Compute::GetV() const {
  return _v;
}

/// Returns the pointer to P
const Grid *Compute::GetP() const {
  return _p;
}

/// Returns the pointer to RHS
const Grid *Compute::GetRHS() const{
  return _rhs;
}

/// Computes and returns the absolute velocity (u_x + u_y)^(1/2)
const Grid *Compute::GetVelocity() {
  // Create Iterator
  Iterator it = Iterator(_geom);
  
  // Go through all cells and calculate absolute velocity (u_x^2 + u_y^2)^(1/2)
  while(it.Valid()){
    _tmp->Cell(it) = sqrt( pow(_u->Cell(it),2) + pow(_v->Cell(it),2) );
    it.Next();
  }
  return _tmp;
}

/// Computes and returns the vorticity
const Grid *Compute::GetVorticity(){
  // Not used so far. Return something.
  return _tmp;
}

/// Computes and returns the stream line values
const Grid *Compute::GetStream(){
  // Not used so far. Return something.
  return _tmp;
}

/// Execute one time step of the fluid simulation (with or without debug info)
// @ param printInfo print information about current solver state (residual etc.)
void Compute::TimeStep(bool printInfo) {
  _t += _param->Dt();
  if (printInfo) {
    printf("Time step: %4.2f\n", _t);
//     _u->Print();
  }
  //TODO
}

/***************************************************************************
 *                            PRIVATE FUNCTIONS                            *
 ***************************************************************************/

/// Compute the new velocites u,v
void Compute::NewVelocities(const real_t &dt){
  //TODO
}
/// Compute the temporary velocites F,G
void Compute::MomentumEqu(const real_t &dt){
  //TODO
}
/// Compute the RHS of the poisson equation
void Compute::RHS(const real_t &dt){
  //TODO
}
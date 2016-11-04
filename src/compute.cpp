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
  
  // Create and velocity / pressure field
  _u = new Grid(geom, offset_u);
  _v = new Grid(geom, offset_v);
  _p = new Grid(geom, offset_p);
  
  // Create other fields
  _F   = new Grid(geom);
  _G   = new Grid(geom);
  
  _rhs = new Grid(geom);
  
  _tmp = new Grid(geom);
  
  // Init velocity / pressure field
  _geom->Update_U(_u);
  _geom->Update_V(_v);
  _geom->Update_P(_p);
  
  // Init time
  _t = 0.0;
  
  // Compute cfl time step limitation
  _cfl = _param->Re() *     (pow(_geom->Mesh()[0],2.0) * pow(_geom->Mesh()[1],2.0))
                       /( 4*(pow(_geom->Mesh()[0],2.0) + pow(_geom->Mesh()[1],2.0)) );
  
  //Init _solver
  _solver = new SOR(_geom,_param->Omega());
  //TODO Init _dtlimit
  _dtlimit = _param->Dt();
  //TODO Init _epslimit
  _epslimit = _param->Eps();
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
    _tmp->Cell(it) = sqrt( pow(_u->Cell(it),2.0) + pow(_v->Cell(it),2.0) );
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
  
  // Compute candidates for current time step
  const real_t max_x = _geom->Mesh()[0] / _u->AbsMax();
  const real_t max_y = _geom->Mesh()[1] / _v->AbsMax();
  
  // Compute smallest time step from all candidates with some security factor
  // TODO _dtlimit noch einbauen
  const real_t dt    = _param->Tau() * std::min(std::min(max_x,max_y),_cfl);
  
  // Compute temporary velocites F,G
  this->MomentumEqu(dt);
  
  // Compute RHS
  this->RHS(dt);
  
  // Solve Poisson equation (-> p)
  index_t it(0);
  real_t  res(_epslimit + 0.1);
  while((it < _param->IterMax()) && (res >= _epslimit)){
    res = _solver->Cycle(_p, _rhs);
    it++;
    //TODO boundary values for p neccessary?
  }
  
  // Compute new velocites (-> u,v)
  this->NewVelocities(dt);
  
  // Set boundary values
  _geom->Update_U(_u);
  _geom->Update_V(_v);
  _geom->Update_P(_p);
  
  // Compute new time
  _t += dt;
  
  if (printInfo) {
    // Print current time
    printf("Current time: %4.2f\n", _t);
    
    // Print time step stuff
    printf("  Time step candidates:\n");
    printf("    x:   %4.3f\n", max_x);
    printf("    y:   %4.3f\n", max_y);
    printf("    cfl: %4.3f\n", _cfl);
    printf("  Current time step %4.3f\n", dt);
    printf("\n");
    
    // Solver stuff
    if( it >= _param->IterMax() ){
      printf("  DIDN'T converge! itermax reached!\n");
    }else{
      printf("  DID converge! eps reached after % d iterations!\n", it);
    }
    
//     // Print field stuff
//     printf("  Current field u:\n");
//     _u->Print();
//     printf("  Current field v:\n");
//     _v->Print();
//     printf("  Current field p:\n");
//     _p->Print();
    
  }
}

/***************************************************************************
 *                            PRIVATE FUNCTIONS                            *
 ***************************************************************************/

/// Compute the new velocites u,v
void Compute::NewVelocities(const real_t &dt){
  InteriorIterator init(_geom);
  
  // Cycle to compute F,G
  for(init.First(); init.Valid(); init.Next()){
    //TODO dx_r richtig hier?
    _u->Cell(init) = _F->Cell(init) - dt * _p->dx_r(init);
    _v->Cell(init) = _G->Cell(init) - dt * _p->dy_r(init);
  }
}
/// Compute the temporary velocites F,G
void Compute::MomentumEqu(const real_t &dt){
  InteriorIterator init(_geom);
  
  // Cycle to compute F,G
  for(init.First(); init.Valid(); init.Next()){
    _F->Cell(init) = _u->Cell(init) + dt * (_param->InvRe() * (_u->dxx(init) + _u->dyy(init))
                                            - 2.0 * _u->DC_udu_x(init, _param->Alpha())
                                            - _u->DC_vdu_y(init, _param->Alpha(), _v)
                                           );
    _G->Cell(init) = _v->Cell(init) + dt * (_param->InvRe() * (_v->dxx(init) + _v->dyy(init))
                                            - _v->DC_udv_x(init, _param->Alpha(), _u)
                                            - 2.0 * _v->DC_vdv_y(init, _param->Alpha())
                                           );
    //TODO 2.0 schon in DC_udu .. und DC_vdv ?
  }
  
  _geom->Update_U(_F);
  _geom->Update_V(_G);
}
/// Compute the RHS of the Poisson equation
void Compute::RHS(const real_t &dt){
  InteriorIterator init(_geom);
  
  // Cycle to compute rhs
  for(init.First(); init.Valid(); init.Next()){
    //TODO dx_l richtig hier?
    _rhs->Cell(init) = 1.0/dt * ( _F->dx_l(init) + _G->dy_l(init) );
  }
}
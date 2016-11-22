#include "compute.hpp"

#include "geometry.hpp"
#include "grid.hpp"
#include "iterator.hpp"
#include "parameter.hpp"
#include "solver.hpp"

#include <cmath>

using namespace std;

#define DYNAMIC_TIMESTEP true

Compute::Compute(const Geometry *geom, const Parameter *param, const Communicator* comm)
    : _geom(geom), _param(param), _comm(comm) {
  
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
  
  // Init _solver
  _solver = new SOR(_geom,_param->Omega());
  // Init _dtlimit
  _dtlimit = _param->Dt();
  // Init _epslimit
  _epslimit = _param->Eps();
}

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

const real_t &Compute::GetTime() const {
  return _t;
}

const Grid *Compute::GetU() const {
  return _u;
}

const Grid *Compute::GetV() const {
  return _v;
}

const Grid *Compute::GetP() const {
  return _p;
}

const Grid *Compute::GetRHS() const{
  return _rhs;
}

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

const Grid *Compute::GetVorticity() {
  Iterator it = Iterator(_geom);

  while (it.Valid()) {
    _tmp->Cell(it) = _u->dy_r(it) - _v->dx_r(it);
    it.Next();
  }

  return _tmp;
}

const Grid *Compute::GetStream() {
  Iterator it = Iterator(_geom);

  _tmp->Cell(it) = 0.0;
  it.Next();

  while (it < _geom->Size()[0]) {
    _tmp->Cell(it) = _tmp->Cell(it.Left()) + _u->dx_l(it);
    it.Next();
  }

  while (it.Valid()) {
    _tmp->Cell(it) = _tmp->Cell(it.Down()) + _u->dy_l(it);
    it.Next();
  }

  return _tmp;
}

void Compute::TimeStep(bool printInfo) {  
  // Compute candidates for current time step
  const real_t max_x = _geom->Mesh()[0] / _u->AbsMax();
  const real_t max_y = _geom->Mesh()[1] / _v->AbsMax();
  
  // Compute smallest time step from all candidates with some security factor
  // and a minimum timestep
  real_t dt;
  if (DYNAMIC_TIMESTEP) {
    dt = _param->Tau() * min(_dtlimit, min(min(max_x, max_y), _cfl));
  } else {
    dt = _dtlimit;
  }
  
  // Compute temporary velocites F,G
  this->MomentumEqu(dt);
  
  // Compute RHS
  this->RHS(dt);
  
  // Solve Poisson equation (-> p)
  index_t it(0);
  real_t  res(_epslimit + 0.1);
  while((it < _param->IterMax()) && (res >= _epslimit))  {
    res = _solver->Cycle(_p, _rhs);
    it++;
    // Set boundary values in each iter, because it changes with each iter
    _geom->Update_P(_p);
  }
  
  // Compute new velocites (-> u,v)
  this->NewVelocities(dt);
  
  // Set boundary values
  _geom->Update_U(_u);
  _geom->Update_V(_v);
  
  // Compute new time
  _t += dt;
  
  if (printInfo) {
    // Print current time
    printf("Current time: %4.2f\n", _t);
    
    // Print time step stuff
    printf("  Time step candidates:\n");
    printf("    x:       %4.3f\n", max_x);
    printf("    y:       %4.3f\n", max_y);
    printf("    cfl:     %4.3f\n", _cfl);
    printf("    dtlimit: %4.3f\n", _dtlimit);
    printf("  Current time step %4.3f\n", dt);
    printf("\n");
    
    // Solver stuff
    if( it >= _param->IterMax() ){
      printf("  DIDN'T converge! itermax reached!\n");
    } else {
      printf("  DID converge! eps (%f < %f) reached after % d iterations!\n", res, _epslimit, it);
    }
  }
}

/***************************************************************************
 *                            PRIVATE FUNCTIONS                            *
 ***************************************************************************/

void Compute::NewVelocities(const real_t &dt){
  InteriorIterator init(_geom);
  
  // Cycle to compute u,v
  for(init.First(); init.Valid(); init.Next()){
    _u->Cell(init) = _F->Cell(init) - dt * _p->dx_r(init);
    _v->Cell(init) = _G->Cell(init) - dt * _p->dy_r(init);
  }
}

void Compute::MomentumEqu(const real_t &dt){
  InteriorIterator init(_geom);
  
  // Cycle to compute F,G
  for(init.First(); init.Valid(); init.Next()){
    _F->Cell(init) = _u->Cell(init) + dt * (_param->InvRe() * (_u->dxx(init) + _u->dyy(init))
                                            - _u->DC_udu_x(init, _param->Alpha())
                                            - _u->DC_vdu_y(init, _param->Alpha(), _v)
                                           );
    _G->Cell(init) = _v->Cell(init) + dt * (_param->InvRe() * (_v->dxx(init) + _v->dyy(init))
                                            - _v->DC_udv_x(init, _param->Alpha(), _u)
                                            - _v->DC_vdv_y(init, _param->Alpha())
                                           );
  }
  
  _geom->Update_U(_F);
  _geom->Update_V(_G);
}

void Compute::RHS(const real_t &dt){
  InteriorIterator init(_geom);
  
  // Cycle to compute rhs
  for(init.First(); init.Valid(); init.Next()){
    _rhs->Cell(init) = 1.0/dt * ( _F->dx_l(init) + _G->dy_l(init) );
  }
}
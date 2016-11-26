#include "compute.hpp"
#include "comm.hpp"
#include "geometry.hpp"
#include "grid.hpp"
#include "iterator.hpp"
#include "parameter.hpp"
#include "solver.hpp"

#include <cmath>

using namespace std;

#define DYNAMIC_TIMESTEP true

Compute::Compute(const Geometry *geom, const Parameter *param, const Communicator *comm)
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
  
  // Create visu fields for stream lines and GetVorticity
  multi_real_t offset_visufields;
  
  offset_visufields[0] = geom->Mesh()[0];
  offset_visufields[1] = geom->Mesh()[1];
  
  _stream = new Grid(geom, offset_visufields);
  _vort   = new Grid(geom, offset_visufields);
  
  // Init velocity / pressure field
  _geom->Update_U(_u);
  _geom->Update_V(_v);
  _geom->Update_P(_p);
  
  // Init time
  _t = 0.0;
  
  // Compute cfl time step limitation
  _diff = _param->Re() *     (pow(_geom->Mesh()[0],2.0) * pow(_geom->Mesh()[1],2.0))
                        /( 4*(pow(_geom->Mesh()[0],2.0) + pow(_geom->Mesh()[1],2.0)) );
  
  // Init _solver
  _solver = new RedOrBlackSOR(_geom,_param->Omega());
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
  delete _stream;
  delete _vort;
  
  delete _solver;
}

const real_t &Compute::GetTime() const {
  return _t;
}

const Grid *Compute::GetU() const {
  _comm->collect(_fullgrid, _u);
  return _fullgrid;
}

const Grid *Compute::GetV() const {
  _comm->collect(_fullgrid, _v);
  return _fullgrid;
}

const Grid *Compute::GetP() const {
  _comm->collect(_fullgrid, _p);
  return _fullgrid;
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
    _vort->Cell(it) = _u->dy_r(it) - _v->dx_r(it);
    it.Next();
  }

  return _vort;
}

const Grid *Compute::GetStream() {
  Iterator it = Iterator(_geom);

  // Init first cell with a fixed value
  _stream->Cell(it) = 0.0;
  it.Next();

  // Calculate integral over first row in x-direction
  while (it < _geom->Size()[0]) {
    _stream->Cell(it) = _stream->Cell(it.Left()) - _geom->Mesh()[0] * _v->Cell(it);
    it.Next();
  }

  // Calculate integrals in y-direction
  while (it.Valid()) {
    _stream->Cell(it) = _stream->Cell(it.Down()) + _geom->Mesh()[1] * _u->Cell(it);
    it.Next();
  }

  return _stream;
}

void Compute::TimeStep(bool printInfo) {  
  // Compute candidates for current time step
  const real_t cfl_x = _geom->Mesh()[0] / _u->AbsMax();
  const real_t cfl_y = _geom->Mesh()[1] / _v->AbsMax();
  
  // Compute smallest time step from all candidates with some security factor
  // and a minimum timestep
  real_t dt;
  if (DYNAMIC_TIMESTEP) {
    dt = _param->Tau() * min(_dtlimit, min(min(cfl_x, cfl_y), _diff));
  } else {
    dt = _dtlimit;
  }
  
  // Communicate minimal timestep with other processes
  dt = _comm->gatherMin(dt);
  
  // Compute preliminary velocites F,G
  this->MomentumEqu(dt);
  
  // Communicate/exchange preliminary velocities at boundaries between subdomains
  _comm->copyBoundary(_F);
  _comm->copyBoundary(_G);
  
  // Compute RHS
  this->RHS(dt);
  
  // Solve Poisson equation (-> p)
  index_t it(0);
  real_t  res(_epslimit + 0.1);
  real_t  tmp_res(0.0);
  while((it < _param->IterMax()) && (res >= _epslimit))  {
    // First half iteration
    tmp_res = _solver->RedCycle(_p, _rhs);
    res     = max(res, tmp_res);
    
    // Communicate/exchange pressure values at boundaries between subdomain
    _comm->copyBoundary(_p);
    
    // Second half interation
    tmp_res = _solver->BlackCycle(_p, _rhs);
    res     = max(res, tmp_res);
    
    // Communicate/exchange pressure values at boundaries between subdomain
    _comm->copyBoundary(_p);
    
    // Communicate maximal residual with other processes
    res = _comm->gatherMax(res);
    
    it++;
    // Set boundary values in each iter, because it changes with each iter
    _geom->Update_P(_p);
  }
  
  // Compute new velocites (-> u,v)
  this->NewVelocities(dt);
  
  // Communicate/exchange final velocities at boundaries between subdomains
  _comm->copyBoundary(_u);
  _comm->copyBoundary(_v);
  
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
    printf("    cfl_x:   %4.3f\n", cfl_x);
    printf("    cfl_y:   %4.3f\n", cfl_y);
    printf("    diff:    %4.3f\n", _diff);
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

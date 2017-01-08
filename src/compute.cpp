#include "compute.hpp"

#include "geometry.hpp"
#include "grid.hpp"
#include "iterator.hpp"
#include "parameter.hpp"
#include "solver.hpp"

#include <cmath>

using namespace std;

#define DYNAMIC_TIMESTEP true

Compute::Compute(const Geometry *geom, const Parameter *param)
    : _geom(geom), _param(param) {
  
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
  _solver = new SOR(_geom,_param->Omega());
  // Init _dtlimit
  _dtlimit = _param->Dt();
  // Init _epslimit
  _epslimit = _param->Eps();
  // Init _dt_fixed
  _dt_fixed     = _param->FixedDt();
  _inv_dt_fixed = 1.0/_dt_fixed;
  
  // Init particles from data loaded in geometry
  particles_t streaklines = _geom->Streaklines();
  for (iter_particles_t it = streaklines.begin(); it != streaklines.end(); ++it) {
    particles_t newList;
    newList.push_back((*it));
    _streakline.push_back(newList);
  }
  particles_t traces = _geom->ParticleTraces();
  for (iter_particles_t it = traces.begin(); it != traces.end(); ++it) {
    particles_t newList;
    newList.push_back((*it));
    _trace.push_back(newList);
  }
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
    if (_geom->CellTypeAt(it) != CellType::Obstacle) {
      _stream->Cell(it) = _stream->Cell(it.Down()) + _geom->Mesh()[1] * _u->Cell(it);
    } else {
      _stream->Cell(it) = _stream->Cell(it.Down());
    }
    it.Next();
  }

  return _stream;
}

list<particles_t> *Compute::GetParticleTracing(){
  return &_trace;
}

list<particles_t> *Compute::GetStreaklines(){
  return &_streakline;
}

bool Compute::TimeStep(bool printInfo) {  
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
  
  // Correct timestep if necessary and decide, if print CSV is necessary
  bool print = false;
  uint32_t curstep = (int)(_t*_inv_dt_fixed);
  if (curstep < (int)((_t+dt)*_inv_dt_fixed)) {
    dt = (curstep+1)*_dt_fixed - _t;
    print = true;
  }
  
  // Compute preliminary velocites F,G
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
  
  // Update positions of particles for streaklines and particle tracing
  this->ComputeStreaklines(dt, printInfo);
  this->ComputeParticleTracing(dt, printInfo);
  
  // Compute new time
  _t += dt;
  
  return print;
  
//   if (printInfo) {
//     // Print current time
//     printf("Current time: %4.2f\n", _t);
//     
//     // Print time step stuff
//     printf("  Time step candidates:\n");
//     printf("    cfl_x:   %4.3f\n", cfl_x);
//     printf("    cfl_y:   %4.3f\n", cfl_y);
//     printf("    diff:    %4.3f\n", _diff);
//     printf("    dtlimit: %4.3f\n", _dtlimit);
//     printf("  Current time step %4.3f\n", dt);
//     printf("\n");
//     
//     // Solver stuff
//     if( it >= _param->IterMax() ){
//       printf("  DIDN'T converge! itermax reached!\n");
//     } else {
//       printf("  DID converge! eps (%f < %f) reached after % d iterations!\n", res, _epslimit, it);
//     }
//   }
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

void Compute::ComputeParticleStep(multi_real_t &particle, const real_t &dt){
  // Get velocities at particle coordinates
  real_t u = _u->Interpolate(particle);
  real_t v = _v->Interpolate(particle);
  
  // Move particle with velocites
  particle[0] = particle[0] + dt * u;
  particle[1] = particle[1] + dt * v;
}

void Compute::ComputeStreaklines(const real_t &dt, bool addOne){
  // Cycle the list of different streaklines
  for (list<particles_t>::iterator it_streak=_streakline.begin(); it_streak!=_streakline.end(); ++it_streak) {
    // Cycle the list of particles
    for (iter_particles_t it=it_streak->begin(); it!=it_streak->end(); ++it) {
      if (this->IsValidParticle(*it)) {
        this->ComputeParticleStep((*it), dt);
      }
    }
    
    // Add new item, if desired
    if (addOne){
      multi_real_t lastStep = *(--it_streak->end());
      
      if (!this->IsValidParticle(lastStep))
        continue;
      
      this->ComputeParticleStep(lastStep, dt);
      it_streak->push_back(lastStep);
    }
  }
}

void Compute::ComputeParticleTracing(const real_t &dt, bool addOne){
  // Cycle the list of different streaklines
  for (list<particles_t>::iterator it_trace=_trace.begin(); it_trace!=_trace.end(); ++it_trace) {
    // Get last known position of the particle to trace
    multi_real_t lastStep = *(--it_trace->end());
    
    if (!this->IsValidParticle(lastStep))
      continue;
    
    // Update position
    this->ComputeParticleStep(lastStep, dt);
    
    if (addOne) {
      // Save updated position
      it_trace->push_back(lastStep);
    } else {
      // Replace last element
      it_trace->erase(--it_trace->end());
      it_trace->push_back(lastStep);
    }
  }
}

bool Compute::IsValidParticle(multi_real_t &particle){
  if (
      (particle[0] < 0.0) ||
      (particle[0] > 1.001*_geom->Length()[0]) ||
      (particle[1] < 0.0) ||
      (particle[1] > 1.001*_geom->Length()[1])
     ){
    return false;
  }else{
    return true;
  }
}
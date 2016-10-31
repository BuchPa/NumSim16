#include "typedef.hpp"
#include "compute.hpp"

Compute::Compute(const Geometry *geom, const Parameter *param) {
  _geom  = geom;
  _param = param;
  
  // Init velocity and pressure field
  _u = geom.Update_U;
  _v = geom.Update_V;
  _p = geom.Update_P;
}

Compute::~Compute() {
}

const real_t &Compute::GetTime() const {
  return _t;
}

const Grid *Compute::GetVelocity() {
  return _u;
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

void Compute::TimeStep(bool printInfo) {

}
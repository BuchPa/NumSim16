#include "typedef.hpp"
#include "parameter.hpp"

Parameter::Parameter(){
  _re      = 1e3;
  _omega   = 1.7;
  _alpha   = 0.9;
  _eps     = 1e-3;
  _tau     = 0.5;
  _itermax = 1e2;
//   _dt      = ;
//   _tend    = ;
}

const real_t &Parameter::Tend() const {
  return _tend;
}

#include <cstdio>  // file methods
#include <cstring> // string
#include <cstdlib> // read/write

#include "typedef.hpp"
#include "parameter.hpp"

using namespace std;

/// Constructs a new Parameter set with default values
Parameter::Parameter(){
  // Driven Cavity parameters; see exercise sheet 1
  _re      = 1e3;
  _omega   = 1.7;
  _alpha   = 0.9;
  _eps     = 1e-3;
  _tau     = 0.5;
  _itermax = 1e2;
  _dt      = 0.1;
  _tend    = 10;
  
  // Compute inverse Re
  _invre   = 1.0/_re;
}

/// Loads the parameter values from a file
//  @param file File path as char array
void Parameter::Load(const char *file) {
  FILE* handle = fopen(file,"r");
  double inval;
  char name[20];
  while (!feof(handle)) {
    if (!fscanf(handle, "%s = %lf\n", name, &inval)) continue;
    if (strcmp(name,"re") == 0) _re = inval;
    else if (strcmp(name,"omg") == 0) _omega = inval;
    else if (strcmp(name,"alpha") == 0) _alpha = inval;
    else if (strcmp(name,"dt") == 0) _dt = inval;
    else if (strcmp(name,"tend") == 0) _tend = inval;
    else if (strcmp(name,"iter") == 0) _itermax = inval;
    else if (strcmp(name,"eps") == 0) _eps = inval;
    else if (strcmp(name,"tau") == 0) _tau = inval;
    else printf("Unknown parameter %s\n",name);
  }
  fclose(handle);
}

/// Getter functions for Re
const real_t &Parameter::Re() const{
  return _re;
}
/// Getter functions for InvRe
const real_t &Parameter::InvRe() const{
  return _invre;
}
/// Getter functions for Omega
const real_t &Parameter::Omega() const{
  return _omega;
}
/// Getter functions for Alpha
const real_t &Parameter::Alpha() const{
  return _alpha;
}
/// Getter functions for Dt
const real_t &Parameter::Dt() const{
  return _dt;
}
/// Getter functions for Tend
const real_t &Parameter::Tend() const{
  return _tend;
}
/// Getter functions for IterMax
const index_t &Parameter::IterMax() const{
  return _itermax;
}
/// Getter functions for Eps
const real_t &Parameter::Eps() const{
  return _eps;
}
/// Getter functions for Tau
const real_t &Parameter::Tau() const{
  return _tau;
}

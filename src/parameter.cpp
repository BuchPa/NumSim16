#include "iostream"
#include "fstream"
#include "string"
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
  //TODO Werte
  _dt      = 0.1; // Dummy value!
  _tend    = 10;  // Dummy value!
  
  // Compute inverse Re
  _invre   = 1.0/_re;
}

/// Loads the parameter values from a file
//  @param file File path as char array
void Parameter::Load(const char *file) {
  string line;
  ifstream ifile(file);
  
  if (ifile.is_open()) {
    while (getline (ifile, line)) {
      index_t pos = line.find("=");
      if (pos > 0) {
        string name = line.substr(0, pos);
        string val = line.substr(pos + 1);

        if (name.find("Re")) {
          _re = strtod(val.c_str(), NULL);
        } else if (name.find("Omega")) {
          _omega = strtod(val.c_str(), NULL);
        } else if (name.find("Alpha")) {
          _alpha = strtod(val.c_str(), NULL);
        } else if (name.find("Dt")) {
          _dt = strtod(val.c_str(), NULL);
        } else if (name.find("Tend")) {
          _tend = strtod(val.c_str(), NULL);
        } else if (name.find("IterMax")) {
          _itermax = strtol(val.c_str(), NULL, 0);
        } else if (name.find("Eps")) {
          _eps = strtod(val.c_str(), NULL);
        } else if (name.find("Tau")) {
          _tau = strtod(val.c_str(), NULL);
        }
      }
    }
    ifile.close();
  }
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

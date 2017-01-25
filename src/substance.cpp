#include "substance.hpp"
#include "typedef.hpp"
#include "geometry.hpp"
#include "grid.hpp"

#include <cstdio>  // file methods
#include <cstring> // string
#include <cstdlib> // read/write
#include <cmath>   // pow
#include <ctype.h>   // isdigit

Substance::Substance(const Geometry *geom) : _geom(geom){
  // Init c boundary values
  _concentration = 0.0;
}

Substance::~Substance() {
  for (index_t i=0; i<_n; ++i){
    delete _c[i];
    delete[] _r[i];
  }
  delete[] _c;
  delete[] _r;

  delete[] _l;
  delete[] _rt;
  delete[] _gamma;
  delete[] _d;
}

void Substance::DefaultInit() {
  _n        = index_t(1);

  _r        = new real_t*[_n];
  _r[0]     = new real_t[_n];
  _r[0][0]  = real_t(0.0001);

  _rt        = new real_t[_n];
  _rt[0]     = real_t(0.0);

  _gamma    = new real_t[_n];
  _gamma[0] = real_t(0.5);

  _d        = new real_t[_n];
  _d[0]     = real_t(0.001);

  _l        = new real_t[_n];
  _l[0]     = real_t(1.0);
  
  // Create single substance
  multi_real_t offset_c;
  offset_c[0] = _geom->Mesh()[0]/2.0;
  offset_c[1] = _geom->Mesh()[1]/2.0;
  
  _c    = new Grid*[_n];
  _c[0] = new Grid(_geom, offset_c);
  
  this->InitCircle(_c[0], multi_real_t({0.15, 0.6}), 0.01);
}

void Substance::Load(const char *file){
  FILE* handle = fopen(file, "r");

  double inval[2];
  char name[20];
  char line[1000]; // Todo: Fix finite line length or add documentation for it
  bool expected = true;

  while (!feof(handle)) {
    if (!fscanf(handle, "%s =", name)) continue;
    
    if (strcmp(name, "n") == 0) {
      if (fscanf(handle, " %lf\n", &inval[0])) {
        // Save new value for n
        _n = inval[0];
        
        // Create n new concentrations
        _gamma = new real_t[_n];
        _r     = new real_t*[_n];
        _rt    = new real_t[_n];
        _l     = new real_t[_n];
        _d     = new real_t[_n];
        _c     = new Grid*[_n];
        
        multi_real_t offset_c;
        offset_c[0] = _geom->Mesh()[0]/2.0;
        offset_c[1] = _geom->Mesh()[1]/2.0;
        
        for (index_t cc=0; cc<_n; ++cc){
          _c[cc] = new Grid(_geom, offset_c);
          _r[cc] = new real_t[_n];
        }
      }
      continue;
    }
    
    if (strcmp(name, "concentration") == 0) {
      if (fscanf(handle, " %lf\n", &inval[0])) {
        _concentration = inval[0];
      }
      continue;
    }
    
    if (strcmp(name, "d") == 0) {
      for (index_t cc=0; cc<_n; ++cc) {
        if (fscanf(handle, " %lf", &inval[0])) {
          _d[cc] = inval[0];
        }
      }
      expected = expected && fscanf(handle, "\n");
      continue;
    }

    if (strcmp(name, "r") == 0) {
      for (index_t self = 0; self < _n; self++) {
        for (index_t other = 0; other < _n; other++) {
          if (fscanf(handle, " %lf", &inval[0])) {
            _r[self][other] = inval[0];
          }
        }
      }
      expected = expected && fscanf(handle, "\n");
      continue;
    }

    if (strcmp(name, "l") == 0) {
      for (index_t cc=0; cc<_n; ++cc) {
        if (fscanf(handle, " %lf", &inval[0])) {
          _l[cc] = inval[0];
        }
      }
      expected = expected && fscanf(handle, "\n");
      continue;
    }

    if (strcmp(name, "gamma") == 0) {
      for (index_t cc=0; cc<_n; ++cc) {
        if (fscanf(handle, " %lf", &inval[0])) {
          _gamma[cc] = inval[0];
        }
      }
      expected = expected && fscanf(handle, "\n");
      continue;
    }

    // As soon as we read the "init = free" line, we assume the remaining
    // file content encodes the free concentrations
    if (strcmp(name, "init") == 0) {
      if (fscanf(handle, " %s\n", name)) {
        if (strcmp(name, "free") == 0) {
          index_t j = 0;
          int result = 0;
          
          multi_index_t size = _geom->Size();

          while (!feof(handle)) {
            result = fscanf(handle, "%s\n", line);

            if (!result) {
              continue;
            }

            for (index_t i = 0; i < size[0]; i++){
              index_t mask = 1;
              for (index_t cc=0; cc < _n; ++cc){
                if (!isdigit(line[i]))
                  std::runtime_error(std::string("Unvalid character in Suspension::Load detected: "+ std::to_string(line[0]) +". Suspension load only accepts digits in the init block\n"));
                if (mask & (int)(line[i] - '0'))
                  _c[cc]->Cell((size[1] - 1 - j) * (size[0]) + i) = 1.0;
                mask = mask << 1;
              }
            }

            j++;
          }
        } else if (strcmp(name, "circle") == 0) {
          for (index_t cc=0; cc < _n; ++cc)
            this->InitCircle(_c[0], multi_real_t({0.15, 0.6}), 0.01, 0.5);
        }
      }
    }
  }

  fclose(handle);
}

const Grid *Substance::GetC(const index_t n_subst) const{
  if (n_subst < _n) {
    return _c[n_subst];
  } else {
    return _c[0];
  }
}

real_t Substance::D(index_t idx) const {
  if (idx >= _n) {
    throw std::runtime_error(std::string("Unknown substance index: "+ std::to_string(idx)));
  } else {
    return _d[idx];
  }
}

const index_t &Substance::N() const{
  return _n;
}

void Substance::NewConcentrations(const real_t &dt, const Grid *u, const Grid *v) const{
  InteriorIterator init(_geom);

  // Cycle to compute c
  for (init.First(); init.Valid(); init.Next()) {
    if (_geom->CellTypeAt(init) == CellType::Fluid) {

      // Calculate inter-dependant reaction terms first, since they will change
      // during calculation
      for (index_t self=0; self < _n; self++) {
        _rt[self] = real_t(0.0);
        for (index_t other=0; other < _n; other++) {
          if (self != other) {
            _rt[self] += _r[self][other] * _c[self]->Cell(init) * _c[other]->Cell(init);
          }
        }
      }

      // Cycle concentrations
      for (index_t self=0; self<_n; self++) {
        _c[self]->Cell(init) =
          // previous value
          _c[self]->Cell(init)
          // diffusion term
          + _d[self] * dt * (_c[self]->dxx(init) + _c[self]->dyy(init))
          // x direction convection term
          - dt * _c[self]->DC_dCu_x(init, _gamma[self], u)
          // y direction convection term
          - dt * _c[self]->DC_dCv_y(init, _gamma[self], v)
          // quadratic reaction term (self-dependent only)
          + _r[self][self] * dt * _c[self]->Cell(init) * (_l[self] - _c[self]->Cell(init))/_l[self]
          // inter-dependent reaction terms (calculated above)
          + _rt[self] * dt;
      }
    }
  }
  
  // Apply boundary condition
  for (index_t cc=0; cc<_n; ++cc)
    this->Update_C(_c[cc]);
  
}

/***************************************************************************
 *                            PRIVATE FUNCTIONS                            *
 ***************************************************************************/

void Substance::Update_C(Grid *c) const{
  BoundaryIterator boit(_geom, 1);
  
  // Set left boundary
  boit.SetBoundary(4);
  this->CycleBoundary_C(c, boit);
  
  // Set right boundary
  boit.SetBoundary(2);
  this->CycleBoundary_C(c, boit);
  
  // Set lower boundary
  boit.SetBoundary(1);
  this->CycleBoundary_C(c, boit);

  // Set upper boundary
  boit.SetBoundary(3);
  this->CycleBoundary_C(c, boit);
  
  // Set corners to avg of neighbour cells
  Iterator cbl = boit.CornerBottomLeft();
  c->Cell(cbl) = (c->Cell(cbl.Right()) + c->Cell(cbl.Top()))/2.0;
  
  Iterator cbr = boit.CornerBottomRight();
  c->Cell(cbr) = (c->Cell(cbr.Left()) + c->Cell(cbr.Top()))/2.0;
  
  Iterator ctl = boit.CornerTopLeft();
  c->Cell(ctl) = (c->Cell(ctl.Right()) + c->Cell(ctl.Down()))/2.0;
  
  Iterator ctr = boit.CornerTopRight();
  c->Cell(ctr) = (c->Cell(ctr.Left()) + c->Cell(ctr.Down()))/2.0; 

  ObstacleIterator oit = ObstacleIterator(_geom);

  for(; oit.Valid(); oit.Next()) {
    switch (_geom->BakedNeighbors(oit)) {
      case 13:
        c->Cell(oit) = -c->Cell(oit.Top());
        break;

      case 11:
        c->Cell(oit) = -c->Cell(oit.Right());
        break;

      case 7:
        c->Cell(oit) = -c->Cell(oit.Down());
        break;

      case 14:
        c->Cell(oit) = -c->Cell(oit.Left());
        break;

      case 3:
        c->Cell(oit) = -0.5 * (c->Cell(oit.Right()) + c->Cell(oit.Down()));
        break;

      case 9:
        c->Cell(oit) = -0.5 * (c->Cell(oit.Right()) + c->Cell(oit.Top()));
        break;

      case 12:
        c->Cell(oit) = -0.5 * (c->Cell(oit.Left()) + c->Cell(oit.Top()));
        break;

      case 6:
        c->Cell(oit) = -0.5 * (c->Cell(oit.Left()) + c->Cell(oit.Down()));
        break;
    }
  }
}

void Substance::CycleBoundary_C(Grid *c, BoundaryIterator boit) const{
  for (; boit.Valid(); boit.Next())
    switch(_geom->CellTypeAt(boit)){

      // In the following, we can reuse the pressure methods, since they work
      // the same for the concentration
      case CellType::Obstacle:
        this->SetCDirichlet(c, boit, 0.0);
        break;
        
      case CellType::Inflow:
        this->SetCNeumann(c, boit, 0.0);
        break;
        
      case CellType::H_Inflow:
        throw std::runtime_error(std::string("Not implemented!"));
        break;
        
      case CellType::V_Inflow:
        this->SetCNeumann(c, boit, 0.0);
        break;
        
      case CellType::Outflow:
        this->SetCNeumann(c, boit, 0.0);
        break;
        
      case CellType::V_Slip:
        this->SetCNeumann(c, boit, 0.0);
        break;
        
      case CellType::H_Slip:
        this->SetCNeumann(c, boit, 0.0);
        break;
        
      default:
        throw std::runtime_error(std::string("Unknown boundary condition: "+ std::to_string(_geom->CellTypeAt(boit))));
        break;
    }
}

void Substance::SetCDirichlet(Grid *c, const BoundaryIterator &boit, const real_t &value) const{
  switch(boit.Boundary()){
    // Set lower boundary
    case 1:
      c->Cell(boit) = 2 * value - c->Cell(boit.Top());
      break;
      
    // Set right boundary
    case 2:
      c->Cell(boit) = 2 * value - c->Cell(boit.Left());
      break;
      
    // Set upper boundary
    case 3:
      c->Cell(boit) = 2 * value - c->Cell(boit.Down());
      break;
      
    // Set left boundary
    case 4:
      c->Cell(boit) = 2 * value - c->Cell(boit.Right());
      break;
      
    default:
      throw std::runtime_error(std::string("Failed to operate with given boundary value: "+ std::to_string(boit.Boundary())));
      break;
  }
}

void Substance::SetCNeumann(Grid *c, const BoundaryIterator &boit, const real_t &value) const{
  switch(boit.Boundary()){
    // Set lower boundary
    case 1:
      c->Cell(boit) = c->Cell(boit.Top()) - _geom->Mesh()[1] * value;
      break;
      
    // Set right boundary
    case 2:
      c->Cell(boit) = c->Cell(boit.Left()) + _geom->Mesh()[0] * value;
      break;
      
    // Set upper boundary
    case 3:
      c->Cell(boit) = c->Cell(boit.Down()) + _geom->Mesh()[1] * value;
      break;
      
    // Set left boundary
    case 4:
      c->Cell(boit) = c->Cell(boit.Right()) - _geom->Mesh()[0] * value;
      break;
      
    default:
      throw std::runtime_error(std::string("Failed to operate with given boundary value: "+ std::to_string(boit.Boundary())));
      break;
  }
}

void Substance::InitCircle(Grid *c, const multi_real_t center, const real_t radius, const real_t val) const {
  InteriorIterator it(_geom);
  real_t x,y;
  for (;it.Valid(); it.Next()) {
    if (_geom->CellTypeAt(it) == CellType::Fluid){
      x = it.Pos()[0] * _geom->Mesh()[0] - center[0] * _geom->Length()[0];
      y = it.Pos()[1] * _geom->Mesh()[1] - center[1] * _geom->Length()[1];
      c->Cell(it) = pow(x*x + y*y, 0.5) > radius * _geom->Length()[1] ? 0.0 : val;
    }
  }

  this->Update_C(c); // Setting boundary values
}

#include "typedef.hpp"
#include "geometry.hpp"

#include "grid.hpp"

#include <cstdio>  // file methods
#include <cstring> // string
#include <cstdlib> // read/write

Geometry::Geometry(){
  // Init number of INNER cells in each dimension
  _size[0] = 8;
  _size[1] = 8;
  
  // Init geom data field
  _cells = new char[_size[0] * _size[1]];

  // Init length of driven cavity
  _length[0] = 1.0;
  _length[1] = 1.0;
  
  // Init u/v boundary values and set u for upper boundary to 1 (as default)
  _velocity[0] = 1.0;
  _velocity[1] = 0.0;
  
  // Init p boundary values
  _pressure = 0.0;
  
  this->Recalculate();
}

Geometry::~Geometry() {
  delete[] _cells;
}

void Geometry::Load(const char *file){
  FILE* handle = fopen(file, "r");

  double inval[2];
  char name[20];
  char line[1000]; // Todo: Fix finite line length or add documentation for it

  while (!feof(handle)) {
    if (!fscanf(handle, "%s =", name)) continue;

    if (strcmp(name, "size") == 0) {
      if (fscanf(handle, " %lf %lf\n", &inval[0], &inval[1])) {
        _size[0] = inval[0];
        _size[1] = inval[1];

        // Resize cell field
        delete[] _cells;
        _cells = new char[(_size[0]+2) * (_size[1]+2)];
      }
      continue;
    }

    if (strcmp(name, "length") == 0) {
      if (fscanf(handle, " %lf %lf\n", &inval[0], &inval[1])) {
        _length[0] = inval[0];
        _length[1] = inval[1];
      }
      continue;
    }

    if (strcmp(name, "velocity") == 0) {
      if (fscanf(handle, " %lf %lf\n", &inval[0], &inval[1])) {
        _velocity[0] = inval[0];
        _velocity[1] = inval[1];
      }
      continue;
    }

    if (strcmp(name, "pressure") == 0) {
      if (fscanf(handle, " %lf\n", &inval[0])) {
        _pressure = inval[0];
      }
      continue;
    }

    if (strcmp(name, "trace") == 0) {
      if (fscanf(handle, " %lf %lf\n", &inval[0], &inval[1])) {
        _trace.push_back(multi_real_t({inval[0], inval[1]}));
      }
      continue;
    }
    
    if (strcmp(name, "streakline") == 0) {
      if (fscanf(handle, " %lf %lf\n", &inval[0], &inval[1])) {
        _streakline.push_back(multi_real_t({inval[0], inval[1]}));
      }
      continue;
    }

    // As soon as we read the "geometry = free" line, we assume the remaining
    // file content encodes the free geometry
    if (strcmp(name, "geometry") == 0) {
      if (fscanf(handle, " %s\n", name)) {
        index_t j = 0;
        int result = 0;

        while (!feof(handle)) {
          result = fscanf(handle, "%s\n", line);

          if (!result) {
            continue;
          }

          for (index_t i = 0; i < _size[0]+2; i++) {

            switch (line[i]) {
              case CellType::Fluid:
              case CellType::Obstacle:
              case CellType::Inflow:
              case CellType::H_Inflow:
              case CellType::V_Inflow:
              case CellType::Outflow:
              case CellType::V_Slip:
              case CellType::H_Slip:
                _cells[j * (_size[0]+2) + i] = line[i];
                break;

              default:
                break;
            }
          }

          j++;
        }
      }
    }
  }

  fclose(handle);

  this->Recalculate();
}

void Geometry::Recalculate() {
  // Calculate cell width/height
  _h[0] = _length[0]/_size[0];
  _h[1] = _length[1]/_size[1];
  
  // Calculate inverse mesh width/height
  _invh[0] = _size[0]/_length[0];
  _invh[1] = _size[1]/_length[1];
  
  // Set _size to size INCL ghost layers
  _size[0] += 2;
  _size[1] += 2;
}

const multi_index_t &Geometry::Size() const {
  return _size;
}

const multi_real_t &Geometry::Mesh() const {
  return _h;
}

const multi_real_t &Geometry::Length() const{
  return _length;
}

const multi_real_t &Geometry::InvMesh() const{
  return _invh;
}

particles_t Geometry::ParticleTraces() const{
  return _trace;
}
  
particles_t Geometry::Streaklines() const{
  return _streakline;
}

void Geometry::Update_U(Grid *u) const{
  BoundaryIterator boit(this, 1);
  
  // Set lower boundary
  boit.SetBoundary(1);
  this->CycleBoundary_U(u, boit);
  
  // Set right boundary
  boit.SetBoundary(2);
  this->CycleBoundary_U(u, boit);
    
  // Set upper boundary
  boit.SetBoundary(3);
  this->CycleBoundary_U(u, boit);
  
  // Set left boundary
  boit.SetBoundary(4);
  this->CycleBoundary_U(u, boit);
}

void Geometry::CycleBoundary_U(Grid *u, BoundaryIterator boit) const{
  for (; boit.Valid(); boit.Next())
    switch(this->CellTypeAt(boit)){
      case CellType::Obstacle:
        this->SetUDirichlet(u, boit, 0.0);
        break;
        
      case CellType::Inflow:
        this->SetUDirichlet(u, boit, _velocity[0]);
        break;
        
      case CellType::H_Inflow:
        throw std::runtime_error(std::string("Not implemented!"));
        break;
        
      case CellType::V_Inflow:
        throw std::runtime_error(std::string("Not implemented!"));
        break;
        
      case CellType::Outflow:
        this->SetUNeumann(u, boit, 0.0);
        break;
        
      case CellType::V_Slip:
        if ((boit.Boundary() == 2) || (boit.Boundary() == 4)){
          this->SetUNeumann(u, boit, 0.0);
        }else{
          throw std::runtime_error(std::string("Vertical slip condition is not allowed on the lower/upper boundary"));
        }
        break;
        
      case CellType::H_Slip:
        if ((boit.Boundary() == 1) || (boit.Boundary() == 3)){
          this->SetUDirichlet(u, boit, 0.0);
        }else{
          throw std::runtime_error(std::string("Horizontal slip condition is not allowed on the left/right boundary"));
        }
        break;
        
      default:
        throw std::runtime_error(std::string("Unknown boundary condition: "+ std::to_string(this->CellTypeAt(boit))));
        break;
    }
}

void Geometry::Update_V(Grid *v) const{
  BoundaryIterator boit(this, 1);
  
  // Set lower boundary
  boit.SetBoundary(1);
  this->CycleBoundary_V(v, boit);
  
  // Set right boundary
  boit.SetBoundary(2);
  this->CycleBoundary_V(v, boit);
    
  // Set upper boundary
  boit.SetBoundary(3);
  this->CycleBoundary_V(v, boit);
  
  // Set left boundary
  boit.SetBoundary(4);
  this->CycleBoundary_V(v, boit);
}

void Geometry::CycleBoundary_V(Grid *v, BoundaryIterator boit) const{
  for (; boit.Valid(); boit.Next())
    switch(this->CellTypeAt(boit)){
      case CellType::Obstacle:
        this->SetVDirichlet(v, boit, 0.0);
        break;
        
      case CellType::Inflow:
        this->SetVDirichlet(v, boit, _velocity[1]);
        break;
        
      case CellType::H_Inflow:
        throw std::runtime_error(std::string("Not implemented!"));
        break;
        
      case CellType::V_Inflow:
        throw std::runtime_error(std::string("Not implemented!"));
        break;
        
      case CellType::Outflow:
        this->SetVNeumann(v, boit, 0.0);
        break;
        
      case CellType::V_Slip:
        if ((boit.Boundary() == 2) || (boit.Boundary() == 4)){
          this->SetVDirichlet(v, boit, 0.0);
        }else{
          throw std::runtime_error(std::string("Vertical slip condition is not allowed on the lower/upper boundary"));
        }
        break;
        
      case CellType::H_Slip:
        if ((boit.Boundary() == 1) || (boit.Boundary() == 3)){
          this->SetVNeumann(v, boit, 0.0);
        }else{
          throw std::runtime_error(std::string("Horizontal slip condition is not allowed on the left/right boundary"));
        }
        break;
        
      default:
        throw std::runtime_error(std::string("Unknown boundary condition: "+ std::to_string(this->CellTypeAt(boit))));
        break;
    }
}

void Geometry::Update_P(Grid *p) const{
  BoundaryIterator boit(this, 1);
  
  // Set lower boundary
  boit.SetBoundary(1);
  this->CycleBoundary_P(p, boit);
  
  // Set right boundary
  boit.SetBoundary(2);
  this->CycleBoundary_P(p, boit);
    
  // Set upper boundary
  boit.SetBoundary(3);
  this->CycleBoundary_P(p, boit);
  
  // Set left boundary
  boit.SetBoundary(4);
  this->CycleBoundary_P(p, boit);
  
  
  // Set corners to avg of neighbour cells
  Iterator cbl = boit.CornerBottomLeft();
  p->Cell(cbl) = (p->Cell(cbl.Right()) + p->Cell(cbl.Top()))/2.0;
  
  Iterator cbr = boit.CornerBottomRight();
  p->Cell(cbr) = (p->Cell(cbr.Left()) + p->Cell(cbr.Top()))/2.0;
  
  Iterator ctl = boit.CornerTopLeft();
  p->Cell(ctl) = (p->Cell(ctl.Right()) + p->Cell(ctl.Down()))/2.0;
  
  Iterator ctr = boit.CornerTopRight();
  p->Cell(ctr) = (p->Cell(ctr.Left()) + p->Cell(ctr.Down()))/2.0; 
}

void Geometry::CycleBoundary_P(Grid *p, BoundaryIterator boit) const{
  for (; boit.Valid(); boit.Next())
    switch(this->CellTypeAt(boit)){
      case CellType::Obstacle:
        this->SetPNeumann(p, boit, 0.0);
        break;
        
      case CellType::Inflow:
        this->SetPNeumann(p, boit, 0.0);
        break;
        
      case CellType::H_Inflow:
        throw std::runtime_error(std::string("Not implemented!"));
        break;
        
      case CellType::V_Inflow:
        throw std::runtime_error(std::string("Not implemented!"));
        break;
        
      case CellType::Outflow:
        this->SetPDirichlet(p, boit, 0.0);
        break;
        
      case CellType::V_Slip:
        this->SetPDirichlet(p, boit, _pressure);
        break;
        
      case CellType::H_Slip:
        this->SetPDirichlet(p, boit, _pressure);
        break;
        
      default:
        throw std::runtime_error(std::string("Unknown boundary condition: "+ std::to_string(this->CellTypeAt(boit))));
        break;
    }
}

void Geometry::SetUDirichlet(Grid *u, const BoundaryIterator &boit, const real_t &value) const{
  switch(boit.Boundary()){
    // Set lower boundary
    case 1:
      u->Cell(boit) = 2 * value - u->Cell(boit.Top());
      break;
      
    // Set right boundary
    case 2:
      u->Cell(boit)        = value;
      u->Cell(boit.Left()) = value;
      break;
      
    // Set upper boundary
    case 3:
      u->Cell(boit) = 2 * value - u->Cell(boit.Down());
      break;
      
    // Set left boundary
    case 4:
      u->Cell(boit) = value;
      break;
      
    default:
      throw std::runtime_error(std::string("Failed to operate with given boundary value: "+ std::to_string(boit.Boundary())));
      break;
  }
}

void Geometry::SetUNeumann(Grid *u, const BoundaryIterator &boit, const real_t &value) const{
  switch(boit.Boundary()){
    // Set lower boundary
    case 1:
      u->Cell(boit) = u->Cell(boit.Top()) - _h[1] * value;
      break;
      
    // Set right boundary
    case 2:
      u->Cell(boit.Left()) = u->Cell(boit.Left().Left()) + _h[0] * value;
      u->Cell(boit)        = u->Cell(boit.Left());
      break;
      
    // Set upper boundary
    case 3:
      u->Cell(boit) = u->Cell(boit.Down()) + _h[1] * value;
      break;
      
    // Set left boundary
    case 4:
      u->Cell(boit) = u->Cell(boit.Right()) - _h[0] * value;
      break;
      
    default:
      throw std::runtime_error(std::string("Failed to operate with given boundary value: "+ std::to_string(boit.Boundary())));
      break;
  }
}
  

void Geometry::SetVDirichlet(Grid *v, const BoundaryIterator &boit, const real_t &value) const{
  switch(boit.Boundary()){
    // Set lower boundary
    case 1:
      v->Cell(boit) = value;
      break;
      
    // Set right boundary
    case 2:
      v->Cell(boit) = 2 * value - v->Cell(boit.Left());
      break;
      
    // Set upper boundary
    case 3:
      v->Cell(boit) = value;
      v->Cell(boit.Down()) = value;
      break;
      
    // Set left boundary
    case 4:
      v->Cell(boit) = 2 * value - v->Cell(boit.Right());
      break;
      
    default:
      throw std::runtime_error(std::string("Failed to operate with given boundary value: "+ std::to_string(boit.Boundary())));
      break;
  }
}

void Geometry::SetVNeumann(Grid *v, const BoundaryIterator &boit, const real_t &value) const{
  switch(boit.Boundary()){
    // Set lower boundary
    case 1:
      v->Cell(boit) = v->Cell(boit.Top()) - _h[1] * value;
      break;
      
    // Set right boundary
    case 2:
      v->Cell(boit) = v->Cell(boit.Left()) + _h[0] * value;
      break;
      
    // Set upper boundary
    case 3:
      v->Cell(boit.Down()) = v->Cell(boit.Down().Down()) + _h[1] * value;
      v->Cell(boit)        = v->Cell(boit.Down());
      break;
      
    // Set left boundary
    case 4:
      v->Cell(boit) = v->Cell(boit.Right()) - _h[0] * value;
      break;
      
    default:
      throw std::runtime_error(std::string("Failed to operate with given boundary value: "+ std::to_string(boit.Boundary())));
      break;
  }
}

void Geometry::SetPDirichlet(Grid *p, const BoundaryIterator &boit, const real_t &value) const{
  switch(boit.Boundary()){
    // Set lower boundary
    case 1:
      p->Cell(boit) = 2 * value - p->Cell(boit.Top());
      break;
      
    // Set right boundary
    case 2:
      p->Cell(boit) = 2 * value - p->Cell(boit.Left());
      break;
      
    // Set upper boundary
    case 3:
      p->Cell(boit) = 2 * value - p->Cell(boit.Down());
      break;
      
    // Set left boundary
    case 4:
      p->Cell(boit) = 2 * value - p->Cell(boit.Right());
      break;
      
    default:
      throw std::runtime_error(std::string("Failed to operate with given boundary value: "+ std::to_string(boit.Boundary())));
      break;
  }
}

void Geometry::SetPNeumann(Grid *p, const BoundaryIterator &boit, const real_t &value) const{
  switch(boit.Boundary()){
    // Set lower boundary
    case 1:
      p->Cell(boit) = p->Cell(boit.Top()) - _h[1] * value;
      break;
      
    // Set right boundary
    case 2:
      p->Cell(boit) = p->Cell(boit.Left()) + _h[0] * value;
      break;
      
    // Set upper boundary
    case 3:
      p->Cell(boit) = p->Cell(boit.Down()) + _h[1] * value;
      break;
      
    // Set left boundary
    case 4:
      p->Cell(boit) = p->Cell(boit.Right()) - _h[0] * value;
      break;
      
    default:
      throw std::runtime_error(std::string("Failed to operate with given boundary value: "+ std::to_string(boit.Boundary())));
      break;
  }
}

char Geometry::CellTypeAt(index_t pos) const {
  return _cells[pos];
}

char Geometry::CellTypeAt(index_t xpos, index_t ypos) const {
  return _cells[ypos * _size[0] + xpos];
}
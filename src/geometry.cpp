#include "typedef.hpp"
#include "geometry.hpp"

#include "iterator.hpp"
#include "grid.hpp"

#include <cstdio>  // file methods
#include <cstring> // string
#include <cstdlib> // read/write

Geometry::Geometry(){
  // Init number of INNER cells in each dimension
  _size[0] = 8;
  _size[1] = 8;
  
  // Init length of driven cavity
  _length[0] = 1.0;
  _length[1] = 1.0;
  
  this->CalculateMesh();
  
  // Set boundary values for upper boundary (u=1, v=0)
  _velocity[0] = real_t(1.0);
  _velocity[1] = real_t(0.0);
  
  // Unused in driven cavity problem
  _pressure    = real_t(0.0);
}

Geometry::Geometry (const Communicator* comm)
  : _comm(comm){
  //TODO
}

void Geometry::Load(const char *file){
  FILE* handle = fopen(file, "r");

  double inval[2];
  char name[20];

  while (!feof(handle)) {
    if (!fscanf(handle, "%s =", name)) continue;

    if (strcmp(name, "size") == 0) {
      if (fscanf(handle, " %lf %lf\n", &inval[0], &inval[1])) {
        _size[0] = inval[0];
        _size[1] = inval[1];
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
      if (fscanf(handle, " %lf\n", &inval[0]))
        _pressure = inval[0];
      continue;
    }
  }
  fclose(handle);
  
  this->CalculateMesh();
}

void Geometry::CalculateMesh() {
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

const multi_index_t &Geometry::TotalSize() const{
  //TODO
  return _bsize;
}

const multi_real_t &Geometry::Mesh() const {
  return _h;
}

const multi_real_t &Geometry::Length() const{
  return _length;
}

const multi_real_t &Geometry::TotalLength() const{
  //TODO
  return _blength;
}

const multi_real_t &Geometry::InvMesh() const{
  return _invh;
}

void Geometry::Update_U(Grid *u) const{
  BoundaryIterator boit(this, 1);
  
  // Set lower boundary - zero dirichlet
  boit.SetBoundary(1);
  while(boit.Valid()){
    u->Cell(boit) = -1 * u->Cell(boit.Top());
    boit.Next();
  }
  
  // Set right boundary - zero dirichlet
  boit.SetBoundary(2);
  while(boit.Valid()){
    u->Cell(boit)        = real_t(0.0);
    u->Cell(boit.Left()) = real_t(0.0);
    boit.Next();
  }
  
  // Set left boundary - zero dirichlet
  boit.SetBoundary(4);
  while(boit.Valid()){
    u->Cell(boit) = real_t(0.0);
    boit.Next();
  }
  
  // Set upper boundary - non-zero dirichlet
  boit.SetBoundary(3);
  while(boit.Valid()){
    u->Cell(boit) = 2* _velocity[0] - u->Cell(boit.Down());
    boit.Next();
  }
  
}

void Geometry::Update_V(Grid *v) const{
  BoundaryIterator boit(this, 1);
  
  // Set right boundary - zero dirichlet
  boit.SetBoundary(2);
  while(boit.Valid()){
    v->Cell(boit) = -1 * v->Cell(boit.Left());
    boit.Next();
  }
  
  // Set left boundary - zero dirichlet
  boit.SetBoundary(4);
  while(boit.Valid()){
    v->Cell(boit) = -1 * v->Cell(boit.Right());
    boit.Next();
  }
  
  // Set lower boundary - zero dirichlet
  boit.SetBoundary(1);
  while(boit.Valid()){
    v->Cell(boit) = real_t(0.0);
    boit.Next();
  }
  
  // Set upper boundary - zero dirichlet
  boit.SetBoundary(3);
  while(boit.Valid()){
    v->Cell(boit)        = _velocity[1];
    v->Cell(boit.Down()) = _velocity[1];
    boit.Next();
  }
}

void Geometry::Update_P(Grid *p) const{
  BoundaryIterator boit(this, 1);
  
  // Set right boundary
  boit.SetBoundary(2);
  while(boit.Valid()){
    p->Cell(boit) = p->Cell(boit.Left());
    boit.Next();
  }
  
  // Set left boundary
  boit.SetBoundary(4);
  while(boit.Valid()){
    p->Cell(boit) = p->Cell(boit.Right());
    boit.Next();
  }
  
  // Set lower boundary
  boit.SetBoundary(1);
  while(boit.Valid()){
    p->Cell(boit) = p->Cell(boit.Top());
    boit.Next();
  }
  
  // Set upper boundary
  boit.SetBoundary(3);
  while(boit.Valid()){
    p->Cell(boit) = p->Cell(boit.Down());
    boit.Next();
  }
  
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
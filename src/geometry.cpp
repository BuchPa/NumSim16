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
  
  // Init geom data field
  _cells = new char[_size[0] * _size[1]];

  // Init length of driven cavity
  _length[0] = 1.0;
  _length[1] = 1.0;
  
  // Init u/v boundary values and set u for upper boundary to 1 (as default)
  for (int i = 0; i < 8; i++) {
    _velocity[i] = real_t(0.0);
    _vtype[i] = 'd';
  }
  _velocity[4] = real_t(1.0);
  
  // Init p boundary values
  for (int i = 0; i < 4; i++) {
    _pressure[i] = real_t(0.0);
    _ptype[i] = 'd';
  }
  
  this->Recalculate();
}

Geometry::~Geometry() {
  delete[] _cells;
}

void Geometry::Load(const char *file){
  FILE* handle = fopen(file, "r");

  double inval[8];
  char cinval[8];
  char name[20];

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
      if (
        fscanf(
          handle, " %lf %lf %lf %lf %lf %lf %lf %lf\n",
          &inval[0], &inval[1], &inval[2], &inval[3],
          &inval[4], &inval[5], &inval[6], &inval[7]
        )
      ) {
        for (int i = 0; i < 8; i++) {
          _velocity[i] = inval[i];
        }
      }
      continue;
    }

    if (strcmp(name, "pressure") == 0) {
      if (
        fscanf(
          handle, " %lf %lf %lf %lf\n",
          &inval[0], &inval[1], &inval[2], &inval[3]
        )
      ) {
        for (int i = 0; i < 4; i++) {
          _pressure[i] = inval[i];
        }
      }
      continue;
    }

    if (strcmp(name, "v_type") == 0) {
      if (
        fscanf(
          handle, " %c %c %c %c %c %c %c %c\n",
          &cinval[0], &cinval[1], &cinval[2], &cinval[3],
          &cinval[4], &cinval[5], &cinval[6], &cinval[7]
        )
      ) {
        for (int i = 0; i < 8; i++) {
          _vtype[i] = cinval[i];
        }
      }
      continue;
    }

    if (strcmp(name, "p_type") == 0) {
      if (
        fscanf(
          handle, " %c %c %c %c\n",
          &cinval[0], &cinval[1], &cinval[2], &cinval[3]
        )
      ) {
        for (int i = 0; i < 4; i++) {
          _vtype[i] = cinval[i];
        }
      }
      continue;
    }

    // None of the cases above matched, so we've probably hit the area declaring cell types
    // for the grid

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

void Geometry::Update_U(Grid *u) const{
  BoundaryIterator boit(this, 1);
  
  // Set lower boundary
  boit.SetBoundary(1);
 if (_vtype[0] == 'd') {
    for (; boit.Valid(); boit.Next())
      // For the upper and lower border for u we need to set the difference between
      // the inner value and twice the boundary value
      u->Cell(boit) = 2 * _velocity[0] - u->Cell(boit.Top());
  } else {
    for (; boit.Valid(); boit.Next())
      u->Cell(boit) = u->Cell(boit.Top()) - _h[1] * _velocity[0];
  }

  // Set right boundary
  boit.SetBoundary(2);
  if (_vtype[2] == 'd') {
    for (; boit.Valid(); boit.Next()) {
      // For u we have one halo cell column too many in the right direction, but
      // for simplicity we still use the same grid side as for v and p. Therefore
      // our actual boundary is one column to the left
      u->Cell(boit)        = _velocity[2];
      // For the right and left border for u we can set the boundary value directly
      u->Cell(boit.Left()) = _velocity[2];
    }
  } else {
    for (; boit.Valid(); boit.Next())
      u->Cell(boit) = u->Cell(boit.Left()) + _h[0] * _velocity[2];
  }

  // Set left boundary
  boit.SetBoundary(4);
  if (_vtype[6] == 'd') {
    for (; boit.Valid(); boit.Next())
      // For the right and left border for u for vwe can set the boundary value directly
      u->Cell(boit) = _velocity[6];
  } else {
    for (; boit.Valid(); boit.Next())
      u->Cell(boit) = u->Cell(boit.Right()) - _h[0] * _velocity[6];
  }
 
  // Set upper boundary
  boit.SetBoundary(3);
  if (_vtype[4] == 'd') {
    for (; boit.Valid(); boit.Next())
      // For the upper and lower border for u we need to set the difference between
      // the inner value and twice the boundary value
      u->Cell(boit) = 2 * _velocity[4] - u->Cell(boit.Down());
  } else {
    for (; boit.Valid(); boit.Next())
      u->Cell(boit) = u->Cell(boit.Down()) + _h[1] * _velocity[4];
  }
  
}

void Geometry::Update_V(Grid *v) const{
  BoundaryIterator boit(this, 1);
  
  // Set right boundary
  boit.SetBoundary(2);
  if (_vtype[3] == 'd') {
    for (; boit.Valid(); boit.Next())
      // For the left and right border for v we need to set the difference between
      // the inner value and twice the boundary value
      v->Cell(boit) = 2 * _velocity[3] - v->Cell(boit.Left());
  } else {
    for (; boit.Valid(); boit.Next())
      v->Cell(boit) = v->Cell(boit.Left()) + _h[1] * _velocity[3];
  }
  
  // Set left boundary
  boit.SetBoundary(4);
  if (_vtype[7] == 'd') {
    for (; boit.Valid(); boit.Next())
      // For the left and right border for v we need to set the difference between
      // the inner value and twice the boundary value
      v->Cell(boit) = 2 * _velocity[7] - v->Cell(boit.Right());
  } else {
    for (; boit.Valid(); boit.Next())
      v->Cell(boit) = v->Cell(boit.Right()) - _h[1] * _velocity[7];
  }
  
  // Set lower boundary
  boit.SetBoundary(1);
  if (_vtype[1] == 'd') {
    for (; boit.Valid(); boit.Next())
      // For the top and bottom borderfor v we can set the boundary value directly
      v->Cell(boit) = _velocity[1];
  } else {
    for (; boit.Valid(); boit.Next())
      v->Cell(boit) = v->Cell(boit.Top()) - _h[1] * _velocity[1];
  }
  
  // Set upper boundary
  boit.SetBoundary(3);
  if (_vtype[5] == 'd') {
    for (; boit.Valid(); boit.Next()) {
      // For v we have one halo cell row too many in the top direction, but
      // for simplicity we still use the same grid side as for v and p. Therefore
      // our actual boundary is one row below
      v->Cell(boit) = _velocity[5];
      // For the top and bottom border for v we can set the boundary value directly
      v->Cell(boit.Down()) = _velocity[5];
    }
  } else {
    for (; boit.Valid(); boit.Next())
      v->Cell(boit) = v->Cell(boit.Down()) + _h[1] * _velocity[5];
  }

}

void Geometry::Update_P(Grid *p) const{
  BoundaryIterator boit(this, 1);
  
  // Set right boundary
  boit.SetBoundary(2);
  if (_ptype[1] == 'd') {
    for (; boit.Valid(); boit.Next())
      p->Cell(boit) = _pressure[1];
  } else {
    for (; boit.Valid(); boit.Next())
      p->Cell(boit) = p->Cell(boit.Left()) + _h[1] * _pressure[1];
  }
  
  // Set left boundary
  boit.SetBoundary(4);
  if (_ptype[3] == 'd') {
    for (; boit.Valid(); boit.Next())
      p->Cell(boit) = _pressure[3];
  } else {
    for (; boit.Valid(); boit.Next())
      p->Cell(boit) = p->Cell(boit.Left()) + _h[1] * _pressure[3];
  }
  
  // Set lower boundary
  boit.SetBoundary(1);
  if (_ptype[0] == 'd') {
    for (; boit.Valid(); boit.Next())
      p->Cell(boit) = _pressure[0];
  } else {
    for (; boit.Valid(); boit.Next())
      p->Cell(boit) = p->Cell(boit.Top()) - _h[0] * _pressure[0];
  }
  
  // Set upper boundary
  boit.SetBoundary(3);
  if (_ptype[2] == 'd') {
    for (; boit.Valid(); boit.Next())
      p->Cell(boit) = _pressure[2];
  } else {
    for (; boit.Valid(); boit.Next())
      p->Cell(boit) = p->Cell(boit.Left()) + _h[0] * _pressure[2];
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
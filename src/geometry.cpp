#include "typedef.hpp"
#include "geometry.hpp"
#include "comm.hpp"
#include "iterator.hpp"
#include "grid.hpp"

#include <cstdio>  // file methods
#include <cstring> // string
#include <cstdlib> // read/write

Geometry::Geometry (const Communicator* comm)
  : _comm(comm) {
  // Init number of INNER cells in each dimension
  _bsize[0] = 4;
  _bsize[1] = 4;
  
  // Init length of driven cavity
  _blength[0] = 1.0;
  _blength[1] = 1.0;
  
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

  // Create extent
  _extent = new index_t[4];

  this->Recalculate();
}

Geometry::~Geometry(){
  // Fully delete _extents
  if (_comm->isMaster()) {
    for(int i_rank=0; i_rank < _comm->ThreadCnt(); ++i_rank)
      delete[] _extents[i_rank];
    
    delete[] _extents;
  }
  
  // Delete extent
  delete[] _extent;
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
        _bsize[0] = inval[0];
        _bsize[1] = inval[1];
      }
      continue;
    }

    if (strcmp(name, "length") == 0) {
      if (fscanf(handle, " %lf %lf\n", &inval[0], &inval[1])) {
        _blength[0] = inval[0];
        _blength[1] = inval[1];
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
  }
  fclose(handle);

  this->Recalculate();
}

void Geometry::Recalculate() {
  // Calculate cell width/height
  _h[0] = _blength[0] / _bsize[0];
  _h[1] = _blength[1] / _bsize[1];
  
  // Calculate inverse mesh width/height
  _invh[0] = _bsize[0] / _blength[0];
  _invh[1] = _bsize[1] / _blength[1];
  
  // Calculate size/length of subdomain
  _size = this->GetSubdomainSize();
  _length = this->GetSubdomainLength();

  // Calculate lower extent in x and y direction
  _extent[0] = _comm->ThreadIdx()[0] * _size[0];
  _extent[2] = _comm->ThreadIdx()[1] * _size[1];
  
  // Calculate upper extent in x and y direction
  _extent[1] = (_comm->ThreadIdx()[0] + 1) * _size[0];
  _extent[3] = (_comm->ThreadIdx()[1] + 1) * _size[1];
  
  // Communicate extents
  _extents = _comm->CollectExtent(_extent);
  
  // Set _size to size INCL ghost layers
  _size[0] += 2;
  _size[1] += 2;

  // Increase by one layer of border cells for the overall domain grid
  _bsize[0] += 2;
  _bsize[1] += 2;
}

index_t *Geometry::Extent() const{
  return _extent;
}

index_t **Geometry::Extents() const{
  return _extents;
}

const multi_index_t &Geometry::Size() const {
  return _size;
}

const multi_index_t &Geometry::TotalSize() const{
  return _bsize;
}

const multi_real_t &Geometry::Mesh() const {
  return _h;
}

const multi_real_t &Geometry::Length() const{
  return _length;
}

const multi_real_t &Geometry::TotalLength() const{
  return _blength;
}

const multi_real_t &Geometry::InvMesh() const{
  return _invh;
}

void Geometry::Update_U(Grid *u) const{
  BoundaryIterator boit(this, 1);
  
  if (_comm->isBottom()) {
    boit.SetBoundary(1);
    while (boit.Valid()) {
      // For the upper and lower border for u we need to set the difference between
      // the inner value and twice the boundary value
      u->Cell(boit) = 2 * _velocity[0] - u->Cell(boit.Top());
      boit.Next();
    }
  }
  
  // Set right boundary - zero dirichlet
  if (_comm->isRight()) {
    boit.SetBoundary(2);
    while (boit.Valid()) {
      // For u we have one halo cell column too many in the right direction, but
      // for simplicity we still use the same grid side as for v and p. Therefore
      // our actual boundary is one column to the left
      u->Cell(boit)        = _velocity[2];
      // For the right and left border we can set the boundary value directly
      u->Cell(boit.Left()) = _velocity[2];
      boit.Next();
    }
  }
  
  if (_comm->isLeft()) {
    boit.SetBoundary(4);
    while (boit.Valid()) {
      // For the right and left border we can set the boundary value directly
      u->Cell(boit) = _velocity[6];
      boit.Next();
    }
  }
  
  // Set upper boundary - non-zero dirichlet
  if (_comm->isTop()) {
    boit.SetBoundary(3);
    while (boit.Valid()) {
      // For the upper and lower border for u we need to set the difference between
      // the inner value and twice the boundary value
      u->Cell(boit) = 2 * _velocity[4] - u->Cell(boit.Down());
      boit.Next();
    }
  }
}

void Geometry::Update_V(Grid *v) const{
  BoundaryIterator boit(this, 1);
  
  if (_comm->isRight()) {
    boit.SetBoundary(2);
    while (boit.Valid()) {
      // For the left and right border for v we need to set the difference between
      // the inner value and twice the boundary value
      v->Cell(boit) = 2 * _velocity[3] - v->Cell(boit.Left());
      boit.Next();
    }
  }
  
  if (_comm->isLeft()) {
    boit.SetBoundary(4);
    while (boit.Valid()) {
      // For the left and right border for v we need to set the difference between
      // the inner value and twice the boundary value
      v->Cell(boit) = 2 * _velocity[7] - v->Cell(boit.Right());
      boit.Next();
    }
  }
  
  if (_comm->isBottom()) {
    boit.SetBoundary(1);
    while (boit.Valid()) {
      // For the top and bottom border we can set the boundary value directly
      v->Cell(boit) = _velocity[1];
      boit.Next();
    }
  }
  
  if (_comm->isTop()) {
    boit.SetBoundary(3);
    while (boit.Valid()) {
      // For v we have one halo cell row too many in the top direction, but
      // for simplicity we still use the same grid side as for v and p. Therefore
      // our actual boundary is one row below
      v->Cell(boit)        = _velocity[5];
      // For the top and bottom border we can set the boundary value directly
      v->Cell(boit.Down()) = _velocity[5];
      boit.Next();
    }
  }
}

void Geometry::Update_P(Grid *p) const{
  BoundaryIterator boit(this, 1);
  
  // Set right boundary
  if (_comm->isRight()) {
    boit.SetBoundary(2);
    while(boit.Valid()){
      p->Cell(boit) = _pressure[1];
      boit.Next();
    }
  }
  
  // Set left boundary
  if (_comm->isLeft()) {
    boit.SetBoundary(4);
    while(boit.Valid()){
      p->Cell(boit) = _pressure[3];
      boit.Next();
    }
  }
  
  // Set lower boundary
  if (_comm->isBottom()) {
    boit.SetBoundary(1);
    while(boit.Valid()){
      p->Cell(boit) =  _pressure[0];
      boit.Next();
    }
  }
  
  // Set upper boundary
  if (_comm->isTop()) {
    boit.SetBoundary(3);
    while(boit.Valid()){
      p->Cell(boit) =  _pressure[2];
      boit.Next();
    }
  }

  // Set corners to avg of neighbour cells
  if (_comm->isBottom() && _comm->isLeft()) {
    Iterator cbl = boit.CornerBottomLeft();
    p->Cell(cbl) = (p->Cell(cbl.Right()) + p->Cell(cbl.Top()))/2.0;
  }
  
  if (_comm->isBottom() && _comm->isRight()) {
    Iterator cbr = boit.CornerBottomRight();
    p->Cell(cbr) = (p->Cell(cbr.Left()) + p->Cell(cbr.Top()))/2.0;
  }
  
  if (_comm->isTop() && _comm->isLeft()) {
    Iterator ctl = boit.CornerTopLeft();
    p->Cell(ctl) = (p->Cell(ctl.Right()) + p->Cell(ctl.Down()))/2.0;
  }
  
  if (_comm->isTop() && _comm->isRight()) {
    Iterator ctr = boit.CornerTopRight();
    p->Cell(ctr) = (p->Cell(ctr.Left()) + p->Cell(ctr.Down()))/2.0;
  }
}

multi_index_t Geometry::GetSubdomainSize() {
  switch (_comm->ThreadCnt()) {
    case 1:
      return {_bsize[0], _bsize[1]};

    case 2:
      return {_bsize[0] / 2, _bsize[1]};

    case 4:
      return {_bsize[0] / 2, _bsize[1] / 2};

    default:
      throw std::runtime_error(std::string("Invalid number of processes: " + std::to_string(_comm->ThreadCnt())));
  }
}

multi_real_t Geometry::GetSubdomainLength() {
  switch (_comm->ThreadCnt()) {
    case 1:
      return {_blength[0], _blength[1]};
      
    case 2:
      return {_blength[0] / 2.0, _blength[1]};

    case 4:
      return {_blength[0] / 2.0, _blength[1] / 2.0};

    default:
      throw std::runtime_error(std::string("Invalid number of processes: " + std::to_string(_comm->ThreadCnt())));
  }
}
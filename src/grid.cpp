#include "grid.hpp"

#include "geometry.hpp"
#include "iterator.hpp"

#include <cmath>     // std::abs

/// Returns the value for the hat function at the given position on the unit
//  square. This function provides weights for each corner value that,
//  multiplied in sum by the values, interpolate the value based on the four
//  corner values.
//
//  @param mode int Which mode of the four-part hat function is evaluated. The
//    lower left corner is 1, lower right is 2, upper left is 3
//    and upper right is 4
//  @param pos multi_real_t The position to evaluate. It assumed this is
//    relative to the unit square
real_t hat(const int &mode, const multi_real_t &pos) {
  switch(mode) {
    case 1:
      return pos[0] * pos[1] - pos[1] - pos[0] + 1;
    case 2:
      return -pos[0] * pos[1] + pos[0];
    case 3:
      return -pos[0] * pos[1] + pos[1];
    case 4:
      return pos[0] * pos[1];
  }
  return 0.0;
}

/// Constructs a grid based on a geometry
//  @param geom   Geometry information
Grid::Grid(const Geometry *geom)
    : _geom(geom) {
  
  // Set offset to default
  _offset[0] = real_t(0.0);
  _offset[1] = real_t(0.0);
  
  // Calculate grid size and create data
  const multi_index_t size = _geom->Size();
  //TODO N oder N+1 ode N+2?
  _data = new real_t[(size[0])*(size[1])];
  
  // Init data with zeros
  this->Initialize(real_t(0.0));
}

/// Constructs a grid based on a geometry with an offset
//  @param geom   Geometry information
//  @param offset distance of staggered grid point to cell's anchor point;
//                (anchor point = lower left corner)
Grid::Grid(const Geometry *geom, const multi_real_t &offset)
    : _geom(geom), _offset(offset) {
  
  // Calculate grid size and create data
  const multi_index_t size = _geom->Size();
  //TODO N oder N+1 ode N+2?
  _data = new real_t[(size[0])*(size[1])];
  
  // Init data with zeros
  this->Initialize(real_t(0.0));
}

/// Deletes the grid
Grid::~Grid(){
  delete[] _data;
}

/// Initializes the grid with a fixed value
//  @param value  Fixed Value
void Grid::Initialize(const real_t &value){
  // Init array with fixed value
  Iterator it(_geom);
  
  for (it.First();it.Valid();it.Next()){
    this->Cell(it) = value;
  }
}

/// Write access to the grid cell at position [it]
//  @param it  Position [it]
real_t &Grid::Cell(const Iterator &it){
  return _data[it];
}

/// Read access to the grid cell at position [it]
//  @param it  Position [it]
const real_t &Grid::Cell(const Iterator &it) const{
  return _data[it];
}

/// Interpolate the value at an arbitrary position
//  For notes on how this algorithm works, see the implementation notes on
//  interpolation and the hat function.
//
//  @param pos multi_real_t An arbitrary position within the grid in absolute
//    coordinates.
real_t Grid::Interpolate(const multi_real_t &pos) const {
  // Clamp to a grid point (lower left corner)
  multi_index_t clamp = {
    pos[0] / _geom->Mesh()[0],
    pos[1] / _geom->Mesh()[1]
  };
  // Calculate position within unit square spanned by the four grid points
  multi_real_t modpos = {
    pos[0] / _geom->Mesh()[0] - clamp[0],
    pos[1] / _geom->Mesh()[1] - clamp[1]
  };
  // Calculate interpolated value by weighing the value of each corner
  // by the weight given by the hat function for that corner
  Iterator it = Iterator(_geom, clamp[1] * _geom->Size()[0] + clamp[0]);
  return this->Cell(it) * hat(1, modpos)
    + this->Cell(it.Right()) * hat(2, modpos)
    + this->Cell(it.Top()) * hat(3, modpos)
    + this->Cell(it.Top().Right()) * hat(4, modpos);

}

/// Computes the left-sided difference quotient in x-dim at [it]
//  @param it  Position [it]
real_t Grid::dx_l(const Iterator &it) const{
  return (this->Cell(it) - this->Cell(it.Left())) / _geom->Mesh()[0];
}
/// Computes the right-sided difference quotient in x-dim at [it]
//  @param it  Position [it]
real_t Grid::dx_r(const Iterator &it) const{
  return (this->Cell(it.Right()) - this->Cell(it)) / _geom->Mesh()[0];
}
/// Computes the left-sided difference quotient in y-dim at [it]
//  @param it  Position [it]
real_t Grid::dy_l(const Iterator &it) const{
  return (this->Cell(it) - this->Cell(it.Down())) / _geom->Mesh()[1];
}
/// Computes the right-sided difference quotient in x-dim at [it]
//  @param it  Position [it]
real_t Grid::dy_r(const Iterator &it) const{
  return (this->Cell(it.Top()) - this->Cell(it)) / _geom->Mesh()[1];
}
/// Computes the central difference quotient of 2nd order in x-dim at [it]
//  @param it  Position [it]
real_t Grid::dxx(const Iterator &it) const{
  return (this->Cell(it.Right()) + this->Cell(it.Left()) - this->Cell(it) - this->Cell(it))
    / (_geom->Mesh()[0] * _geom->Mesh()[0]);
}
/// Computes the central difference quotient of 2nd order in y-dim at [it]
//  @param it  Position [it]
real_t Grid::dyy(const Iterator &it) const{
  return (this->Cell(it.Top()) + this->Cell(it.Down()) - this->Cell(it) - this->Cell(it))
    / (_geom->Mesh()[1] * _geom->Mesh()[1]);
}

/// Computes u*du/dx with the donor cell method
//  @param it  Position [it]
real_t Grid::DC_udu_x(const Iterator &it, const real_t &alpha) const{
  //TODO
  return real_t(0.0);
}
/// Computes v*du/dy with the donor cell method
//  @param it  Position [it]
real_t Grid::DC_vdu_y(const Iterator &it, const real_t &alpha, const Grid *v) const{
  real_t ft = 0.25 * (v->Cell(it) + v->Cell(it.Right())) * (this->Cell(it) + this->Cell(it.Top()))
    - 0.25 * (v->Cell(it.Top()) + v->Cell(it.Right().Down())) * (this->Cell(it.Down()) + this->Cell(it));
  real_t st = 0.25 * abs(v->Cell(it) + v->Cell(it.Right())) * (this->Cell(it) - this->Cell(it.Top()))
    + 0.25 * abs(v->Cell(it.Down()) + v->Cell(it.Right().Down())) * (this->Cell(it.Down()) - this->Cell(it));
  return (ft + alpha * st) / _geom->Mesh()[1];
}
/// Computes u*dv/dx with the donor cell method
//  @param it  Position [it]
real_t Grid::DC_udv_x(const Iterator &it, const real_t &alpha, const Grid *u) const{
  //TODO
  return real_t(0.0);
}
/// Computes v*dv/dy with the donor cell method
//  @param it  Position [it]
real_t Grid::DC_vdv_y(const Iterator &it, const real_t &alpha) const{
  //TODO
  return real_t(0.0);
}

/// Returns the maximal value of the grid
real_t Grid::Max() const{
  // Create iterator and cycle _data
  Iterator it(_geom);
  real_t res = _data[0];
  for (it.First();it.Valid();it.Next())
    if (_data[it] > res) res = _data[it];
  return res;
}
/// Returns the minimal value of the grid
real_t Grid::Min() const{
  // Create iterator and cycle _data
  Iterator it(_geom);
  real_t res = _data[0];
  for (it.First();it.Valid();it.Next())
    if (_data[it] < res) res = _data[it];
  return res;
}
/// Returns the absolute maximal value
real_t Grid::AbsMax() const{
  // Create iterator and cycle _data
  Iterator it(_geom);
  real_t res = std::abs(_data[0]);
  for (it.First();it.Valid();it.Next())
    if (std::abs(_data[it]) > res) res = std::abs(_data[it]);
  return res;
}

/// Returns a pointer to the raw data
real_t *Grid::Data(){
  return _data;
}

/// Print field to console
void Grid::Print() const{
  // Cycle field with Iterator and print
  Iterator it(_geom);
  
  for (it.First();it.Valid();it.Next()){
    if((it % _geom->Size()[0]) == 0) printf("\n");
    printf("%6.2f", this->Cell(it));
  }
  printf("\n");
}


#include "grid.hpp"

#include "geometry.hpp"
#include "iterator.hpp"

#include <cmath>     // std::abs
#include <algorithm> // std::fill

/// Constructs a grid based on a geometry
//  @param geom   Geometry information
Grid::Grid(const Geometry *geom)
    : _geom(geom) {
  
  // Set offset to default
  _offset[0] = real_t(0.0);
  _offset[1] = real_t(0.0);
  
  // Create data
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
  
  // Create data
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
  const multi_index_t size = _geom->Size();
  std::fill( &_data[0], &_data[0] + (size[0])*(size[1]), value );
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

/// Interpolate the value at a arbitrary position
//  @param pos  arbitrary position
real_t Grid::Interpolate(const multi_real_t &pos) const {
  real_t v = 0.0; //TODO
  return v;
}

/// Computes the left-sided difference quatient in x-dim at [it]
//  @param it  Position [it]
real_t Grid::dx_l(const Iterator &it) const{
  //TODO
  return real_t(0.0);
}
/// Computes the right-sided difference quatient in x-dim at [it]
//  @param it  Position [it]
real_t Grid::dx_r(const Iterator &it) const{
  //TODO
  return real_t(0.0);
}
/// Computes the left-sided difference quatient in y-dim at [it]
//  @param it  Position [it]
real_t Grid::dy_l(const Iterator &it) const{
  //TODO
  return real_t(0.0);
}
/// Computes the right-sided difference quatient in x-dim at [it]
//  @param it  Position [it]
real_t Grid::dy_r(const Iterator &it) const{
  //TODO
  return real_t(0.0);
}
/// Computes the central difference quatient of 2nd order in x-dim at [it]
//  @param it  Position [it]
real_t Grid::dxx(const Iterator &it) const{
  //TODO
  return real_t(0.0);
}
/// Computes the central difference quatient of 2nd order in y-dim at [it]
//  @param it  Position [it]
real_t Grid::dyy(const Iterator &it) const{
  //TODO
  return real_t(0.0);
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
  //TODO
  return real_t(0.0);
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
  real_t res = _data[0];
  for (it.First();it.Valid();it.Next())
          if (std::abs(_data[it]) > res) res = std::abs(_data[it]);
  return res;
}

/// Returns a pointer to the raw data
real_t *Grid::Data(){
  return _data;
}


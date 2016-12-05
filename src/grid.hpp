/*
 * Copyright (C) 2015   Malte Brunn
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "typedef.hpp"
//------------------------------------------------------------------------------
#ifndef __GRID_HPP
#define __GRID_HPP
//------------------------------------------------------------------------------
class Grid {
public:
  /// Constructs a grid based on a geometry.
  ///
  /// @param geom Geometry The geometry containing information on sizes, etc.
  Grid(const Geometry *geom);

  /// Constructs a grid based on a geometry with an offset.
  ///
  /// @param geom Geometry The geometry containing information on sizes, etc.
  /// @param offset multi_real_t Distance of staggered grid point to cell's anchor point;
  ///   (anchor point = lower left corner)
  Grid(const Geometry *geom, const multi_real_t &offset);

  /// Deconstructs a grid instance.
  ~Grid();

  /// Initializes the grid with a fixed value in each cell.
  ///
  /// @param value real_t The value to set in each cell
  void Initialize(const real_t &value);

  /// Write access to the grid cell at position [it].
  ///
  /// @param it Iterator The position
  real_t &Cell(const Iterator &it);

  /// Read access to the grid cell at position [it].
  ///
  /// @param it Iterator The position
  const real_t &Cell(const Iterator &it) const;

  /// Interpolate the value at an arbitrary position
  /// For notes on how this algorithm works, see the implementation notes on
  /// interpolation and the hat function.
  ///
  /// @param pos multi_real_t An arbitrary position within the grid in absolute
  ///   coordinates.
  real_t Interpolate(const multi_real_t &pos) const;

  /// Computes the left-sided difference quotient in x-dim at [it].
  ///
  /// @param it Iterator The position
  real_t dx_l(const Iterator &it) const;

  /// Computes the right-sided difference quotient in x-dim at [it].
  ///
  /// @param it Iterator The position
  real_t dx_r(const Iterator &it) const;

  /// Computes the left-sided difference quotient in y-dim at [it].
  ///
  /// @param it Iterator The position
  real_t dy_l(const Iterator &it) const;

  /// Computes the right-sided difference quotient in x-dim at [it].
  ///
  /// @param it Iterator The position
  /// @param it  Position [it]
  real_t dy_r(const Iterator &it) const;

  /// Computes the central difference quotient of 2nd order in x-dim at [it].
  ///
  /// @param it Iterator The position
  real_t dxx(const Iterator &it) const;

  /// Computes the central difference quotient of 2nd order in y-dim at [it].
  ///
  /// @param it Iterator The position
  real_t dyy(const Iterator &it) const;

  /// Computes u*du/dx with the donor cell method at position [it].
  ///
  /// @param it Iterator The position
  /// @param alpha real_t The alpha parameter; a weight for the DC algorithm
  real_t DC_udu_x(const Iterator &it, const real_t &alpha) const;

  /// Computes v*du/dy with the donor cell method at position [it].
  ///
  /// @param it Iterator The position
  /// @param alpha real_t The alpha parameter; a weight for the DC algorithm
  /// @param v Grid A grid with the values for v. "this" is assumed u
  real_t DC_vdu_y(const Iterator &it, const real_t &alpha, const Grid *v) const;

  /// Computes u*dv/dx with the donor cell method at position [it].
  ///
  /// @param it Iterator The position
  /// @param alpha real_t The alpha parameter; a weight for the DC algorithm
  /// @param v Grid A grid with the values for u. "this" is assumed v
  real_t DC_udv_x(const Iterator &it, const real_t &alpha, const Grid *u) const;

  /// Computes v*dv/dy with the donor cell method at position [it].
  ///
  /// @param it Iterator The position
  /// @param alpha real_t The alpha parameter; a weight for the DC algorithm
  real_t DC_vdv_y(const Iterator &it, const real_t &alpha) const;

  /// Returns the maximal value of the grid.
  ///
  /// @return real_t The maximal value
  real_t Max() const;

  /// Returns the minimal value of the grid.
  ///
  /// @return real_t The minimal value
  real_t Min() const;

  /// Returns the absolute maximal value of the grid.
  ///
  /// @return real_t The absolute maximal value
  real_t AbsMax() const;

  /// Returns a pointer to the raw data.
  ///
  /// @return real_t* The data of the grid
  real_t *Data();
  
  /// Prints the grid values to the console.
  void Print() const;

  /// Returns the values along the left boundary of the grid. If the offset
  /// flag is given and true, offsets the values by one in x dimension. This
  /// therefore encompasses the inner boundary of the grid.
  ///
  /// @param offset bool If true, offsets the reading of values by one in x
  ///   dimension
  /// @return arrayBuffer The values along the left boundary
  arrayBuffer GetLeftBoundary(bool offset) const;

  /// Returns the values along the right boundary of the grid. If the offset
  /// flag is given and true, offsets the values by one in x dimension. This
  /// therefore encompasses the inner boundary of the grid.
  ///
  /// @param offset bool If true, offsets the reading of values by one in x
  ///   dimension
  /// @return arrayBuffer The values along the right boundary
  arrayBuffer GetRightBoundary(bool offset) const;

  /// Returns the values along the top boundary of the grid. If the offset
  /// flag is given and true, offsets the values by one in y dimension. This
  /// therefore encompasses the inner boundary of the grid.
  ///
  /// @param offset bool If true, offsets the reading of values by one in y
  ///   dimension
  /// @return arrayBuffer The values along the top boundary
  arrayBuffer GetTopBoundary(bool offset) const;

  /// Returns the values along the bottom boundary of the grid. If the offset
  /// flag is given and true, offsets the values by one in y dimension. This
  /// therefore encompasses the inner boundary of the grid.
  ///
  /// @param offset bool If true, offsets the reading of values by one in y
  ///   dimension
  /// @return arrayBuffer The values along the bottom boundary
  arrayBuffer GetBottomBoundary(bool offset) const;

  /// Writes the given data to the left boundary of the grid.
  ///
  /// @param data real_t* The data to be written
  void WriteLeftBoundary(real_t* data);

  /// Writes the given data to the right boundary of the grid.
  ///
  /// @param data real_t* The data to be written
  void WriteRightBoundary(real_t* data);

  /// Writes the given data to the top boundary of the grid.
  ///
  /// @param data real_t* The data to be written
  void WriteTopBoundary(real_t* data);

  /// Writes the given data to the bottom boundary of the grid.
  ///
  /// @param data real_t* The data to be written
  void WriteBottomBoundary(real_t* data);
  
  /// Adds the given value to all cells along the bottom boundary.
  ///
  /// @param val real_t The value to add
  void AddValInXDir(real_t val);

  /// Adds the given array of values to the cells along the bottom
  /// boundary. It is assumed that the number of cells matches the
  /// number of given values, so each value is added to one cell only.
  ///
  /// @param data real_t The values to add
  void AddXArrayInYDir(real_t* data);
  
  /// Returns the value of the grid in the right-most, lower-most cell of
  /// the grid.
  ///
  /// @return real_t The value in the bottom-right corner
  real_t GetBottomRightCorner() const;

private:
  /// _data real_t* The raw data of the grid as pointer-array of real_t numbers
  real_t *_data;

  /// _bufferX real_t* A buffer array used in exchanging boundary values in
  /// horizontal direction
  real_t *_bufferX;

  /// _bufferY real_t* A buffer array used in exchanging boundary values in
  /// vertical direction
  real_t *_bufferY;

  /// _geom Geometry The geometry instance containing boundary values and such
  const Geometry *_geom;

  /// _offset multi_real_t The offset from the grid's coordinate system to the
  ///   global coordinate system.
  multi_real_t _offset;
};
//------------------------------------------------------------------------------
#endif // __GRID_HPP

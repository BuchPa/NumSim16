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
#ifndef __GEOMETRY_HPP
#define __GEOMETRY_HPP
//------------------------------------------------------------------------------
class Geometry {
public:
  /// Constructs a geometry instance with default values. Other values can be
  /// loaded by using the Load method.
  ///
  /// Default values are:
  /// Size: 8 by 8
  /// Length: 0.0 to 1.0 and 0.0 to 1.0
  /// Border-Values: 
  ///
  ///      u=1, v=0
  ///    -------------
  ///    |           |
  /// u=0|           |u=0
  /// v=0|           |v=0
  ///    |           |
  ///    |           |
  ///    -------------
  ///      u=0, v=0
  
  /// Constructs a new Geometry.
  Geometry();
  
  /// Constructs a new Geometry considering the given Communicator.
  ///
  ///  @param comm Communicator The communicator to work with
  Geometry (const Communicator* comm);

  /// Loads values for the geometry from a file.
  /// See sample file ex1_geometry for how the file should be structured.
  ///
  ///  @param file char* File path as char array
  void Load(const char *file);

  /// Recalculates the mesh width, inverse mesh width and overhang-size and
  /// saves it in their correspondig private members.
  void CalculateMesh();

  /// Returns the number of cells in each dimension on the current CPU.
  ///
  /// @return multi_index_t The size in x and y dimension on the current CPU
  const multi_index_t &Size() const;

  /// Returns the number of cells in each dimension on all CPUs.
  ///
  /// @return multi_index_t The size in x and y dimension on all CPUs
  const multi_index_t &TotalSize() const;

  /// Returns the length of the domain in each dimension on the current CPU.
  ///
  /// @return multi_real_t The domain length in x and y dimension on the current CPU
  const multi_real_t &Length() const;

  /// Returns the length of the domain in each dimension on all CPUs.
  ///
  /// @return multi_real_t The domain length in x and y dimension on all CPUs
  const multi_real_t &TotalLength() const;

  /// Returns the mesh width of a cell in each dimension.
  ///
  /// @return multi_real_t The mesh width in x and y dimension
  const multi_real_t &Mesh() const;

  /// Returns the inverse mesh width.
  ///
  /// @see Geometry::Mesh()
  /// @return multi_real_t The inverse mesh width
  const multi_real_t &InvMesh() const;

  /// Updates the velocity field u at the boundaries by applying the
  /// boundary values to them.
  ///
  /// @param u Grid The velocity field u in x direction
  void Update_U(Grid *u) const;

  /// Updates the velocity field v at the boundaries by applying the
  /// boundary values to them.
  ///
  /// @param v Grid The velocity field v in y direction
  void Update_V(Grid *v) const;

  /// Updates the pressure field p at the boundaries by applying the
  /// boundary values to them.
  ///
  /// @param p Grid The pressure field p
  void Update_P(Grid *p) const;

private:
  /// _comm Communicator The communicator containing parallelization information
  const Communicator* _comm;

  /// _size multi_index_t The number of cells in each dimension on the current CPU
  multi_index_t _size;

  /// _bsize multi_index_t The number of cells in each dimension on all CPUs
  multi_index_t _bsize;

  /// _length multi_real_t The domain length in each dimension on the current CPU
  multi_real_t _length;

  /// _blength multi_real_t The domain length in each dimension on all CPUs
  multi_real_t _blength;

  /// _h multi_real_t The mesh width in each dimension
  multi_real_t _h;

  /// _velocity multi_real_t The velocity boundary values
  multi_real_t _velocity;

  /// _pressure real_t The pressure boundary value
  real_t _pressure;
  
  // _invh multi_real_t The inverse mesh width in each dimension
  multi_real_t _invh;
};
//------------------------------------------------------------------------------
#endif // __GEOMETRY_HPP

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
  /// Size: 4 by 4
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
  Geometry();

  /// Loads values for the geometry from a file.
  /// See sample file ex1_geometry for how the file should be structured.
  ///
  ///  @param file char* File path as char array
  void Load(const char *file);

  /// Recalculates the mesh width, inverse mesh width and overhang-size and
  /// saves it in their correspondig private members.
  void Recalculate();

  /// Returns the number of cells in each dimension.
  ///
  /// @return multi_index_t The size in x and y dimension
  const multi_index_t &Size() const;

  /// Returns the length of the domain in each dimension.
  ///
  /// @return multi_real_t The domain length in x and y dimension
  const multi_real_t &Length() const;

  /// Returns the mesh width of a cell in each dimension.
  ///
  /// @return multi_real_t The mesh width in x and y dimension
  const multi_real_t &Mesh() const;

  /// Returns the inverse mesh width.
  ///
  /// @see Geometry::Mesh()
  /// @return multi_real_t The inverse mesh width
  const multi_real_t &InvMesh() const;
  
  ///  Returns the particles for particle tracing
  ///
  /// @return particles_t The particles for particle tracing
  particles_t ParticleTraces() const;
  
  ///  Returns the particles for streaklines
  ///
  /// @return particles_t The particles for streaklines
  particles_t Streaklines() const;

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
  /// _size multi_index_t The number of cells in each dimension
  multi_index_t _size;

  /// _length multi_real_t The domain length in each dimension
  multi_real_t _length;

  /// _h multi_real_t The mesh width in each dimension
  multi_real_t _h;

  /// _velocity array_t<real_t, 8> The velocity boundary values for the boundaries
  /// numbered clockwise from the lower boundary with two values each
  array_t<real_t, 8> _velocity;

  /// _pressure array_t<real_t, 4> The pressure boundary values for the boundaries
  /// numbered clockwise from the lower boundary with two values each
  array_t<real_t, 4> _pressure;

  /// _vtype array_t<char, 8> The type of boundary (Dirichlet or von Neuman) for
  /// the velocity boundary values
  array_t<char, 8> _vtype;

  /// _vtype array_t<char, 8> The type of boundary (Dirichlet or von Neuman) for
  /// the pressure boundary values
  array_t<char, 4> _ptype;
  
  // _invh multi_real_t The inverse mesh width in each dimension
  multi_real_t _invh;
  
  // _trace particles_t List of all particles to trace
  particles_t _trace;
  
  // _streakline particles_t List of all particles to build a streakline
  particles_t _streakline;
};
//------------------------------------------------------------------------------
#endif // __GEOMETRY_HPP

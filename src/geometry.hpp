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
#include "iterator.hpp"
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

  /// Desctructor.
  ~Geometry();

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

  /// Returns the cell type of the cell at the given iterator position.
  ///
  /// @see Enum CellType
  /// @param pos index_t The position of the iterator
  /// @return char The cell type at the given position
  char CellTypeAt(index_t pos) const;

  /// Returns the cell type of the cell at the given x/y position.
  ///
  /// @see Enum CellType
  /// @param pos index_t The x/y position
  /// @return char The cell type at the given position
  char CellTypeAt(index_t xpos, index_t ypos) const;

  /// Returns cell types of the von-Neumann neighborhood of the cell at the
  /// given iterator position coded into a four-sized int array. The four
  /// neighbors are numbered counter-clockwise beginning with the lower neighbor.
  ///
  /// @param pos index_t The iterator position
  /// @return int* The cell types of the neighborhood of the given cell
  int* NeighbourCode(index_t pos) const;

private:
  /// _size multi_index_t The number of cells in each dimension
  multi_index_t _size;

  /// _length multi_real_t The domain length in each dimension
  multi_real_t _length;

  /// _h multi_real_t The mesh width in each dimension
  multi_real_t _h;

  /// _velocity multi_real_t The velocity boundary values for the boundaries
  multi_real_t _velocity;

  /// _pressure real_t The pressure boundary values for the boundaries
  real_t _pressure;
  
  // _invh multi_real_t The inverse mesh width in each dimension
  multi_real_t _invh;

  /// cells char* A field containing the cell types, e.g. fluid cell, obstacle,
  /// inflow boundary etc.
  char* _cells;
  
  // _trace particles_t List of all particles to trace
  particles_t _trace;
  
  // _streakline particles_t List of all particles to build a streakline
  particles_t _streakline;
  
  /// _nb int* An array used in calculating the neighborhood of a cell
  int* _nb;
  
  /// Cycle the full boundary given by BoundaryIterator boit on Grid u
  ///
  /// @param u Grid The velocity field u in x direction
  /// @param boit BoundaryIterator Boundary to iterate until boit.Valid() equals false
  void CycleBoundary_U(Grid *u, BoundaryIterator boit) const;
  
  /// Cycle the full boundary given by BoundaryIterator boit on Grid v
  ///
  /// @param v Grid The velocity field v in y direction
  /// @param boit BoundaryIterator Boundary to iterate until boit.Valid() equals false
  void CycleBoundary_V(Grid *v, BoundaryIterator boit) const;
  
  /// Cycle the full boundary given by BoundaryIterator boit on Grid p
  ///
  /// @param p Grid The pressure field p
  /// @param boit BoundaryIterator Boundary to iterate until boit.Valid() equals false
  void CycleBoundary_P(Grid *p, BoundaryIterator boit) const;
  
  /// Sets Dirichlet boundary condition for Grid u on boundary boit.Boundary() at position boit to value
  /// 
  /// @param u Grid The velocity field u in x direction
  /// @param boit BoundaryIterator Position to set boundary condition
  /// @param value real_t Value to set as boundary condition
  void SetUDirichlet(Grid *u, const BoundaryIterator &boit, const real_t &value) const;
  
  /// Sets Neumann boundary condition for Grid u on boundary boit.Boundary() at position boit to value
  /// 
  /// @param u Grid The velocity field u in x direction
  /// @param boit BoundaryIterator Position to set boundary condition
  /// @param value real_t Value to set as boundary condition
  void SetUNeumann(Grid *u, const BoundaryIterator &boit, const real_t &value) const;
  
  /// Sets Dirichlet boundary condition for Grid u on boundary boit.Boundary() at position boit to a
  /// value corresponding to a parabolic inflow with value being maximal velocity.
  /// @param u Grid The velocity field u in x direction
  /// @param boit BoundaryIterator Position to set boundary condition
  /// @param value real_t Value to set as boundary condition
  /// @coord y coordinate of boit to adapt local velocity to the parabol 
  void SetUParabol(Grid *u, const BoundaryIterator &boit, const real_t &value, real_t &coord) const;
  
/// Sets Dirichlet boundary condition for Grid u on boundary boit.Boundary() at position boit to value
  /// 
  /// @param v Grid The velocity field v in y direction
  /// @param boit BoundaryIterator Position to set boundary condition
  /// @param value real_t Value to set as boundary condition
  void SetVDirichlet(Grid *v, const BoundaryIterator &boit, const real_t &value) const;
  
  /// Sets Neumann boundary condition for Grid u on boundary boit.Boundary() at position boit to value
  /// 
  /// @param v Grid The velocity field v in y direction
  /// @param boit BoundaryIterator Position to set boundary condition
  /// @param value real_t Value to set as boundary condition
  void SetVNeumann(Grid *v, const BoundaryIterator &boit, const real_t &value) const;
  
  /// Sets Dirichlet boundary condition for Grid u on boundary boit.Boundary() at position boit to value
  /// 
  /// @param p Grid The pressure field p
  /// @param boit BoundaryIterator Position to set boundary condition
  /// @param value real_t Value to set as boundary condition
  void SetPDirichlet(Grid *p, const BoundaryIterator &boit, const real_t &value) const;
  
  /// Sets Neumann boundary condition for Grid u on boundary boit.Boundary() at position boit to value
  /// 
  /// @param p Grid The pressure field p
  /// @param boit BoundaryIterator Position to set boundary condition
  /// @param value real_t Value to set as boundary condition
  void SetPNeumann(Grid *p, const BoundaryIterator &boit, const real_t &value) const;
};
//------------------------------------------------------------------------------
#endif // __GEOMETRY_HPP

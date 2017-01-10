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
#ifndef __ITERATOR_HPP
#define __ITERATOR_HPP
//------------------------------------------------------------------------------
/// Iterator base class. Contains functionality for an iterator that covers a
/// grid and provides access to cells without having to worry about boundaries
/// within the calling function.
///
/// Child classes may extend Iterator for example to restrict iteration to a
/// certain range of cells.
class Iterator {
public:
  /// Constructs a new Iterator constricted by the given geometry.
  ///
  /// @param geom Geometry The geometry to work with
  Iterator(const Geometry *geom);

  /// Constructs a new Iterator on a geometry with a defined starting value.
  ///
  /// @param geom Geometry The geometry to work with
  /// @param value index_t Starting value
  Iterator(const Geometry *geom, const index_t &value);
  
  /// Constructs a new Iterator on a geometry with a defined starting value and the geometry sizes given as parameters
  ///
  /// @param value index_t Starting value
  /// @param xmax index_t Size of the geometry in x direction
  /// @param ymax index_t Size of the geometry in y direction
  Iterator(const index_t &value, const index_t &xmax, const index_t &ymax);

  /// Returns the current position value.
  ///
  /// @return index_t The current position value
  virtual const index_t &Value() const;

  /// Cast operator to convert Iterators to integers.
  ///
  /// @return index_t The integer (as index_t type) value of the Iterator
  virtual operator const index_t &() const;

  /// Returns the position coordinates.
  ///
  /// @return multi_index_t The current position on the grid in each dimension
  virtual multi_index_t Pos() const;

  /// Sets the iterator to the first element.
  virtual void First();

  /// Goes to the next element of the iterator, disables it if position is end.
  virtual void Next();

  /// Checks if the iterator still has a valid value.
  virtual bool Valid() const;

  /// Returns an Iterator that is located left from this one.
  /// if we are at the left boundary, the cell sees itself.
  ///
  /// @return Iterator An iterator that starts on the cell to the left of this
  ///   or itself, if we are at the left boundary.
  virtual Iterator Left() const;

  /// Returns an Iterator that is located right from this one
  // If we are at the right boundary, the cell sees itself.
  ///
  /// @return Iterator An iterator that starts on the cell to the right of this
  ///   or itself, if we are at the right boundary.
  virtual Iterator Right() const;

  /// Returns an Iterator that is located above this one
  // If we are at the upper domain boundary, the cell sees itself.
  ///
  /// @return Iterator An iterator that starts on the cell above of this
  ///   or itself, if we are at the upper boundary.
  virtual Iterator Top() const;

  /// Returns an Iterator that is located below this one
  // If we are at the lower domain boundary, the cell sees itself.
  ///
  /// @return Iterator An iterator that starts on the cell below of this
  ///   or itself, if we are at the lower boundary.
  virtual Iterator Down() const;
  
  /// Checks if current value is valid.
  virtual void UpdateValid();
  
  /// Tests Iterator in a dummy run.
  ///
  /// @param printNeighbours bool If true, prints the neighbors of each cell
  virtual void TestRun(const bool printNeighbours);

  /// Print iterator value with all its printNeighbours.
  virtual void printNeighbours() const;

protected:
  /// _value index_t The current value of the iterator
  index_t _value;

  /// _valid bool Flag to contain the iterators validity, e.g. if we have
  ///   reached the end of the grid
  bool _valid;
  
  // _xmax index_t Size of the geometry in x direction
  index_t _xmax;
  
  // _ymax index_t Size of the geometry in y direction
  index_t _ymax;

  // _itmax index_t Maximal value for _value (inclusive _itmax)
  index_t _itmax;

  // _itmind index_t Minimal value for _value (inclusive _itmin)
  index_t _itmin;
};

//------------------------------------------------------------------------------
/// An iterator that only iterates over interio cells of a grid.
class InteriorIterator : public Iterator {
public:
  /// Construct a new InteriorIterator working with the given geometry.
  ///
  /// @param geom Geometry The geometry to work with
  InteriorIterator(const Geometry *geom);
  
  /// Goes to the next element of the iterator, disables it if position is end.
  void Next();
};

//------------------------------------------------------------------------------
/// An iterator that only iterates over the domain boundary cells.
class BoundaryIterator : public Iterator {
public:
  /// Constructs a new BoundaryIterator working with the given geometry and
  /// iterating over the given boundary. The boundaries are numbered as
  /// follows:
  ///   1: Border Bottom
  ///   2: Border Right
  ///   3: Border Top
  ///   4: Border Left
  ///
  /// @param geom Geometry The geometry to work with
  /// @param boundary index_t Which boundary to iterate over
  BoundaryIterator(const Geometry *geom, const index_t &boundary);

  /// Sets the boundary to iterate.
  ///
  /// @see BoundaryIterator::BoundaryIterator() for how the boundaries are
  ///   numbered
  /// @param boundary index_t Which boundary to iterate over
  void SetBoundary(const index_t &boundary);
  
  /// Gets the currently iterated boundary.
  ///
  /// @see BoundaryIterator::BoundaryIterator() for how the boundaries are
  ///   numbered
  /// @return boundary index_t Which boundary to iterate over
  const index_t &Boundary() const;

  /// Sets the iterator to the first element.
  void First();

  /// Goes to the next element of the iterator, disables it if position is end.
  void Next();
  
  /// Returns Iterator pointing to the bottom left corner.
  ///
  /// @return Iterator An iterator sitting on the bottom left corner
  Iterator CornerBottomLeft();

  /// Returns Iterator pointing to the bottom right corner.
  ///
  /// @return Iterator An iterator sitting on the bottom right corner
  Iterator CornerBottomRight();

  /// Returns Iterator pointing to the top left corner.
  ///
  /// @return Iterator An iterator sitting on the top left corner
  Iterator CornerTopLeft();

  /// Returns Iterator pointing to the top right corner.
  ///
  /// @return Iterator An iterator sitting on the top right corner
  Iterator CornerTopRight();

private:
  /// _boundary index_t On which boundary the iterator operates.
  /// @see BoundaryIterator::BoundaryIterator() for how the boundaries are
  ///   numbered
  index_t _boundary;
};

//------------------------------------------------------------------------------

/// An iterator that only iterates over the domain boundary cells.
class ObstacleIterator : public InteriorIterator {
public:
  /// Constructs a new ObstacleIterator working with the given geometry and
  /// iterating over the obstacle cells within the inner domain.
  ///
  /// @param geom Geometry The geometry to work with
  ObstacleIterator(const Geometry *geom);

  /// Sets the iterator to the first element.
  void First();

  /// Goes to the next element of the iterator, disables it if position is end.
  void Next();
private:
  /// cells char* A field containing the cell types, e.g. fluid cell, obstacle,
  /// inflow boundary etc.
  const char* _cells;
};
//------------------------------------------------------------------------------
#endif // __ITERATOR_HPP

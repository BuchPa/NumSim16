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
//------------------------------------------------------------------------------
#include "typedef.hpp"
//------------------------------------------------------------------------------
#ifndef __SOLVER_HPP
#define __SOLVER_HPP
//------------------------------------------------------------------------------

/// Abstract base class for an iterative solver. Child classes will implement
/// specific algorithmn for solving the fluid dynamics equation (the
/// Navier-Stokes equation)
class Solver {
public:
  /// Constructs the solver working with the given geometry.
  ///
  /// @param geom Geometry The geometry to work with
  Solver(const Geometry *geom);

  /// Destructs the solver instance.
  virtual ~Solver();

  /// Performs on cycle of the solver algorithm and returns the residual after
  /// the calculation. This function must be implemented in a child class.
  ///
  /// @param [in][out] grid Grid The current p values. This grid will be modified with the
  ///   new values.
  /// @param rhs Grid The RHS values used in the calculation
  /// @return real_t The accumulated residual
  virtual real_t Cycle(Grid *grid, const Grid *rhs) const = 0;

protected:
  /// _geom Geometry The geometry for boundary values etc.
  const Geometry *_geom;

  /// _hsquare real_t A constant used in the calculation
  real_t _hsquare;

  /// _ihsquare real_t Inverse of the hsquare constant
  real_t _ihsquare;

  /// _sh_ism0 real_t Shorthand variables for the inverted square mesh width
  ///   in x direction.
  real_t _sh_ism0;

  /// _sh_ism1 real_t Shorthand variables for the inverted square mesh width
  ///   in y direction.
  real_t _sh_ism1;

  /// Returns the residual at [it] for the pressure-Poisson equation.
  ///
  /// @param it Iterator The position
  /// @param grid Grid The grid containing the p values
  /// @param rhs Grid The grid containging the RHS values
  real_t localRes(const Iterator &it, const Grid *grid, const Grid *rhs) const;
};

//------------------------------------------------------------------------------

/// The SOR solver, implementing Solver functionality by using the SOR algo-
/// rithm.
class SOR : public Solver {
public:
  /// Constructs an SOR solver using the given geometry and omega parameter.
  ///
  /// @param geom Geometry The geometry
  /// @param omega real_t The omega parameter used in the calculation
  SOR(const Geometry *geom, const real_t &omega);

  /// Deconstructs the SOR instance.
  ~SOR();

  /// Performs one cycle of the solver algorithm and returns the residual after
  /// the calculation.
  ///
  /// @param grid Grid The current p values. This grid will be modified with the
  ///   new values.
  /// @param rhs Grid The RHS values used in the calculation
  /// @return real_t The accumulated residual
  real_t Cycle(Grid *grid, const Grid *rhs) const;

protected:
  /// _omega real_t The omega parameter
  real_t _omega;
};
//------------------------------------------------------------------------------
#endif // __SOLVER_HPP

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
#ifndef __PARAMETER_HPP
#define __PARAMETER_HPP
//------------------------------------------------------------------------------
class Parameter {
public:
  /// Constructs a new Parameter set with default values.
  /// See sample file ex1_parameter for these values.
  Parameter();

  /// Loads the parameter values from a file. See sample file
  /// ex1_parameter for information on the required format of this file.
  ///
  /// @param file char* The filepath as char array
  void Load(const char *file);

  /// Returns the value of the reynolds number.
  ///
  /// @return real_t The value of the reynolds number
  const real_t &Re() const;

  /// Returns the value of the inverse reynolds number.
  ///
  /// @return real_t The value of the inverse reynolds number
  const real_t &InvRe() const;

  /// Returns the value of the omega parameter.
  ///
  /// @return real_t The value of the omega parameter
  const real_t &Omega() const;

  /// Returns the value of the alpha parameter.
  ///
  /// @return real_t The value of the alpha parameter
  const real_t &Alpha() const;

  /// Returns the value of the minimum timestep.
  ///
  /// @return real_t The value of the minimum timestep
  const real_t &Dt() const;

  /// Returns the upper bound on the simulation time.
  ///
  /// @return real_t The upper bound on the simulation time
  const real_t &Tend() const;

  /// Returns the maximum number of iterations of the solver.
  ///
  /// @return index_t The value of the maximum number of solver iterations
  const index_t &IterMax() const;

  /// Returns the value of the epsilon parameter.
  ///
  /// @return real_t The value of the epsilon parameter
  const real_t &Eps() const;

  /// Returns the value of the tau parameter.
  ///
  /// @return real_t The value of the tau parameter
  const real_t &Tau() const;

private:
  /// _re real_t The reynolds number
  real_t _re;

  /// _invre real_t The inverse reynolds number
  real_t _invre;

  /// _omega real_t The omega parameter
  real_t _omega;

  /// _alpha real_t The alpha parameter
  real_t _alpha;

  /// _dt real_t The minimum timestep
  real_t _dt;

  /// _tend real_t The upper bound of simulation time
  real_t _tend;

  /// _eps real_t The epsilon parameter
  real_t _eps;

  /// _tau real_t The tau parameter
  real_t _tau;

  /// _itermax index_t The maximum number of iterations of the solver
  index_t _itermax;
};
//------------------------------------------------------------------------------
#endif // __PARAMETER_HPP

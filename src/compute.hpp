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
#include "substance.hpp"
//------------------------------------------------------------------------------
#ifndef __COMPUTE_HPP
#define __COMPUTE_HPP
//------------------------------------------------------------------------------
class Compute {
public:
  /// Creates a compute instance with given geometry and parameter.
  //
  //  @param geom Geometry The geometry to work with
  //  @param param Parameter The parameter to work with
  Compute(const Geometry *geom, const Parameter *param, const Substance *subst);

  /// Deconstructs the compute instance.
  ~Compute();

  /// Execute one time step of the fluid simulation.
  ///
  /// @param int stepNr The number of the current step. Is used for calculating
  ///   when to add new particles
  /// @return bool A boolean, if the current timestep needs to be printed to CSV
  bool TimeStep(int stepNr);

  /// Returns the simulated time in total.
  //
  // @return real_t The simulated time in total.
  const real_t &GetTime() const;

  /// Returns the pointer to U.
  //
  // @return Grid The grid containing the u velocities.
  const Grid *GetU() const;

  /// Returns the pointer to V.
  //
  // @return Grid The grid containing the v velocities.
  const Grid *GetV() const;

  /// Returns the pointer to P.
  //
  // @return Grid The grid containing the pressure.
  const Grid *GetP() const;

  /// Returns the pointer to RHS.
  //
  // @return Grid The grid containing the right-hand-side of the Navier-Stokes
  //   equation.
  const Grid *GetRHS() const;

  /// Computes and returns the absolute velocity (u_x + u_y)^(1/2) on a grid.
  //
  // @return Grid A grid containing the absolute velocities.
  const Grid *GetVelocity();

  /// Computes and returns the vorticity on a grid.
  //
  // @return Grid A grid containing the vorticity.
  const Grid *GetVorticity();

  /// Computes and returns the stream line values on a grid.
  //
  // @return Grid A grid containing the stream lines.
  const Grid *GetStream();
  
  /// Returns the position of the traced particles.
  //
  // @return list<real_t> A list containing the traced particles at the current timestep.
  list<particles_t> *GetParticleTracing();
  
  /// Returns the position of the streakline particles.
  //
  // @return list<real_t> A list containing the particles of a streakline at the current timestep.
  list<particles_t> *GetStreaklines();

private:
  /// _t real_t The current timestep
  real_t _t;

  /// _dtlimit real_t The donor-cell diffusion condition (p. 27).
  ///
  /// Acts an upper limit to how large the dynamically calculated timestep
  /// can be
  real_t _dtlimit;

  /// _epslimit real_t The limit for the residual.
  real_t _epslimit;
  
  /// _diff real_t The diffusion limitation on the timestep.
  real_t _diff;
  
  /// _dt_fixed real_t The fixed timestep width to output to CSV
  real_t _dt_fixed;
  
  /// _dt_fixed real_t Inverse of _dt_fixed
  real_t _inv_dt_fixed;

  /// _u Grid The u velocities.
  Grid *_u;

  /// _v Grid The v velocities.
  Grid *_v;

  /// _p Grid The pressure.
  Grid *_p;

  /// _F Grid Preliminary velocity F, used in calculation.
  Grid *_F;

  /// _G Grid Preliminary velocity G, used in calculation.
  Grid *_G;

  /// _rhs Grid The right-hand side, used in calculation.
  Grid *_rhs;

  /// _tmp Grid A container for interpolating various values.
  Grid *_tmp;

  /// _stream Grid Contains the stream function values
  Grid *_stream;

  /// _vort Grid Contains the vorticity values
  Grid *_vort;

  /// _solver Solver The solver used for iteratively calculating the
  /// values for the next timestep.
  Solver *_solver;

  /// _geom Geometry Holds the geometry information like borders and sizes
  const Geometry *_geom;

  /// _param Parameter Holds parameter information like the reynolds number
  const Parameter *_param;
  
  /// _subst Substance Holds substance information and calculation
  const Substance *_subst;
  
  // _streakline particles_t List to hold position of particles for streaklines
  list<particles_t> _streakline;
  
  // _trace particles_t List to hold position of particles for particle tracing
  list<particles_t> _trace;

  /// Compute the new velocites u & v.
  //
  // @param dt real_t The timestep dt
  void NewVelocities(const real_t &dt);

  /// Compute the temporary velocites F & G.
  //
  // @param dt real_t The timestep dt
  void MomentumEqu(const real_t &dt);

  /// Compute the RHS of the Poisson equation.
  //
  // @param dt real_t The timestep dt
  void RHS(const real_t &dt);
  
  /// Compute the new position of a particle.
  //
  // @param particle multi_real_t The coordinates of the particle to move
  // @param dt real_t The timestep dt
  void ComputeParticleStep(multi_real_t &particle, const real_t &dt);
  
  /// Compute the new position of the streakline particles.
  //
  // @param dt real_t The timestep dt
  void ComputeStreaklines(const real_t &dt, bool addOne);
  
  /// Compute the new position of the particles for particle tracing.
  //
  // @param dt real_t The timestep dt
  void ComputeParticleTracing(const real_t &dt, bool addOne);
  
  /// Returns boolean, whether given particle lays inside the simulated area
  ///
  /// @param multi_real_t The particle to be checked
  /// @return bool Tells whether the given particle lays inside the simulated area
  bool IsValidParticle(multi_real_t &particle);
};
//------------------------------------------------------------------------------
#endif // __COMPUTE_HPP

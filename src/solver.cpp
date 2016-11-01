#include "typedef.hpp"
#include "solver.hpp"


/// Constructor of the abstract Solver class
Solver::Solver(const Geometry *geom)
    : _geom(geom){}

/// Destructor of the Solver Class
Solver::~Solver(){
  //TODO
}

/// Returns the residual at [it] for the pressure-Poisson equation
// @param it   position it
// @param grid current pressure values
// @param rhs  right hand side
real_t Solver::localRes(const Iterator &it, const Grid *grid, const Grid *rhs) const{
  //TODO
  return real_t(0.0);
}


/***************************************************************************
 *                                    SOR                                  *
 ***************************************************************************/

/// Constructs an actual SOR solver
SOR::SOR(const Geometry *geom, const real_t &omega)
    : Solver(geom), _omega(omega){}

/// Destructor
SOR::~SOR(){
  //TODO
}

/// Returns the total residual and executes a solver cycle
// @param grid current pressure values
// @param rhs right hand side
real_t SOR::Cycle(Grid *grid, const Grid *rhs) const{
  //TODO
  return real_t(0.0);
}
#include "typedef.hpp"
#include "solver.hpp"
#include "geometry.hpp"
#include "grid.hpp"
#include "iterator.hpp"

#include <cmath>

using namespace std;

/// Constructor of the abstract Solver class
Solver::Solver(const Geometry *geom)
    : _geom(geom){
      _hsquare =       (pow(_geom->Mesh()[0],2.0) * pow(_geom->Mesh()[1],2.0))
                 / ( 2*(pow(_geom->Mesh()[0],2.0) + pow(_geom->Mesh()[1],2.0)) );
    }

/// Destructor of the Solver Class
Solver::~Solver(){
}

/// Returns the residual at [it] for the pressure-Poisson equation
// @param it   position it
// @param grid current pressure values
// @param rhs  right hand side
real_t Solver::localRes(const Iterator &it, const Grid *grid, const Grid *rhs) const{
  return _hsquare * (
                      ( grid->Cell(it.Left()) + grid->Cell(it.Right()) )/pow(_geom->Mesh()[0],2.0)
                     +( grid->Cell(it.Down()) + grid->Cell(it.Top())   )/pow(_geom->Mesh()[1],2.0)
                     -( grid->Cell(it)/_hsquare )
                     - rhs->Cell(it)
                    );
}


/***************************************************************************
 *                                    SOR                                  *
 ***************************************************************************/

/// Constructs an actual SOR solver
SOR::SOR(const Geometry *geom, const real_t &omega)
    : Solver(geom), _omega(omega){
      //TODO choose omega mesh width specific -> Script
      printf("Omega given: %f, omega computed: %f, %f\n", _omega, 2/(1+sin(M_PI*_geom->Mesh()[0])),
                                                                  2/(1+sin(M_PI*_geom->Mesh()[1]))
      );
    }

/// Destructor
SOR::~SOR(){
}

/// Returns the total residual and executes a solver cycle
// @param grid current pressure values
// @param rhs right hand side
real_t SOR::Cycle(Grid *grid, const Grid *rhs) const{
  
  InteriorIterator init(_geom);
  
  real_t  totalRes(0.0);
  index_t n_avg(0);
  
  for(init.First(); init.Valid(); init.Next()){
    real_t localRes = this->localRes(init, grid, rhs);
    grid->Cell(init) = grid->Cell(init) + _omega * _hsquare * localRes;
    
    // Compute total residual
    totalRes += pow(localRes,2.0);
    n_avg    += 1;
  }
  
  return sqrt(totalRes)/n_avg;
}
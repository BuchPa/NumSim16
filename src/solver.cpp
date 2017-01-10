#include "typedef.hpp"
#include "solver.hpp"
#include "geometry.hpp"
#include "grid.hpp"
#include "iterator.hpp"

#include <cmath>

using namespace std;

Solver::Solver(const Geometry *geom)
    : _geom(geom){
      _hsquare =         (pow(_geom->Mesh()[0],2.0) * pow(_geom->Mesh()[1],2.0))
                 / ( 2.0*(pow(_geom->Mesh()[0],2.0) + pow(_geom->Mesh()[1],2.0)) );
      _ihsquare = 1.0 / _hsquare;

      _sh_ism0 = pow(_geom->Mesh()[0], 2.0);
      _sh_ism1 = pow(_geom->Mesh()[1], 2.0);
    }

Solver::~Solver(){
}

real_t Solver::localRes(const Iterator &it, const Grid *grid, const Grid *rhs) const{
  return (
    (grid->Cell(it.Left()) + grid->Cell(it.Right())) * _sh_ism0
    + (grid->Cell(it.Down()) + grid->Cell(it.Top())) * _sh_ism1
    - grid->Cell(it) * _ihsquare
    - rhs->Cell(it)
  );
}


/***************************************************************************
 *                                    SOR                                  *
 ***************************************************************************/

SOR::SOR(const Geometry *geom, const real_t &omega)
    : Solver(geom), _omega(omega){
      /// @TODO choose omega mesh width specific -> Script
      printf("Omega given: %f, omega computed: %f, %f\n", _omega, 2/(1+sin(M_PI*_geom->Mesh()[0])),
                                                                  2/(1+sin(M_PI*_geom->Mesh()[1]))
      );
    }

SOR::~SOR(){
}

real_t SOR::Cycle(Grid *grid, const Grid *rhs) const {
  InteriorIterator init(_geom);
  
  real_t  totalRes(0.0);
  index_t n_avg(0);
  
  for (init.First(); init.Valid(); init.Next()) {
    // Skip obstacles
    if (_geom->CellTypeAt(init) != CellType::Fluid)
      continue;
    
    real_t localRes = this->localRes(init, grid, rhs);
    grid->Cell(init) = grid->Cell(init) + _omega * _hsquare * localRes;
    
    // Compute total residual
    totalRes += pow(localRes, 2.0);
    n_avg    += 1;
  }
  
  return sqrt(totalRes / n_avg);
}
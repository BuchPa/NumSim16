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
    }

Solver::~Solver(){
}

real_t Solver::localRes(const Iterator &it, const Grid *grid, const Grid *rhs) const{
  return (
            ( grid->Cell(it.Left()) + grid->Cell(it.Right()) )/pow(_geom->Mesh()[0],2.0)
            +( grid->Cell(it.Down()) + grid->Cell(it.Top())   )/pow(_geom->Mesh()[1],2.0)
            -( grid->Cell(it)/_hsquare )
            - rhs->Cell(it)
          );
}


/***************************************************************************
 *                                    SOR                                  *
 ***************************************************************************/

SOR::SOR(const Geometry *geom, const real_t &omega)
    : Solver(geom), _omega(omega){
      /// @TODO choose omega mesh width specific -> Script
      mprintf("Omega given: %f, omega computed: %f, %f\n", _omega, 2/(1+sin(M_PI*_geom->Mesh()[0])),
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
    real_t localRes = this->localRes(init, grid, rhs);
    grid->Cell(init) = grid->Cell(init) + _omega * _hsquare * localRes;
    
    // Compute total residual
    totalRes += pow(localRes, 2.0);
    n_avg    += 1;
  }
  
  return sqrt(totalRes / n_avg);
}

/***************************************************************************
 *                               RedOrBlackSOR                             *
 ***************************************************************************/

RedOrBlackSOR::RedOrBlackSOR (const Geometry *geom, const real_t &omega)
  : SOR(geom, omega){
    mprintf("Omega given: %f, omega computed: %f, %f\n", _omega, 2/(1+sin(M_PI*_geom->Mesh()[0])),
                                                                  2/(1+sin(M_PI*_geom->Mesh()[1]))
      ); 
  }

RedOrBlackSOR::~RedOrBlackSOR(){
 }

real_t RedOrBlackSOR::RedCycle (Grid* grid, const Grid* rhs) const{
  InteriorIterator init(_geom);
  
  real_t totalRes(0.0);
  index_t n_avg(0);
  
  if (_geom->Size()[0] % 2 == 0) { // check if size is even
    while (init.Valid()) {
      n_avg++;
      real_t localRes = this->localRes(init, grid, rhs);
      grid->Cell(init) = grid->Cell(init) + _omega * _hsquare * localRes;

      totalRes += pow(localRes, 2.0);

      if (init.Pos()[0] + 2 == _geom->Size()[0]) { // if last red cell in the row is not at boundary jump three further
        init.Next();
        init.Next();
        init.Next();
      } else {
        if (init.Pos()[0] + 1 == _geom->Size()[0]) { // if last red cell in the row is at boundary jump one further
          init.Next();
        } else { // inner red cells, jump two further
          init.Next();
          init.Next();
        }
      }
    }
  } else { // odd celll number, just alternate black and red
    while (init.Valid()) {
      n_avg++;
      real_t localRes = this->localRes(init, grid, rhs);
      grid->Cell(init) = grid->Cell(init) + _omega * _hsquare * localRes;
      
      totalRes += pow(localRes, 2.0);

      init.Next();
      init.Next();
    }
  }

  return sqrt(totalRes / n_avg);
}

real_t RedOrBlackSOR::BlackCycle (Grid* grid, const Grid* rhs) const{
  InteriorIterator init(_geom);

  real_t  totalRes(0.0);
  index_t n_avg(0);
  init.Next(); // first black cell is second in the row
  
  if (_geom->Size()[0] % 2 == 0) { // check if size is even
    while (init.Valid()) {
      n_avg++;
      real_t localRes = this->localRes(init, grid, rhs);
      grid->Cell(init) = grid->Cell(init) + _omega * _hsquare * localRes;

      totalRes += pow(localRes, 2.0);

      if (init.Pos()[0] + 2 == _geom->Size()[0]) { // if last red cell in the row is not at boundary jump three further
        init.Next();
        init.Next();
        init.Next();
      } else {
        if (init.Pos()[0] + 1 == _geom->Size()[0]) { //if last red cell in the row is at boundary jump one further
          init.Next();
        } else { // inner red cells, jump two further
          init.Next();
          init.Next();
        }
      }
    }
  } else { // odd celll number, just alternate black and red
    while (init.Valid()) {
      n_avg++;
      real_t localRes = this->localRes(init, grid, rhs);
      grid->Cell(init) = grid->Cell(init) + _omega * _hsquare * localRes;

      totalRes += pow(localRes, 2.0);

      init.Next();
      init.Next();
    }
  }

  return sqrt(totalRes / n_avg);
}


#include "typedef.hpp"
#include "iterator.hpp"
#include "geometry.hpp"
#include "grid.hpp"
//------------------------------------------------------------------------------
#ifndef __SUBSTANCE_HPP
#define __SUBSTANCE_HPP
//------------------------------------------------------------------------------
class Substance {
public:
  /// Constructs a variable substance class
  Substance(const Geometry *geom);

  /// Desctructor.
  ~Substance();
  
  /// Default init substace
  ///
  void DefaultInit();

  /// Loads values for the substance from a file.
  ///
  ///  @param file char* File path as char array
  void Load(const char *file);
  
  /// Returns the pointer to substance n_subst.
  ///
  /// @return Grid The grid containing the substance concentration n_subst.
  const Grid *GetC(const index_t n_subst) const;

  /// Returns the substance diffusion coefficient with the
  /// given index. Indizes are numbered starting with 0.
  ///
  /// @param idx index_t Which substance
  /// @return real_t The diffusion coefficient of the substance
  real_t D(const index_t idx) const;

  /// Returns the number of substances.
  ///
  /// @return index_t The number of substances.
  const index_t &N() const;
  
  /// Compute the new substance concentration as defined by the convection-
  /// diffusion-reaction equation.
  ///
  /// @param dt real_t The timestep dt
  /// @param u real_t The velocity u
  /// @param v real_t The velocity v
  void NewConcentrations(const real_t &dt, const Grid *u, const Grid *v) const;

private:
  /// _n index_t The number of substances
  index_t _n;
  
  /// _concentration real_t The concentration boundary values for the boundaries
  real_t _concentration;
  
  /// _geom Geometry Holds the geometry information like borders and sizes
  const Geometry *_geom;
  
  /// _r real_t Diffusion coefficients
  real_t *_d;

  /// _r real_t Reaction coefficients
  real_t **_r;

  /// _rt real_t Reaction terms (used in synchronous calculation)
  real_t *_rt;

  /// _l real_t Reaction limits (population limit)
  real_t *_l;
  
  /// _k real_t Kill coefficients (-> Grey-Scott)
  real_t _k;
  
  /// _f real_t Feed coefficients (-> Grey-Scott)
  real_t _f;
  
  /// _useGS bool Flag, whether to use the Grey-Scott terms (n=2 only)
  bool _useGS;
  
  /// _gamma real_t DonorCell parameters
  real_t *_gamma;
  
  /// _c Grid Array holding all Grid instances of substances
  Grid **_c;
  
  /// Updates the concentration field c at the boundaries by applying the
  /// boundary values to them.
  ///
  /// @param c Grid The concentration field c
  void Update_C(Grid *c) const;
  
  /// Cycle the full boundary given by BoundaryIterator boit on Grid c
  ///
  /// @param c Grid The concentration field c
  /// @param boit BoundaryIterator Boundary to iterate until boit.Valid() equals false
  void CycleBoundary_C(Grid *c, BoundaryIterator boit) const;
  /// 
  /// @param c Grid The concentration field c
  /// @param boit BoundaryIterator Position to set boundary condition
  /// @param value real_t Value to set as boundary condition
  void SetCDirichlet(Grid *c, const BoundaryIterator &boit, const real_t &value) const;
  
  /// Sets Neumann boundary condition for Grid u on boundary boit.Boundary() at position boit to value
  /// 
  /// @param c Grid The concentration field c
  /// @param boit BoundaryIterator Position to set boundary condition
  /// @param value real_t Value to set as boundary condition
  void SetCNeumann(Grid *c, const BoundaryIterator &boit, const real_t &value) const;
  
  /// Init substance with a circle at the position center with radius r.
  /// 
  /// @param c Grid The concentration field c
  /// @param center multi_real_t The circle center
  /// @param radius real_t The radius of the circle
  /// @param val real_t Value to set inside the circle. Default: 1.0
  void InitCircle(Grid *c, const multi_real_t center, const real_t radius, const real_t val=1.0) const;
  
  /// Init substance with a square at the position r.
  /// 
  /// @param c Grid The concentration field c
  /// @param center multi_real_t The square center
  /// @param width real_t The width of the square
  /// @param height real_t The height of the square
  /// @param val real_t Value to set inside the square. Default: 1.0
  void InitSquare(Grid *c, const multi_real_t center, const real_t width, const real_t height, const real_t val) const;
};
//------------------------------------------------------------------------------
#endif // __SUBSTANCE_HPP

#include <iostream>

int main() {
  int _n = 2;

  double **_r;
  double *_c;
  double *_rt;
  double *_l;

  // Gedämpft: r: -0.13 0.016 -0.07 0.1 und l: 2.0 10.0
  // Schwingung: r: -0.13 0.019 -0.08 0.1 und l: 2.0 10.0
  // Explodiert: r: -0.11 0.018 -0.08 0.1 und l: 2.0 10.0
  // Jeweils c[0]=0.4 und c[1] = 1.3 bei t=0

  _r        = new double*[_n];
  _r[0]     = new double[_n];
  _r[1]     = new double[_n];
  _r[0][0]  = -0.11;
  _r[0][1]  = 0.018;
  _r[1][0]  = -0.08;
  _r[1][1]  = 0.1;

  _rt        = new double[_n];
  _rt[0]     = double(0.0);
  _rt[1]     = double(0.0);

  _c = new double[_n];
  _c[0] = 0.4;
  _c[1] = 1.3;

  _l = new double[_n];
  _l[0] = 2.0;
  _l[1] = 10.0;

  double dt = 0.05;

  for (double t = 0.0; t < 500.0; t += dt) {
    // Calculate inter-dependant reaction terms first, since they will change
    // during calculation
    for (int self=0; self < _n; self++) {
      _rt[self] = double(0.0);
      for (int other=0; other < _n; other++) {
        if (self != other) {
          _rt[self] += _r[self][other] * _c[self] * _c[other];
        }
      }
    }

    // Cycle concentrations
    for (int self=0; self<_n; self++) {
      _c[self] =
        // previous value
        _c[self]
        // quadratic reaction term (self-dependent only)
        + _r[self][self] * dt * _c[self] * (_l[self] - _c[self]) / _l[self]
        // inter-dependent reaction terms (calculated above)
        + _rt[self] * dt;
    }

    printf("%lf %lf %lf\n", t, _c[0], _c[1]);
  }

  for (int i=0; i<_n; ++i){
    delete[] _r[i];
  }
  delete[] _c;
  delete[] _r;

  delete[] _l;
  delete[] _rt;

  return 0;
}
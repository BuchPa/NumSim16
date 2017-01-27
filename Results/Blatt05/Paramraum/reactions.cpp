#include <iostream>
#include <cmath> // isnan()
#include "renderer.hpp"

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

  double dt = 0.1;
  double tend = 0.0;

  /*double r00l = 0.1,
    r00u = 0.13,
    r01l = 0.016,
    r01u = 0.018;*/
  double r00l = -15.245,
    r00u = 15.475,
    r01l = -0.8272,
    r01u = 1.536;

  int N = 300;
  double h1 = (r00u - r00l) / N;
  double h2 = (r01u - r01l) / N;

  double **_points = new double*[N];
  for (int k = 0; k < N; k++) {
    _points[k] = new double[N];
  }

  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      _r[0][0] = -(r00l + i * h1);
      _r[0][1] = r01l + j * h2;
      _c[0] = 0.4;
      _c[1] = 1.3;
      tend = 0.0;

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

        if (fabs(_c[0]) > 1000.0 || isnan(_c[0]) || fabs(_c[1]) > 1000.0 || isnan(_c[1])) {
          tend = t;
          break;
        }

        tend = t;
      }

      _points[i][j] = tend;
    }
  }

  Renderer r = Renderer();
  int S = 2;
  r.Init(N*S, N*S);

  double min = 500.0, max = 0.0;
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      if (_points[i][j] > max) {
        max = _points[i][j];
      }
      if (_points[i][j] < min) {
        min = _points[i][j];
      }
    }
  }

  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      for (int k = 0; k < S; k++) {
        for (int l = 0; l < S; l++) {
          int val = std::min(255, std::max(0, (int)floor(255 * (_points[i][j] - min) / max)));
          r.SetPixelRGB(i*S+k, j*S+l, val, val, val);
        }
      }
    }
  }

  r.Render();
  
  bool running = true;
  SDL_Event event;

  while (running) {
    SDL_Delay(30);

    if (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
      }
    }
  }


  for (int i=0; i<_n; ++i){
    delete[] _r[i];
  }
  delete[] _c;
  delete[] _r;

  delete[] _l;
  delete[] _rt;

  for (int i=0; i < N; i++) {
    delete[] _points[i];
  }
  delete[] _points;

  return 0;
}
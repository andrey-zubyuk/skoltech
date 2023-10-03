# Source of Particles (SkolTech Exercise)

## Mathematical Problem

This project implements algorithms to find a tensor $` X `$ from the equation
$$\sum_{x=0}^{x_\max} \sum_{y=0}^{y_\max} \sum_{t_s=0}^{t_{s\max}} A_{x y t_d t_s} X_{x y t_s} = n_{t_d}, \quad t_d = 0, \ldots, t_{d\max},$$
where

* $` X_{x y t_s} `$ &mdash; number of particles emitted from a point $` (x, y) `$ at time $` t_s `$,
* $` A_{x y t_d t_s} `$ &mdash; fraction of those particles transported to some fixed point &mdash; a detector &mdash; and detected there at time $` t_d `$,
* $` n_{t_d} `$ &mdash; total number of particles detected at time $` t_d `$.

Note that this problem can have multiple (and even infinitely many) solutions. The one that will be selected depends on a method used to solve the problem.

## Interface

The following function which solves the mathematical problem above is defined in the project:

    #include "solver.h"
    
    void solve(
        // Input parameters
        const std::vector<size_t> &shape,
        const double *A,
        const double *n,
        int method,
        
        // Output parameters
        void *X,
        void *extra
    )

The parameters are as follows:

* `shape` &mdash; a vector of values $` x_\max `$, $` y_\max `$, $` t_{d\max} `$, $` t_{s\max} `$,
* `A` &mdash; an array of elements of the tensor $` A `$,
* `n` &mdash; an array of elements of the tensor $` n `$,
* `method` &mdash; a constant specifying a method that shall be used to solve the mathematical problem, see details in [the section below](#implemented-methods),
* `X` &mdash; a pointer to memory where the results shall be stored, see details in [the section below](#implemented-methods),
* `extra` &mdash; a pointer to memory where , if not NULL, see details in [the section below](#implemented-methods).

## Implemented Methods

Currently the only method implemented is suitable for the case of a static point source of particles: $$ X_{xyt} = \begin{cases} c \end{cases} $$ if $` t \geq T,\ x= x_s,\ y= y_s `$, $` X_{xyt} = 0 `$ otherwise, where

* $` c `$ &mdash; source intensity,
* $` T `$ &mdash; emission start time,
* $` x_s, y_s `$ &mdash; a point where the source is localized.

In this case the problem is mathematically reformulated as a problem of residual sum of squares (RSS) minimization:
$$\text{RSS}(c, T, x, y) = \sum_{t_d=0}^{t_{d\max}}\left( n_{t_d} - c \sum_{t_s=T}^{t_d} A_{x y t_d t_s} \right)^2 \sim \min_{c, T, x, y}.$$

The value of the parameter `method` corresponding to this method is `STATIC_SOURCE_MIN_MSE`. The parameter `X` shall be a pointer to the structure `StaticSource` defined in `solver.h` as follows:

    struct StaticSource {
        int x, y, T;
        double intensity;
    }

If not NULL, the parameter `extra` shall be a pointer to a 2d-array of size $` (x_\max + 1) \times (y_\max + 1) `$ to store the values $` \min_{c, T}\text{RSS}(c, T, x, y) `$.

## Build

## Run

/****************************************************************************
 *   Copyright (C) 2011 by Bernd Flemisch                                    *
 *   Institute of Hydraulic Engineering                                      *
 *   University of Stuttgart, Germany                                        *
 *   email: <givenname>.<name>@iws.uni-stuttgart.de                          *
 *                                                                           *
 *   This program is free software: you can redistribute it and/or modify    *
 *   it under the terms of the GNU General Public License as published by    *
 *   the Free Software Foundation, either version 2 of the License, or       *
 *   (at your option) any later version.                                     *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 *   GNU General Public License for more details.                            *
 *                                                                           *
 *   You should have received a copy of the GNU General Public License       *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 *****************************************************************************/
/*!
 * \file
 * \brief Dumux solver backend
 */
#ifndef DUMUX_SOLVER_BACKEND_HH
#define DUMUX_SOLVER_BACKEND_HH

#include <dune/istl/solvers.hh>
#include <dune/istl/superlu.hh>

#include <dumux/linear/linearsolverproperties.hh>

namespace Dumux
{

template <class TypeTag>
class ILU0BiCGSTABBackend
{
  typedef typename GET_PROP_TYPE(TypeTag, PTAG(Problem)) Problem;
public:

  ILU0BiCGSTABBackend(Problem& problem)
  {}

  template<class Matrix, class Vector>
  void solve(const Matrix& A, Vector& x, const Vector& b)
  {
    int verbosity = GET_PROP_VALUE(TypeTag, PTAG(LSVerbosity));
    static const int maxIter = GET_PROP_VALUE(TypeTag, PTAG(LSMaxIterations));
    static const double residReduction = GET_PROP_VALUE(TypeTag, PTAG(LSResidualReduction));

    Vector bTmp(b);

    typedef Dune::SeqILU0<Matrix, Vector, Vector> Preconditioner;
    static const double relaxation = GET_PROP_VALUE(TypeTag, PTAG(PreconditionerRelaxation));
    Preconditioner precond(A, relaxation);

    typedef Dune::MatrixAdapter<Matrix, Vector, Vector> MatrixAdapter;
    MatrixAdapter operatorA(A);

    typedef Dune::BiCGSTABSolver<Vector> Solver;
    Solver solver(operatorA, precond, residReduction, maxIter, verbosity);

    solver.apply(x, bTmp, result_);
  }

  const Dune::InverseOperatorResult& result() const
  {
    return result_;
  }

private:
  Dune::InverseOperatorResult result_;
};

template <class TypeTag>
class ILU0CGBackend
{
  typedef typename GET_PROP_TYPE(TypeTag, PTAG(Problem)) Problem;
public:

  ILU0CGBackend(Problem& problem)
  {}

  template<class Matrix, class Vector>
  void solve(const Matrix& A, Vector& x, const Vector& b)
  {
    int verbosity = GET_PROP_VALUE(TypeTag, PTAG(LSVerbosity));
    static const int maxIter = GET_PROP_VALUE(TypeTag, PTAG(LSMaxIterations));
    static const double residReduction = GET_PROP_VALUE(TypeTag, PTAG(LSResidualReduction));

    Vector bTmp(b);

    typedef Dune::SeqILU0<Matrix, Vector, Vector> Preconditioner;
    static const double relaxation = GET_PROP_VALUE(TypeTag, PTAG(PreconditionerRelaxation));
    Preconditioner precond(A, relaxation);

    typedef Dune::MatrixAdapter<Matrix, Vector, Vector> MatrixAdapter;
    MatrixAdapter operatorA(A);

    typedef Dune::CGSolver<Vector> Solver;
    Solver solver(operatorA, precond, residReduction, maxIter, verbosity);

    solver.apply(x, bTmp, result_);
  }

  const Dune::InverseOperatorResult& result() const
  {
    return result_;
  }

private:
  Dune::InverseOperatorResult result_;
};

template <class TypeTag>
class IterativePrecondSolverBackend
{
public:

  IterativePrecondSolverBackend()
  {}

  template<class Preconditioner, class Solver, class Matrix, class Vector>
  void solve(const Matrix& A, Vector& x, const Vector& b)
  {
    int verbosity = GET_PROP_VALUE(TypeTag, PTAG(LSVerbosity));
    static const int maxIter = GET_PROP_VALUE(TypeTag, PTAG(LSMaxIterations));
    static const double residReduction = GET_PROP_VALUE(TypeTag, PTAG(LSResidualReduction));

    Vector bTmp(b);

    static const double relaxation = GET_PROP_VALUE(TypeTag, PTAG(PreconditionerRelaxation));
    static const int precondIter = GET_PROP_VALUE(TypeTag, PTAG(PreconditionerIterations));

    Preconditioner precond(A, precondIter, relaxation);

    typedef Dune::MatrixAdapter<Matrix, Vector, Vector> MatrixAdapter;
    MatrixAdapter operatorA(A);

    Solver solver(operatorA, precond, residReduction, maxIter, verbosity);

    solver.apply(x, bTmp, result_);
  }

  const Dune::InverseOperatorResult& result() const
  {
    return result_;
  }

private:
  Dune::InverseOperatorResult result_;
};

template <class TypeTag>
class ILUnBiCGSTABBackend: public IterativePrecondSolverBackend<TypeTag>
{
  typedef typename GET_PROP_TYPE(TypeTag, PTAG(Problem)) Problem;
  typedef IterativePrecondSolverBackend<TypeTag> ParentType;
public:

  ILUnBiCGSTABBackend(Problem& problem)
  {}

  template<class Matrix, class Vector>
  void solve(const Matrix& A, Vector& x, const Vector& b)
  {
    typedef Dune::SeqILUn<Matrix, Vector, Vector> Preconditioner;
    typedef Dune::BiCGSTABSolver<Vector> Solver;

    ParentType::template solve<Preconditioner, Solver>(A, x, b);
  }
};

template <class TypeTag>
class SORBiCGSTABBackend: public IterativePrecondSolverBackend<TypeTag>
{
  typedef typename GET_PROP_TYPE(TypeTag, PTAG(Problem)) Problem;
  typedef IterativePrecondSolverBackend<TypeTag> ParentType;
public:

  SORBiCGSTABBackend(Problem& problem)
  {}

  template<class Matrix, class Vector>
  void solve(const Matrix& A, Vector& x, const Vector& b)
  {
    typedef Dune::SeqSOR<Matrix, Vector, Vector> Preconditioner;
    typedef Dune::BiCGSTABSolver<Vector> Solver;

    ParentType::template solve<Preconditioner, Solver>(A, x, b);
  }
};

template <class TypeTag>
class SSORBiCGSTABBackend: public IterativePrecondSolverBackend<TypeTag>
{
  typedef typename GET_PROP_TYPE(TypeTag, PTAG(Problem)) Problem;
  typedef IterativePrecondSolverBackend<TypeTag> ParentType;
public:

  SSORBiCGSTABBackend(Problem& problem)
  {}

  template<class Matrix, class Vector>
  void solve(const Matrix& A, Vector& x, const Vector& b)
  {
    typedef Dune::SeqSSOR<Matrix, Vector, Vector> Preconditioner;
    typedef Dune::BiCGSTABSolver<Vector> Solver;

    ParentType::template solve<Preconditioner, Solver>(A, x, b);
  }
};

template <class TypeTag>
class GSBiCGSTABBackend: public IterativePrecondSolverBackend<TypeTag>
{
  typedef typename GET_PROP_TYPE(TypeTag, PTAG(Problem)) Problem;
  typedef IterativePrecondSolverBackend<TypeTag> ParentType;
public:

  GSBiCGSTABBackend(Problem& problem)
  {}

  template<class Matrix, class Vector>
  void solve(const Matrix& A, Vector& x, const Vector& b)
  {
    typedef Dune::SeqGS<Matrix, Vector, Vector> Preconditioner;
    typedef Dune::BiCGSTABSolver<Vector> Solver;

    ParentType::template solve<Preconditioner, Solver>(A, x, b);
  }
};

template <class TypeTag>
class JacBiCGSTABBackend: public IterativePrecondSolverBackend<TypeTag>
{
  typedef typename GET_PROP_TYPE(TypeTag, PTAG(Problem)) Problem;
  typedef IterativePrecondSolverBackend<TypeTag> ParentType;
public:

  JacBiCGSTABBackend(Problem& problem)
  {}

  template<class Matrix, class Vector>
  void solve(const Matrix& A, Vector& x, const Vector& b)
  {
    typedef Dune::SeqJac<Matrix, Vector, Vector> Preconditioner;
    typedef Dune::BiCGSTABSolver<Vector> Solver;

    ParentType::template solve<Preconditioner, Solver>(A, x, b);
  }
};

template <class TypeTag>
class ILUnCGBackend: public IterativePrecondSolverBackend<TypeTag>
{
  typedef typename GET_PROP_TYPE(TypeTag, PTAG(Problem)) Problem;
  typedef IterativePrecondSolverBackend<TypeTag> ParentType;
public:

  ILUnCGBackend(Problem& problem)
  {}

  template<class Matrix, class Vector>
  void solve(const Matrix& A, Vector& x, const Vector& b)
  {
    typedef Dune::SeqILUn<Matrix, Vector, Vector> Preconditioner;
    typedef Dune::CGSolver<Vector> Solver;

    ParentType::template solve<Preconditioner, Solver>(A, x, b);
  }
};

template <class TypeTag>
class SORCGBackend: public IterativePrecondSolverBackend<TypeTag>
{
  typedef typename GET_PROP_TYPE(TypeTag, PTAG(Problem)) Problem;
  typedef IterativePrecondSolverBackend<TypeTag> ParentType;
public:

  SORCGBackend(Problem& problem)
  {}

  template<class Matrix, class Vector>
  void solve(const Matrix& A, Vector& x, const Vector& b)
  {
    typedef Dune::SeqSOR<Matrix, Vector, Vector> Preconditioner;
    typedef Dune::CGSolver<Vector> Solver;

    ParentType::template solve<Preconditioner, Solver>(A, x, b);
  }
};

template <class TypeTag>
class SSORCGBackend: public IterativePrecondSolverBackend<TypeTag>
{
  typedef typename GET_PROP_TYPE(TypeTag, PTAG(Problem)) Problem;
  typedef IterativePrecondSolverBackend<TypeTag> ParentType;
public:

  SSORCGBackend(Problem& problem)
  {}

  template<class Matrix, class Vector>
  void solve(const Matrix& A, Vector& x, const Vector& b)
  {
    typedef Dune::SeqSSOR<Matrix, Vector, Vector> Preconditioner;
    typedef Dune::CGSolver<Vector> Solver;

    ParentType::template solve<Preconditioner, Solver>(A, x, b);
  }
};

template <class TypeTag>
class GSCGBackend: public IterativePrecondSolverBackend<TypeTag>
{
  typedef typename GET_PROP_TYPE(TypeTag, PTAG(Problem)) Problem;
  typedef IterativePrecondSolverBackend<TypeTag> ParentType;
public:

  GSCGBackend(Problem& problem)
  {}

  template<class Matrix, class Vector>
  void solve(const Matrix& A, Vector& x, const Vector& b)
  {
    typedef Dune::SeqGS<Matrix, Vector, Vector> Preconditioner;
    typedef Dune::CGSolver<Vector> Solver;

    ParentType::template solve<Preconditioner, Solver>(A, x, b);
  }
};

template <class TypeTag>
class JacCGBackend: public IterativePrecondSolverBackend<TypeTag>
{
  typedef typename GET_PROP_TYPE(TypeTag, PTAG(Problem)) Problem;
  typedef IterativePrecondSolverBackend<TypeTag> ParentType;
public:

  JacCGBackend(Problem& problem)
  {}

  template<class Matrix, class Vector>
  void solve(const Matrix& A, Vector& x, const Vector& b)
  {
    typedef Dune::SeqJac<Matrix, Vector, Vector> Preconditioner;
    typedef Dune::CGSolver<Vector> Solver;

    ParentType::template solve<Preconditioner, Solver>(A, x, b);
  }
};

#if HAVE_SUPERLU
template <class TypeTag>
class SuperLUBackend
{
  typedef typename GET_PROP_TYPE(TypeTag, PTAG(Problem)) Problem;

public:

  SuperLUBackend(const Problem& problem)
  : problem_(problem)
  {}

  template<class Matrix, class Vector>
  int solve(const Matrix& A, Vector& x, const Vector& b)
  {
    int verbosity = GET_PROP_VALUE(TypeTag, PTAG(LSVerbosity));
    static const int maxIter = GET_PROP_VALUE(TypeTag, PTAG(LSMaxIterations));
    static const double residReduction = GET_PROP_VALUE(TypeTag, PTAG(LSResidualReduction));

    Vector bTmp(b);
    Dune::SuperLU<Matrix> solver(A);

    solver.apply(x, bTmp, result_);

    return result_.converged;
  }

  const Dune::InverseOperatorResult& result() const
  {
    return result_;
  }

private:
  Dune::InverseOperatorResult result_;
  const Problem& problem_;
};
#endif

}
#endif

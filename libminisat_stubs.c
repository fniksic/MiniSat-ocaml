
#define __STDC_LIMIT_MACROS
#define __STDC_FORMAT_MACROS
#include <minisat2/Solver.h>

using namespace Minisat;

extern "C"
{
#include <caml/mlvalues.h>
#include <caml/memory.h>
#include <caml/alloc.h>
#include <caml/custom.h>
#include <caml/fail.h>
}

/* Declaring the functions which should be accessible on the C side. */
extern "C" 
{
  CAMLprim value minisat_new(value solver);
  CAMLprim value minisat_del(value solver);
  CAMLprim value minisat_new_var(value solver);
  CAMLprim value minisat_pos_lit(value v);
  CAMLprim value minisat_neg_lit(value v);
  CAMLprim value minisat_add_clause(value solver, value c);
  CAMLprim value minisat_simplify(value solver);
  CAMLprim value minisat_conflict(value solver);
  CAMLprim value minisat_model(value solver);
  CAMLprim value minisat_solve(value solver);
  CAMLprim value minisat_solve_with_assumption(value solver, value a);
  CAMLprim value minisat_value_of(value solver, value v);
}

#define Val_none Val_int(0)

static inline value Val_var( lbool v ) { 
  CAMLlocal1( result );

  if(v == l_False) {
    result = Val_int(0);
  } else if(v == l_True) {
    result = Val_int(1);
  } else if (v == l_Undef) {
    result = Val_int(2);
  } else {
    assert(0);
  }

  return result;
}

static inline value Val_some( value v ) {
  CAMLparam1( v );
  CAMLlocal1( some );
  some = caml_alloc(1, 0);
  Store_field( some, 0, v );
  CAMLreturn(some) ;
}

static inline value tuple( value a, value b) {
  CAMLparam2( a, b );
  CAMLlocal1( tuple );

  tuple = caml_alloc(2, 0);

  Store_field( tuple, 0, a );
  Store_field( tuple, 1, b );

  CAMLreturn(tuple);
}

static inline value append( value hd, value tl ) {
  CAMLparam2( hd , tl );
  CAMLreturn(tuple( hd, tl ));
}

static inline void convert_literals(value l, vec<Lit> &r) {
  while(Int_val(l) != 0) {
    Lit lit = toLit(Int_val(Field(l, 0)));
    r.push(lit);
    l = Field(l, 1);
  }
}

CAMLprim value minisat_new(value unit) {
  CAMLparam0 ();
  CAMLlocal1 (result);
  Solver *solver = new Solver();
  result = alloc_small(1, Abstract_tag);
  Field(result, 0) = (value) solver;

  CAMLreturn(result);
}

#define solver_val(v) ((Solver*)(Field((v), 0)))

CAMLprim value minisat_del(value solver) {
  CAMLparam1 (solver);
  Solver* _solver = solver_val(solver);
  delete _solver;
  CAMLreturn(Val_unit);
}

CAMLprim value minisat_new_var(value solver) {
  CAMLparam1 (solver);
  Solver* _solver = solver_val(solver);
  return Val_int(_solver->newVar());
}

CAMLprim value minisat_pos_lit(value v) {
  Var var = Int_val(v);
  return Val_int(toInt(~mkLit(var)));
}

CAMLprim value minisat_neg_lit(value v) {
  Var var = Int_val(v);
  return Val_int(toInt(mkLit(var)));
}

CAMLprim value minisat_add_clause(value solver, value c) {
  CAMLparam2 (solver, c);
  CAMLlocal1 (result);

  Solver* _solver = solver_val(solver);

  vec<Lit> clause;
  convert_literals(c, clause);
  _solver->addClause(clause);

  result = alloc_small(1, Abstract_tag);
  Field(result, 0) = (value) _solver;

  CAMLreturn(result);
}

CAMLprim value minisat_simplify(value solver) {
  CAMLparam1 (solver);
  Solver* _solver = solver_val(solver);
  CAMLreturn(Val_bool(_solver->simplify()));
}

// If problem is unsatisfiable (possibly under assumptions), 
// this vector represent the final conflict clause expressed in the assumptions.
CAMLprim value minisat_conflict(value solver) {
  CAMLparam1 (solver);
  CAMLlocal2 ( hd, tl );
  tl = Val_emptylist;

  Solver* _solver = solver_val(solver);
  if (!_solver->okay()) {
    vec<Lit>& l = _solver->conflict;
    for (int i = 0; i < l.size(); i++) {
      hd = Val_int(toInt(l[i]));
      tl = append(hd,tl); 
    }
  }

  CAMLreturn(tl);
}

// If problem is satisfiable, this vector contains the model (if any).
CAMLprim value minisat_model(value solver) {
  CAMLparam1 (solver);
  CAMLlocal1(result);
  result = caml_alloc(0,0);

  Solver* _solver = solver_val(solver);
  if (_solver->okay()) {
    result = caml_alloc(_solver->nVars(), 0);
    for (int i = 0; i < _solver->nVars(); i++) {
      Store_field(result, i, Val_var(_solver->model[i]));
      //if (_solver->model[i]==l_Undef)
        fprintf(stdout,"%s%s%d!!!\n", (i==0)?"":" ", (_solver->model[i]==l_True)?"":"-", i+1);

    }
  }

  CAMLreturn(result);
}

CAMLprim value minisat_solve(value solver) {
  CAMLparam1 (solver);
  CAMLlocal1 (result);
  vec<Lit> dummy;

  Solver* _solver = solver_val(solver);

  // solveLimited returns lbool and it is l_Undef if it was
  // possible to find a solution within the allocated resources
  CAMLreturn(Val_var(_solver->solveLimited(dummy)));
}

CAMLprim value minisat_solve_with_assumption(value solver, value a) {
  CAMLparam2 (solver, a);
  CAMLlocal1 (result);
  vec<Lit> ass;

  convert_literals(a, ass);
  Solver* _solver = solver_val(solver);

  CAMLreturn(Val_var(_solver->solveLimited(ass)));
}

CAMLprim value minisat_value_of(value solver, value v) {
  CAMLparam2 (solver,v);

  Solver* _solver = solver_val(solver);
  Var var = Int_val(v);

  CAMLreturn(Val_var(_solver->value(var)));
}

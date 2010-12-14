
type minisat
type var = int
type lit
type lbool = True | False | Unknown

external create : unit -> minisat = "minisat_new"
external destroy : minisat -> unit = "minisat_del"

external new_var : minisat -> var = "minisat_new_var"

external pos_lit : var -> lit = "minisat_pos_lit"
external neg_lit : var -> lit = "minisat_neg_lit"

external add_clause : minisat -> lit list -> bool = "minisat_add_clause"

external simplify : minisat -> unit = "minisat_simplify"

external conflict : minisat -> lit list = "minisat_conflict"
external model : minisat -> lbool array = "minisat_model"

external solveLimited : minisat -> lbool = "minisat_solveLimited"
external solveLimited_with_assumption : 
  minisat -> lit list -> lbool = "minisat_solveLimited"

external solve : minisat -> bool = "minisat_solve"
external solve_with_assumption : 
  minisat -> lit list -> bool = "minisat_solve_with_assumption"

external value_of : minisat -> var -> lbool = "minisat_value_of"

class solver = object (self)
  val solver = create ()
  method new_var = new_var solver
  method add_clause l = add_clause solver l
  method simplify = simplify solver
  method conflict = conflict solver
  method model = model solver
  method solve = solve solver
  method solve_with_assumption l = solve_with_assumption solver l
  method value_of v = value_of solver v
end

let string_of_lbool = function
  |False -> "false"
  |True -> "true"
  |Unknown -> "unknown"

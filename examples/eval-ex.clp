(deffunction tcl/eval (?tcl $?arguments)
  (tcl-eval-ex ?tcl (tcl-merge ?arguments) -1 /))

(defrule main
 =>
  (bind ?tcl (tcl-create-interp))

  (println (tcl-eval-ex ?tcl "puts {Hello, world.}" -1 /))

  (println (tcl/eval ?tcl "puts" "Hello, world."))

  (tcl-delete-interp ?tcl))

(run)

(exit)

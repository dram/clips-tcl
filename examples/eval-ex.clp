(deffunction tcl/eval (?tcl $?arguments)
  (tcl-eval-ex ?tcl (tcl-merge ?arguments) /))

(defrule main
 =>
  (bind ?tcl (tcl-create-interp))

  (println (tcl-eval-ex ?tcl "puts {Hello, world.}" /))

  (println (tcl/eval ?tcl "puts" "Hello, world."))

  (tcl-delete-interp ?tcl))

(run)

(exit)

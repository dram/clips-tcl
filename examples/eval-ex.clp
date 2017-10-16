(deffunction tcl/eval (?interp $?arguments)
  (tcl-eval-ex ?interp (tcl-merge ?arguments) /))

(defrule main
 =>
  (bind ?interp (tcl-create-interp))

  (println (tcl-eval-ex ?interp "puts {Hello, world.}" /))

  (println (tcl/eval ?interp "puts" "Hello, world."))

  (tcl-delete-interp ?interp))

(run)

(exit)

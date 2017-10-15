(defrule main
 =>
  (bind ?interp (tcl-create-interp))

  (println (tcl-eval-ex ?interp "puts {Hello, world.}" /))

  (tcl-delete-interp ?interp))

(run)

(exit)

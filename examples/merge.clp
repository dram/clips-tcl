(defrule main
 =>
  (bind ?interp (tcl-create-interp))

  (println (tcl-merge (create$ "a" "b c")))

  (tcl-delete-interp ?interp))

(run)

(exit)

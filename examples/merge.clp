(defrule main
 =>
  (bind ?tcl (tcl-create-interp))

  (println (tcl-merge (create$ "a" "b c")))

  (tcl-delete-interp ?tcl))

(run)

(exit)

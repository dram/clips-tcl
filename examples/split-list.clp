(defrule main
 =>
  (bind ?tcl (tcl-create-interp))

  (println (tcl-split-list ?tcl "1 2 3"))
  (println (tcl-split-list ?tcl "1 {2 3} {4 5}"))
  (println (tcl-split-list ?tcl "1 {2 3} {4 5} {"))

  (tcl-delete-interp ?tcl))

(run)

(exit)

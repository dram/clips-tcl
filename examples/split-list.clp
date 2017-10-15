(defrule main
 =>
  (bind ?interp (tcl-create-interp))

  (println (tcl-split-list ?interp "1 2 3"))
  (println (tcl-split-list ?interp "1 {2 3} {4 5}"))
  (println (tcl-split-list ?interp "1 {2 3} {4 5} {"))

  (tcl-delete-interp ?interp))

(run)

(exit)

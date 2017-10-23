(defrule main
 =>
  (println (tcl-split-list "1 2 3"))
  (println (tcl-split-list "1 {2 3} {4 5}"))
  (println (tcl-split-list "1 {2 3} {4 5} {")))

(run)

(exit)

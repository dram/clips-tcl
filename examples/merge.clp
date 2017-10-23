(defrule main
 =>
  (println (tcl-merge (create$ "a" "b c"))))

(run)

(exit)

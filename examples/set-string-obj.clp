(defrule main
 =>
  (bind ?o (tcl-new-obj))

  (println (tcl-get-string ?o))

  (tcl-set-string-obj ?o "foo" -1)

  (println (tcl-get-string ?o)))

(run)

(exit)

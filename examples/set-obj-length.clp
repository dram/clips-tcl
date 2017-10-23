(defrule main
 =>
  (bind ?o (tcl-new-string-obj "foo" -1))

  (tcl-set-obj-length ?o 2)

  (println (tcl-get-string ?o)))

(run)

(exit)

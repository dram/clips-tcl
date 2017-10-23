(defrule main
 =>
  (bind ?o (tcl-new-string-obj "abc" -1))

  (println (tcl-is-shared ?o))

  (tcl-incr-ref-count ?o)
  (println (tcl-is-shared ?o))

  (tcl-incr-ref-count ?o)
  (println (tcl-is-shared ?o)))

(run)

(exit)

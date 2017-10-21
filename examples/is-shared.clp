(defrule main
 =>
  (bind ?tcl (tcl-create-interp))

  (bind ?o (tcl-new-string-obj "abc" -1))

  (println (tcl-is-shared ?o))

  (tcl-incr-ref-count ?o)
  (println (tcl-is-shared ?o))

  (tcl-incr-ref-count ?o)
  (println (tcl-is-shared ?o))

  (tcl-delete-interp ?tcl))

(run)

(exit)

(defrule main
 =>
  (bind ?tcl (tcl-create-interp))

  (bind ?o (tcl-new-string-obj "foo" -1))

  (tcl-set-obj-length ?o 2)

  (println (tcl-get-string ?o))

  (tcl-delete-interp ?tcl))

(run)

(exit)

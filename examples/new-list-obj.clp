(defrule main
 =>
  (bind ?tcl (tcl-create-interp))

  (bind ?o (tcl-new-list-obj (create$ (tcl-new-string-obj "a")
                                      (tcl-new-string-obj "b c"))))

  (println (tcl-get-string ?o))

  (tcl-delete-interp ?tcl))

(run)

(exit)

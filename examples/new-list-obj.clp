(defrule main
 =>
  (bind ?interp (tcl-create-interp))

  (bind ?o (tcl-new-list-obj (create$ (tcl-new-string-obj "a")
                                      (tcl-new-string-obj "b c"))))

  (println (tcl-get-string ?o))

  (tcl-delete-interp ?interp))

(run)

(exit)

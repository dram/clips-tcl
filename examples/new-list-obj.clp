(defrule main
 =>
  (bind ?o (tcl-new-list-obj (create$ (tcl-new-string-obj "a" -1)
                                      (tcl-new-string-obj "b c" -1))))

  (println (tcl-get-string ?o)))

(run)

(exit)

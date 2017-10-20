(defrule main
 =>
  (bind ?tcl (tcl-create-interp))

  (foreach ?o (tcl-list-obj-get-elements
               ?tcl (tcl-new-string-obj "1 2 3"))
    (println (tcl-get-string ?o)))
  (foreach ?o (tcl-list-obj-get-elements
               ?tcl (tcl-new-string-obj "1 {2 3} {4 5}"))
    (println (tcl-get-string ?o)))
  (println (tcl-list-obj-get-elements
            ?tcl (tcl-new-string-obj "1 {2 3} {4 5} {")))

  (tcl-delete-interp ?tcl))

(run)

(exit)

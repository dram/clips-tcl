(defrule main
 =>
  (bind ?tcl (tcl-create-interp))

  (foreach ?o (tcl-list-obj-get-elements
               ?tcl (tcl-new-string-obj "1 2 3" -1))
    (println (tcl-get-string ?o)))
  (foreach ?o (tcl-list-obj-get-elements
               ?tcl (tcl-new-string-obj "1 {2 3} {4 5}" -1))
    (println (tcl-get-string ?o)))
  (println (tcl-list-obj-get-elements
            ?tcl (tcl-new-string-obj "1 {2 3} {4 5} {" -1)))

  (tcl-delete-interp ?tcl))

(run)

(exit)

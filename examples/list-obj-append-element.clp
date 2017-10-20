(defrule main
 =>
  (bind ?tcl (tcl-create-interp))
  (bind ?list (tcl-new-obj))
  (bind ?a (tcl-new-string-obj "a"))

  (tcl-list-obj-append-element ?tcl ?list ?a)

  (println (tcl-get-string ?list))

  (foreach ?s (create$ "a" "b" "c")
    (tcl-list-obj-append-element ?tcl
                                 ?list
                                 (tcl-new-string-obj ?s)))

  (println (tcl-get-string ?list))

  (tcl-delete-interp ?tcl))

(run)

(exit)

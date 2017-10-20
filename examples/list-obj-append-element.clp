(defrule main
 =>
  (bind ?interp (tcl-create-interp))
  (bind ?list (tcl-new-obj))
  (bind ?a (tcl-new-string-obj "a"))

  (tcl-list-obj-append-element ?interp ?list ?a)

  (println (tcl-get-string ?list))

  (foreach ?s (create$ "a" "b" "c")
    (tcl-list-obj-append-element ?interp
                                 ?list
                                 (tcl-new-string-obj ?s)))

  (println (tcl-get-string ?list))

  (tcl-delete-interp ?interp))

(run)

(exit)

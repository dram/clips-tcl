(defrule main
 =>
  (bind ?list (tcl-new-obj))
  (bind ?a (tcl-new-string-obj "a" -1))

  (tcl-list-obj-append-element ?list ?a)

  (println (tcl-get-string ?list))

  (foreach ?s (create$ "a" "b" "c")
    (tcl-list-obj-append-element ?list
                                 (tcl-new-string-obj ?s -1)))

  (println (tcl-get-string ?list)))

(run)

(exit)

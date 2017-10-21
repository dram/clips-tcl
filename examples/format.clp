(defrule main
 =>
  (bind ?tcl (tcl-create-interp))

  (bind ?o (tcl-format ?tcl
                       "%5.2f %s /

%s"
                       (create$ (tcl-new-string-obj "12.3" -1)
                                (tcl-new-string-obj "a" -1)
                                (tcl-new-string-obj "b c" -1))))

  (println (tcl-get-string ?o))

  (tcl-delete-interp ?tcl))

(run)

(exit)

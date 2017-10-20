(defrule main
 =>
  (bind ?tcl (tcl-create-interp))

  (bind ?o (tcl-new-string-obj "## "))

  (tcl-append-format-to-obj ?tcl
                            ?o
                            "%5.2f %s /

%s"
                            (create$ (tcl-new-string-obj "12.3")
                                     (tcl-new-string-obj "a")
                                     (tcl-new-string-obj "b c")))

  (println (tcl-get-string ?o))

  (tcl-delete-interp ?tcl))

(run)

(exit)

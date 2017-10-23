(defrule main
 =>
  (bind ?o (tcl-new-string-obj "## " -1))

  (tcl-append-format-to-obj ?o
                            "%5.2f %s /

%s"
                            (create$ (tcl-new-string-obj "12.3" -1)
                                     (tcl-new-string-obj "a" -1)
                                     (tcl-new-string-obj "b c" -1)))

  (println (tcl-get-string ?o)))

(run)

(exit)

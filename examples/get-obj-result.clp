(defrule main
 =>
  (tcl-eval-objv (create$ (tcl-new-string-obj "expr" -1)
                          (tcl-new-string-obj "1 + 2" -1))
                 /)

  (println (tcl-get-string (tcl-get-obj-result))))

(run)

(exit)

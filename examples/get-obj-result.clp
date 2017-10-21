(defrule main
 =>
  (bind ?tcl (tcl-create-interp))

  (tcl-eval-objv ?tcl
                 (create$ (tcl-new-string-obj "expr" -1)
                          (tcl-new-string-obj "1 + 2" -1))
                 /)

  (println (tcl-get-string (tcl-get-obj-result ?tcl)))

  (tcl-delete-interp ?tcl))

(run)

(exit)

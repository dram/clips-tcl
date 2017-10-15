(defrule main
 =>
  (bind ?interp (tcl-create-interp))

  (tcl-eval-objv ?interp
                 (create$ (tcl-new-string-obj "expr")
                          (tcl-new-string-obj "1 + 2"))
                 /)

  (println (tcl-get-string (tcl-get-obj-result ?interp)))

  (tcl-delete-interp ?interp))

(run)

(exit)

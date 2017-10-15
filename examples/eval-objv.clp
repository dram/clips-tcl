(defrule main
 =>
  (bind ?interp (tcl-create-interp))

  (tcl-eval-objv ?interp
                 (create$ (tcl-new-string-obj "puts")
                          (tcl-new-string-obj "Hello, world."))
                 /)

  (tcl-delete-interp ?interp))

(run)

(exit)

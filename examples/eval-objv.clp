(defrule main
 =>
  (bind ?interp (tcl-create-interp))

  (println (tcl-eval-objv ?interp
                          (create$ (tcl-new-string-obj "puts")
                                   (tcl-new-string-obj "Hello, world."))
                          /))

  (tcl-delete-interp ?interp))

(run)

(exit)

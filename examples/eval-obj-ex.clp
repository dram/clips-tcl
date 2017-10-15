(defrule main
 =>
  (bind ?interp (tcl-create-interp))

  (println (tcl-eval-obj-ex ?interp
                            (tcl-new-string-obj "puts {Hello, world.}")
                            /))

  (println (tcl-eval-obj-ex ?interp
                            (tcl-new-string-obj "set i [expr {1 + 2}]")
                            /))
  (println (tcl-get-var ?interp "i" /))

  (tcl-delete-interp ?interp))

(run)

(exit)

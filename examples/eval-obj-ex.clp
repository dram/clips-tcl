(defrule main
 =>
  (bind ?interp (tcl-create-interp))

  (tcl-eval-obj-ex ?interp
                   (tcl-new-string-obj "puts {Hello, world.}")
                   (create$))

  (tcl-eval-obj-ex ?interp
                   (tcl-new-string-obj "set i [expr {1 + 2}]")
                   (create$))
  (println (tcl-get-var ?interp "i" (create$)))

  (tcl-delete-interp ?interp))

(run)

(exit)

(defrule main
 =>
  (bind ?interp (tcl-create-interp))

  (bind ?c (tcl-get-std-channel /stdout/))
  (tcl-write-obj ?c (tcl-new-string-obj (format nil "hello%n")))
  (tcl-close ?interp ?c)

  (tcl-delete-interp ?interp))

(run)

(exit)

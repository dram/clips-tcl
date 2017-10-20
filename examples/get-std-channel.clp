(defrule main
 =>
  (bind ?tcl (tcl-create-interp))

  (bind ?c (tcl-get-std-channel /stdout/))
  (tcl-write-obj ?c (tcl-new-string-obj (format nil "hello%n")))
  (tcl-close ?tcl ?c)

  (tcl-delete-interp ?tcl))

(run)

(exit)

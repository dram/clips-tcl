(defrule main
 =>
  (bind ?c (tcl-get-std-channel /stdout/))
  (tcl-write-obj ?c (tcl-new-string-obj (format nil "hello%n") -1))
  (tcl-close ?c))

(run)

(exit)

(defglobal ?*interp* = (tcl-create-interp))

(defrule main
 =>
  (println (tcl-eval-obj-ex ?*interp*
                            (tcl-new-string-obj "puts {Hello, world.}")
                            /))

  (println (tcl-eval-obj-ex ?*interp*
                            (tcl-new-string-obj "set i [expr {1 + 2}]")
                            /))
  (println (tcl-get-var ?*interp* "i" /)))

(run)

(exit)

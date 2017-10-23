(defrule main
 =>
  (println (tcl-eval-obj-ex (tcl-new-string-obj "puts {Hello, world.}" -1)
                            /))

  (println (tcl-eval-obj-ex (tcl-new-string-obj "set i [expr {1 + 2}]" -1)
                            /))
  (println (tcl-get-var "i" /)))

(run)

(exit)

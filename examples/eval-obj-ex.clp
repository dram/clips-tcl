(defglobal ?*tcl* = (tcl-create-interp))

(defrule main
 =>
  (println (tcl-eval-obj-ex ?*tcl*
                            (tcl-new-string-obj "puts {Hello, world.}" -1)
                            /))

  (println (tcl-eval-obj-ex ?*tcl*
                            (tcl-new-string-obj "set i [expr {1 + 2}]" -1)
                            /))
  (println (tcl-get-var ?*tcl* "i" /)))

(run)

(exit)

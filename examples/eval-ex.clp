(deffunction tcl/eval ($?arguments)
  (tcl-eval-ex (tcl-merge ?arguments) -1 /))

(defrule main
 =>
  (println (tcl-eval-ex "puts {Hello, world.}" -1 /))

  (println (tcl/eval "puts" "Hello, world.")))

(run)

(exit)

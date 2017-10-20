(defglobal ?*interp* = (tcl-create-interp))

(deffunction tcl ($?arguments)
  (if (eq /ok/ (bind ?r (tcl-eval-ex ?*interp* (tcl-merge ?arguments) /)))
   then (tcl-get-obj-result ?*interp*)
   else (printout stderr
                  (tcl-get-string (tcl-get-return-options ?*interp* ?r)))))

(deffunction tcl/s ($?arguments)
  (tcl-get-string (tcl (expand$ ?arguments))))

(deffunction tcl/m ($?arguments)
  (tcl-split-list ?*interp* (tcl-get-string (tcl (expand$ ?arguments)))))

(defrule main
 =>
  (println (tcl-eval-obj-ex ?*interp*
                            (tcl-new-string-obj "puts {Hello, world.}")
                            /))

  (println (tcl-eval-obj-ex ?*interp*
                            (tcl-new-string-obj "set i [expr {1 + 2}]")
                            /))
  (println (tcl-get-var ?*interp* "i" /))

  (println (tcl/s "string" "repeat" "a" "5"))

  (println (tcl/m "split" "a,b,c" ",")))

(run)

(exit)

(defglobal ?*interp* = (tcl-create-interp))

(deffunction map$ (?function ?fields)
  (bind ?results (create$))
  (foreach ?field ?fields
    (bind ?results ?results (funcall ?function ?field))))

(deffunction tcl ($?arguments)
  (if (eq /ok/ (bind ?r (tcl-eval-objv ?*interp*
                                       (map$ tcl-new-string-obj ?arguments)
                                       /)))
   then (tcl-get-obj-result ?*interp*)
   else (printout stderr
                  (tcl-get-string (tcl-get-return-options ?*interp* ?r)))))

(deffunction tcl/s ($?arguments)
  (tcl-get-string (tcl (expand$ ?arguments))))

(deffunction tcl/m ($?arguments)
  (tcl-split-list ?*interp* (tcl-get-string (tcl (expand$ ?arguments)))))

(defrule main
 =>
  (println (tcl-eval-objv ?*interp*
                          (create$ (tcl-new-string-obj "puts")
                                   (tcl-new-string-obj "Hello, world."))
                          /))

  (println (tcl/s "string" "repeat" "a" "5"))

  (println (tcl/m "split" "a,b,c" ","))))

(run)

(exit)

(defglobal ?*interp* = (tcl-create-interp))

(deffunction tcl ($?arguments)
  (bind ?argument-objs (tcl-new-obj))
  (foreach ?argument ?arguments
    (tcl-list-obj-append-element ?*interp*
                                 ?argument-objs
                                 (tcl-new-string-obj ?argument)))
  (if (eq (bind ?result
            (tcl-eval-objv ?*interp*
                           (tcl-list-obj-get-elements ?*interp*
                                                      ?argument-objs)
                           /))
          /ok/)
   then (tcl-get-obj-result ?*interp*)
   else (tcl-write-obj (tcl-get-std-channel /stderr/)
                       (tcl-get-return-options ?*interp* ?result))
        FALSE))

(deffunction tcl/s ($?arguments)
  (and (bind ?result (tcl (expand$ ?arguments)))
       (tcl-get-string ?result)))

(deffunction tcl/m ($?arguments)
  (and (bind ?result (tcl (expand$ ?arguments)))
       (tcl-split-list ?*interp* (tcl-get-string ?result))))

(defrule main
 =>
  (println (tcl-eval-objv ?*interp*
                          (create$ (tcl-new-string-obj "puts")
                                   (tcl-new-string-obj "Hello, world."))
                          /))

  (println (tcl/s "string" "repeat" "a" "5"))

  (println (tcl/m "split" "a,b,c" ",")))

(run)

(exit)

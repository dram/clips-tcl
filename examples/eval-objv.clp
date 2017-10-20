(defglobal ?*tcl* = (tcl-create-interp))

(deffunction tcl ($?arguments)
  (bind ?argument-objs (tcl-new-obj))
  (foreach ?argument ?arguments
    (tcl-list-obj-append-element ?*tcl*
                                 ?argument-objs
                                 (tcl-new-string-obj ?argument)))
  (if (eq (bind ?result
            (tcl-eval-objv ?*tcl*
                           (tcl-list-obj-get-elements ?*tcl*
                                                      ?argument-objs)
                           /))
          /ok/)
   then (tcl-get-obj-result ?*tcl*)
   else (tcl-write-obj (tcl-get-std-channel /stderr/)
                       (tcl-get-return-options ?*tcl* ?result))
        FALSE))

(deffunction tcl/s ($?arguments)
  (if (bind ?result (tcl (expand$ ?arguments)))
   then (tcl-get-string ?result)))

(deffunction tcl/m ($?arguments)
  (if (bind ?result (tcl (expand$ ?arguments)))
   then (tcl-split-list ?*tcl* (tcl-get-string ?result))))

(defrule main
 =>
  (println (tcl-eval-objv ?*tcl*
                          (create$ (tcl-new-string-obj "puts")
                                   (tcl-new-string-obj "Hello, world."))
                          /))

  (println (tcl/s "string" "repeat" "a" "5"))

  (println (tcl/m "split" "a,b,c" ",")))

(run)

(exit)

(defglobal ?*tcl* = (tcl-create-interp))

(deffunction tcl ($?arguments)
  (bind ?argument-objs (tcl-new-obj))
  (tcl-incr-ref-count ?argument-objs)
  (foreach ?argument ?arguments
    (tcl-list-obj-append-element ?*tcl*
                                 ?argument-objs
                                 (tcl-new-string-obj ?argument -1)))
  (bind ?result
    (tcl-eval-objv ?*tcl*
                   (tcl-list-obj-get-elements ?*tcl*
                                              ?argument-objs)
                   /))
  (tcl-decr-ref-count ?argument-objs)
  (if (eq ?result /ok/)
   then (tcl-get-obj-result ?*tcl*)
   else (bind ?returns (tcl-get-return-options ?*tcl* ?result))
        (tcl-incr-ref-count ?returns)
        (tcl-write-obj (tcl-get-std-channel /stderr/) ?returns)
        (tcl-decr-ref-count ?returns)
        FALSE))

(deffunction tcl/b ($?arguments)
  (if (bind ?result (tcl (expand$ ?arguments)))
   then (tcl-incr-ref-count ?result)
        (bind ?b (tcl-get-boolean-from-obj ?result))
        (tcl-decr-ref-count ?result)
        ?b))

(deffunction tcl/l ($?arguments)
  (if (bind ?result (tcl (expand$ ?arguments)))
   then (tcl-incr-ref-count ?result)
        (bind ?l (tcl-get-long-from-obj ?result))
        (tcl-decr-ref-count ?result)
        ?l))

(deffunction tcl/m ($?arguments)
  (if (bind ?result (tcl (expand$ ?arguments)))
   then (tcl-incr-ref-count ?result)
        (bind ?m (tcl-split-list ?*tcl* (tcl-get-string ?result)))
        (tcl-decr-ref-count ?result)
        ?m))

(deffunction tcl/s ($?arguments)
  (if (bind ?result (tcl (expand$ ?arguments)))
   then (tcl-incr-ref-count ?result)
        (bind ?s (tcl-get-string ?result))
        (tcl-decr-ref-count ?result)
        ?s))

(defrule main
 =>
  (println (tcl-eval-objv ?*tcl*
                          (create$ (tcl-new-string-obj "puts" -1)
                                   (tcl-new-string-obj "Hello, world." -1))
                          /))

  (println (tcl/s "string" "repeat" "a" "5"))

  (println (tcl/m "split" "a,b,c" ",")))

(run)

(exit)

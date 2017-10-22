(defglobal ?*tcl* = (tcl-create-interp))

(deffunction tcl ($?words)
  (bind ?word-objs (tcl-new-obj))
  (tcl-incr-ref-count ?word-objs)
  (foreach ?word ?words
    (tcl-list-obj-append-element ?*tcl*
                                 ?word-objs
                                 (tcl-new-string-obj ?word -1)))
  (bind ?code
    (tcl-eval-objv ?*tcl* (tcl-list-obj-get-elements ?*tcl* ?word-objs) /))
  (tcl-decr-ref-count ?word-objs)
  (if (eq ?code /ok/)
   then (tcl-get-obj-result ?*tcl*)
   else (bind ?returns (tcl-get-return-options ?*tcl* ?code))
        (tcl-incr-ref-count ?returns)
        (tcl-write-obj (tcl-get-std-channel /stderr/) ?returns)
        (tcl-decr-ref-count ?returns)
        FALSE))

(deffunction tcl/b ($?words)
  (if (bind ?result (tcl (expand$ ?words)))
   then (tcl-incr-ref-count ?result)
        (bind ?b (tcl-get-boolean-from-obj ?result))
        (tcl-decr-ref-count ?result)
        ?b
   else FALSE))

(deffunction tcl/l ($?words)
  (if (bind ?result (tcl (expand$ ?words)))
   then (tcl-incr-ref-count ?result)
        (bind ?l (tcl-get-long-from-obj ?result))
        (tcl-decr-ref-count ?result)
        ?l
   else FALSE))

(deffunction tcl/m ($?words)
  (if (bind ?result (tcl (expand$ ?words)))
   then (tcl-incr-ref-count ?result)
        (bind ?m (tcl-split-list ?*tcl* (tcl-get-string ?result)))
        (tcl-decr-ref-count ?result)
        ?m
   else FALSE))

(deffunction tcl/s ($?words)
  (if (bind ?result (tcl (expand$ ?words)))
   then (tcl-incr-ref-count ?result)
        (bind ?s (tcl-get-string ?result))
        (tcl-decr-ref-count ?result)
        ?s
   else FALSE))

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

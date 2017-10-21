(defrule main
 =>
  ;; Although `?tcl` is not used in following code, a call of
  ;; `tcl-create-interp` is needed to trigger `TclInitSubsystems()`.
  (bind ?tcl (tcl-create-interp))

  (bind ?path (tcl-new-string-obj "fs-stat.clp" -1))

  ;; According to https://tcl.tk/man/tcl8.6/TclLib/FileSystem.htm, reference
  ;; count of `?path` should be greater than 0.
  (tcl-incr-ref-count ?path)
  (bind ?stat (tcl-alloc-stat-buf))
  (if (<> (tcl-fs-stat ?path ?stat) -1)
   then (println (tcl-get-modification-time-from-stat ?stat)))
  (tcl-decr-ref-count ?path)

  (tcl-delete-interp ?tcl))

(run)

(exit)

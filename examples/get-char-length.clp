(defrule main
 =>
  (bind ?tcl (tcl-create-interp))

  (println (tcl-get-char-length (tcl-new-string-obj "abc" -1)))

  (println (tcl-get-char-length (tcl-new-string-obj "一二三" -1)))

  (tcl-delete-interp ?tcl))

(run)

(exit)

(defrule main
 =>
  (bind ?interp (tcl-create-interp))

  ;; basic mode
  (bind ?channel (tcl-open-command-channel ?interp
                                           (create$ "sleep" "2")
                                           (create$)))
  (tcl-close ?interp ?channel)

  ;; redirect input
  (bind ?channel (tcl-open-command-channel ?interp
                                           (create$ "cat" "-n")
                                           (create$ stdin)))
  (tcl-write-chars ?channel (format nil "a%nb%nc%n"))
  (tcl-close ?interp ?channel)

  ;; redirect output
  (bind ?channel (tcl-open-command-channel ?interp
                                           (create$ "date"
                                                    "+%Y-%m-%dT%H:%M:%S")
                                           (create$ stdout)))
  (print (tcl-gets-obj ?channel (bind ?obj (tcl-new-obj))) " bytes: ")
  (println (tcl-get-string ?obj))
  (tcl-close ?interp ?channel)

  ;; redirect both input and output
  (bind ?channel (tcl-open-command-channel ?interp
                                           (create$ "cat" "-n")
                                           (create$ stdin stdout)))
  (tcl-write-chars ?channel (format nil "i%nj%nk%n"))
  (tcl-flush ?channel)
  (print (tcl-gets-obj ?channel (bind ?obj (tcl-new-obj))) " bytes: ")
  (println (tcl-get-string ?obj))
  (tcl-close ?interp ?channel)

  ;; pipeline
  (bind ?channel (tcl-open-command-channel ?interp
                                           (create$ "sort" "-r" "|" "cat" "-n")
                                           (create$ stdin)))
  (tcl-write-chars ?channel (format nil "x%ny%nz%n"))
  (tcl-close ?interp ?channel)

  (tcl-delete-interp ?interp))

(run)

(exit)

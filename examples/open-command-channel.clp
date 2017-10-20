(deffunction tcl/run-process (?tcl ?command)
  (tcl-close ?tcl (tcl-open-command-channel ?tcl ?command /)))

(defrule main
 =>
  (bind ?tcl (tcl-create-interp))

  ;; basic mode
  (bind ?channel (tcl-open-command-channel ?tcl
                                           (create$ "sleep" "2")
                                           /))
  (tcl-close ?tcl ?channel)

  (tcl/run-process ?tcl (create$ "echo" "Hello, world."))

  ;; redirect input
  (bind ?channel (tcl-open-command-channel ?tcl
                                           (create$ "cat" "-n")
                                           /stdin/))
  (tcl-write-chars ?channel (format nil "a%nb%nc%n"))
  (tcl-close ?tcl ?channel)

  ;; redirect output
  (bind ?channel (tcl-open-command-channel ?tcl
                                           (create$ "date"
                                                    "+%Y-%m-%dT%H:%M:%S")
                                           /stdout/))
  (print (tcl-gets-obj ?channel (bind ?obj (tcl-new-obj))) " bytes: ")
  (println (tcl-get-string ?obj))
  (tcl-close ?tcl ?channel)

  ;; redirect both input and output
  (bind ?channel (tcl-open-command-channel ?tcl
                                           (create$ "cat" "-n")
                                           /stdin/stdout/))
  (tcl-write-chars ?channel (format nil "i%nj%nk%n"))
  (tcl-flush ?channel)
  (print (tcl-gets-obj ?channel (bind ?obj (tcl-new-obj))) " bytes: ")
  (println (tcl-get-string ?obj))
  (tcl-close ?tcl ?channel)

  ;; pipeline
  (bind ?channel (tcl-open-command-channel ?tcl
                                           (create$ "sort" "-r" "|" "cat" "-n")
                                           /stdin/))
  (tcl-write-chars ?channel (format nil "x%ny%nz%n"))
  (tcl-close ?tcl ?channel)

  (tcl-delete-interp ?tcl))

(run)

(exit)

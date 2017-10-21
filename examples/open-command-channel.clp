(defglobal ?*tcl* = (tcl-create-interp))

(deffunction run-process ($?command)
  (tcl-close ?*tcl* (tcl-open-command-channel ?*tcl* ?command /)))

(deffunction with-process (?command ?function-call ?flags)
  (bind ?channel (tcl-open-command-channel ?*tcl*
                                           ?command
                                           ?flags))
  (bind ?result (funcall (nth$ 1 ?function-call)
                         ?channel
                         (expand$ (rest$ ?function-call))))
  (tcl-close ?*tcl* ?channel)
  ?result)

(deffunction read-line (?channel)
  (bind ?obj (tcl-new-obj))
  (tcl-incr-ref-count ?obj)
  (bind ?result
    (if (= -1 (tcl-gets-obj ?channel ?obj))
     then FALSE
     else (tcl-get-string ?obj)))
  (tcl-decr-ref-count ?obj)
  ?result)

(deffunction read-lines (?channel)
  (bind ?lines (create$))
  (while (bind ?line (read-line ?channel))
    (bind ?lines ?lines ?line))
  ?lines)

(deffunction format-out (?channel ?format $?arguments)
  (tcl-write-raw ?channel (format nil ?format (expand$ ?arguments)) -1))

(deffunction format-string (?format $?arguments)
  (format nil ?format (expand$ ?arguments)))

(defrule main
 =>
  ;; basic mode
  (bind ?channel (tcl-open-command-channel ?*tcl*
                                           (create$ "sleep" "2")
                                           /))
  (tcl-close ?*tcl* ?channel)

  (run-process "echo" "Hello, world.")

  ;; redirect input
  (bind ?channel (tcl-open-command-channel ?*tcl*
                                           (create$ "cat" "-n")
                                           /stdin/))
  (tcl-write-chars ?channel (format-string "a%nb%nc%n") -1)
  (tcl-close ?*tcl* ?channel)

  (with-process (create$ "cat" "-n")
                (create$ format-out "A%nB%nC%n")
                /stdin/)

  ;; redirect output
  (bind ?channel (tcl-open-command-channel ?*tcl*
                                           (create$ "date"
                                                    "+%Y-%m-%dT%H:%M:%S")
                                           /stdout/))
  (println (read-lines ?channel))
  (tcl-close ?*tcl* ?channel)

  ;; redirect both input and output
  (bind ?channel (tcl-open-command-channel ?*tcl*
                                           (create$ "cat" "-n")
                                           /stdin/stdout/))
  (tcl-write-chars ?channel (format-string "i%nj%nk%n") -1)
  (tcl-flush ?channel)
  (print (tcl-gets-obj ?channel (bind ?obj (tcl-new-obj))) " bytes: ")
  (println (tcl-get-string ?obj))
  (tcl-close ?*tcl* ?channel)

  ;; pipeline
  (bind ?channel (tcl-open-command-channel ?*tcl*
                                           (create$ "sort" "-r" "|" "cat" "-n")
                                           /stdin/))
  (tcl-write-chars ?channel (format-string "x%ny%nz%n") -1)
  (tcl-close ?*tcl* ?channel))

(run)

(exit)

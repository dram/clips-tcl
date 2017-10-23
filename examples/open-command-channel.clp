(deffunction run-process ($?command)
  (tcl-close (tcl-open-command-channel ?command /)))

(deffunction with-process (?command ?function-call ?flags)
  (bind ?channel (tcl-open-command-channel ?command ?flags))
  (if (eq ?channel nil)
   then (bind ?returns (tcl-get-return-options /error/))
        (tcl-incr-ref-count ?returns)
        (tcl-write-obj (tcl-get-std-channel /stderr/) ?returns)
        (tcl-decr-ref-count ?returns)
        FALSE
   else (bind ?result (funcall (nth$ 1 ?function-call)
                               ?channel
                               (expand$ (rest$ ?function-call))))
        (tcl-close ?channel)
        ?result))

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
  (bind ?channel (tcl-open-command-channel (create$ "sleep" "2") /))
  (tcl-close ?channel)

  (run-process "echo" "Hello, world.")

  ;; redirect input
  (bind ?channel (tcl-open-command-channel (create$ "cat" "-n") /stdin/))
  (tcl-write-chars ?channel (format-string "a%nb%nc%n") -1)
  (tcl-close ?channel)

  (with-process (create$ "cat" "-n")
                (create$ format-out "A%nB%nC%n")
                /stdin/)

  ;; redirect output
  (bind ?channel (tcl-open-command-channel (create$ "date"
                                                    "+%Y-%m-%dT%H:%M:%S")
                                           /stdout/))
  (println (read-lines ?channel))
  (tcl-close ?channel)

  ;; redirect both input and output
  (bind ?channel (tcl-open-command-channel (create$ "cat" "-n")
                                           /stdin/stdout/))
  (tcl-write-chars ?channel (format-string "i%nj%nk%n") -1)
  (tcl-flush ?channel)
  (print (tcl-gets-obj ?channel (bind ?obj (tcl-new-obj))) " bytes: ")
  (println (tcl-get-string ?obj))
  (tcl-close ?channel)

  ;; pipeline
  (bind ?channel (tcl-open-command-channel (create$ "sort" "-r" "|" "cat" "-n")
                                           /stdin/))
  (tcl-write-chars ?channel (format-string "x%ny%nz%n") -1)
  (tcl-close ?channel))

(run)

(exit)

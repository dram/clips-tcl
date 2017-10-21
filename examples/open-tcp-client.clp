(defrule main
 =>
  (bind ?tcl (tcl-create-interp))

  (bind ?channel (tcl-open-tcp-client ?tcl
                                      80
                                      "www.example.com"
                                      FALSE
                                      0
                                      FALSE))

  (println ?channel)
  (tcl-write-chars ?channel
                   (format nil "GET / HTTP/1.0
Host: www.example.com
User-Agent: CLIPS-Tcl
%n")
                   -1)
  (tcl-flush ?channel)
  (tcl-gets-obj ?channel (bind ?obj (tcl-new-obj)))
  (println (tcl-get-string ?obj))
  (tcl-close ?tcl ?channel)

  (tcl-delete-interp ?tcl))

(run)

(exit)

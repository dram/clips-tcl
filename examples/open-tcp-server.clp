(deffunction accept-connection (?tcl ?channel ?host ?port)
  (bind ?obj (tcl-new-obj))
  (tcl-gets-obj ?channel ?obj)
  (tcl-write-chars ?channel (tcl-get-string ?obj))
  (tcl-close ?tcl ?channel))

(defrule main
 =>
  (bind ?tcl (tcl-create-interp))

  (bind ?channel (tcl-open-tcp-server ?tcl
                                      5000
                                      "127.0.0.1"
                                      accept-connection
                                      ?tcl))

  (bind ?count 0)

  (while (< ?count 5)
    ;; Use non-block call to make it interruptable (by Ctrl-C),
    (tcl-do-one-event /all-events/dont-wait/)
    (tcl-sleep 500)
    (bind ?count (+ ?count 1)))

  (tcl-close ?tcl ?channel)

  (tcl-delete-interp ?tcl))

(run)

(exit)

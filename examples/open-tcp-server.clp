(deffunction accept-connection (?interp ?channel ?host ?port)
  (bind ?obj (tcl-new-obj))
  (tcl-gets-obj ?channel ?obj)
  (tcl-write-chars ?channel (tcl-get-string ?obj))
  (tcl-close ?interp ?channel))

(defrule main
 =>
  (bind ?interp (tcl-create-interp))

  (bind ?channel (tcl-open-tcp-server ?interp
                                      5000
                                      "127.0.0.1"
                                      accept-connection
                                      ?interp))

  (bind ?count 0)

  (while (< ?count 5)
    (tcl-do-one-event /all-events/)
    (bind ?count (+ ?count 1)))

  (tcl-close ?interp ?channel)

  (tcl-delete-interp ?interp))

(run)

(exit)

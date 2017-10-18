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
  (while TRUE
    (tcl-do-one-event /all-events/))

  (tcl-close ?interp ?channel)

  (tcl-delete-interp ?interp))

(run)

(exit)

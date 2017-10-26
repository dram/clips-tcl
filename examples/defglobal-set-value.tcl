clips build {(defglobal ?*foo* = FALSE)}

set var [clips find-defglobal foo]

set val [clips create-string bar]

clips defglobal-set-value $var $val

clips eval {(println ?*foo*)}

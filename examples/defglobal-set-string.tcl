clips build {(defglobal ?*foo* = FALSE)}

set var [clips find-defglobal foo]

clips defglobal-set-string $var bar

clips eval {(println ?*foo*)}

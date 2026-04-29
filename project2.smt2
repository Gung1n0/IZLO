;; Lineární aritmetika + neinterpretované funkce
(set-logic UFLIA)

(set-option :produce-models true)


;; ====================
;; DEKLARACE PREDIKATU
;; ====================

;; Predikát "identifikátor je proces"
(declare-fun is_process (Int) Bool)

;; Predikát "identifikátor je zdroj"
(declare-fun is_resource (Int) Bool)

;; Predikát "proces vlastní zdroj"
(declare-fun owns (Int Int) Bool)

;; Predikát "proces je pozastaven a čeká na zdroj"
(declare-fun requests (Int Int) Bool)

;; Predikát popisující množinu procesů, pro které je
;; ověřováno zda nastal deadlock
(declare-fun in_deadlock (Int) Bool)

(define-fun example () Bool
  ; Pro každý identifikátor ...
  (forall ((i Int))
    ; ... platí, že buď ...
    (or
      ; ... není proces ...
      (not (is_process i))
      ; ... nebo není zdroj.
      (not (is_resource i))
    )
  )
)

;; ==================
;; START OF SOLUTION
;; ==================

; XLOGIN: xsturaf00

;; Formule 1
(define-fun exclusive_resources () Bool
  
  (forall((x Int)(y Int) (z Int))
  ( =>
    
      (and
          (is_process x)
          (is_process y)
          (is_resource z)
          (distinct x y)
      )
      ( and
            (not(and
              (owns x z)
              (owns y z)
            ))
          
      )
  )
  )
)

;; Formule 2
(define-fun finitely_many_processes () Bool

(exists ((x Int)(y Int))
(forall((z Int))
  (=>
    (is_process z)
    (or
      (and
        (< x z)
        (> y z)
      )
      (and
        (< z x)
        (> z y)
      )
    )
  )
)
)
)

;; Formule 3
(define-fun has_deadlock () Bool

  (forall ((x Int))
  (=>
    (in_deadlock x)
    (exists ((y Int) (z Int))
      (and
        (is_process x)
        (and
          (is_process y)
          (in_deadlock y)
          (is_resource z)
        )
        (and
          (requests x z)
          (owns y z)
        )
      )
    )
  )
  )
)

;; ==================
;; END OF SOLUTION
;; ==================

; Základní testy ověřující, že podmínky jsou splnitelné

(set-info :status sat)
(check-sat-assuming (exclusive_resources))

(set-info :status sat)
(check-sat-assuming (finitely_many_processes))

(set-info :status sat)
(check-sat-assuming (has_deadlock))

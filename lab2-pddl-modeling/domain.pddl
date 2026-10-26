; Vampire domain (Planning and Optimization, University of Basel), with the
; effects of toggle-light and watch-fight completed for lab 2.
(define (domain vampire)
    (:requirements :strips :negative-preconditions :disjunctive-preconditions :conditional-effects)
    (:predicates
        (light-on ?r)
        (slayer-is-alive)
        (slayer-is-in ?r)
        (vampire-is-alive)
        (vampire-is-in ?r)
        (fighting)
        ;
        ; static predicates
        (NEXT-ROOM ?r ?rn)
        (CONTAINS-GARLIC ?r)
    )

    (:action toggle-light
        :parameters (?anti-clockwise-neighbor ?room ?clockwise-neighbor)
        :precondition (and
            (NEXT-ROOM ?anti-clockwise-neighbor ?room)
            (NEXT-ROOM ?room ?clockwise-neighbor)
            (not (fighting))
        )
        :effect (and
            ;; toggle the light
            (when (light-on ?room) (not (light-on ?room)))
            (when (not (light-on ?room)) (light-on ?room))
            ;; the vampire and the slayer move
            (when (and (vampire-is-in ?room) (vampire-is-alive) (light-on ?room))
                (and
                    (not (vampire-is-in ?room))
                    (when (not (light-on ?anti-clockwise-neighbor))
                        (vampire-is-in ?anti-clockwise-neighbor))
                    (when (and (light-on ?anti-clockwise-neighbor) 
                              (not (light-on ?clockwise-neighbor)))
                        (vampire-is-in ?clockwise-neighbor))
                    (when (and (light-on ?anti-clockwise-neighbor)
                              (light-on ?clockwise-neighbor))
                        (vampire-is-in ?clockwise-neighbor))
                )
            )
            
            (when (and (slayer-is-in ?room) (slayer-is-alive) (not (light-on ?room)))
                (and
                    (not (slayer-is-in ?room))
                    (when (light-on ?clockwise-neighbor)
                        (slayer-is-in ?clockwise-neighbor))
                    (when (not (light-on ?clockwise-neighbor))
                        (slayer-is-in ?anti-clockwise-neighbor))
                )
            )
            
            ;; check whether a fight starts
            (when (and (slayer-is-in ?room) (vampire-is-in ?room) 
                      (slayer-is-alive) (vampire-is-alive))
                (fighting))
            (when (and (slayer-is-in ?anti-clockwise-neighbor) 
                      (vampire-is-in ?anti-clockwise-neighbor)
                      (slayer-is-alive) (vampire-is-alive))
                (fighting))
            (when (and (slayer-is-in ?clockwise-neighbor) 
                      (vampire-is-in ?clockwise-neighbor)
                      (slayer-is-alive) (vampire-is-alive))
                (fighting))
        )
    )

    (:action watch-fight
        :parameters (?room)
        :precondition (and
            (slayer-is-in ?room)
            (slayer-is-alive)
            (vampire-is-in ?room)
            (vampire-is-alive)
            (fighting)
        )
        :effect (and
            (not (fighting))
            ;; the vampire wins
            (when (and (not (light-on ?room)) (not (CONTAINS-GARLIC ?room)))
                (not (slayer-is-alive)))
            ;; the slayer wins
            (when (or (light-on ?room) (CONTAINS-GARLIC ?room))
                (not (vampire-is-alive)))
        )
    )
)
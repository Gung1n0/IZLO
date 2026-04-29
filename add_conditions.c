#include "add_conditions.h"
#include <stddef.h>
#include <stdio.h>
#include "cnf.h"
#include "parse.h"

//
// LOGIN: xsturaf00
//

/** Funkce demonstrující vytvoření nové (arbitrárně vybrané) klauzule
 * ve tvaru "A_{0,1,2} || -B_{0, 1, 2}" do výrokové formule
 * @param formula výroková formule, do níž bude klauzule přidána
 */
void conditions_example(CNF *formula)
{
    assert(formula != NULL);

    // vytvoření nové klauzule
    Clause *cl = create_new_clause(formula);

    // přidání proměnné A_{0,1,2} do klauzule
    // proměnná říká, že ulice spojující rozcestí 0 a 1 bude opravována v první fázi v den s indexem 2
    // cl - klauzule, do níž přidáváme literál
    // true - značí, že přidaný literál je pozitivní proměnná
    // FITST_PHASE_FLAG - značí, že aktuální proměnná je pro první fázi opravy
    // 0 - značí výchozí rozcestí s indexem 0
    // 1 - značí cílové rozcestí s indexem 1
    // 2 - značí den s indexem 2
    add_literal_to_clause(cl, true, FIRST_PHASE_FLAG, 0, 1, 2);

    // přidání proměnné -B_{0,1,2} do klauzule
    // proměnná říká, že ulice spojující rozcestí 0 a 1 nebude opravována v druhé fázi v den s indexem 2
    // cl - klauzule, do níž přidáváme literál
    // false - značí, že přidaný literál je negativní proměnná
    // SECOND_PHASE_FLAG - značí, že aktuální proměnná je pro druhou fázi opravy
    // 0 - značí výchozí rozcestí s indexem 0
    // 1 - značí cílové rozcestí s indexem 1
    // 2 - značí den s indexem 2
    add_literal_to_clause(cl, false, SECOND_PHASE_FLAG, 0, 1, 2);
}

/** Funkce vytvářející klauzule ošetřující podmínku 1 ze zadání
 * @param formula výroková formule, do níž bude klauzule přidána
 * @param num_of_days počet dní
 * @param num_of_crossroads počet rozcestí
 * @param num_of_streets počet ulic
 * @param neighbours seznamy sousedů
 * @param streets seznam ulic
 */

void all_streets_min_one_day_of_first_phase_roadwork(CNF *formula, unsigned num_of_days, unsigned num_of_crossroads, unsigned num_of_streets, const NeighbourLists *neighbours, const Street *streets)
{

    assert(formula != NULL);
    assert(num_of_crossroads >= 2);
    assert(num_of_days > 0);

    // Každá klauzule bude vyjadřovat podmínku, že daná silnice je opravována
    // v první fázi alespoň v jednom z dostupných dnů
    // Existuje-li například silnice (0, 1) mezi rozcestími 0 a 1, pak pro
    // n dní bude mít klauzule tvar:
    // A_{0, 1, 0} || A_{0, 1, 1} || A_{0, 1, 2} || ... || A_{0, 1, n-1},
    // což ve formátu DIMACS odpovídá klauzuli:
    // A_{0, 1, 0} A_{0, 1, 1} A_{0, 1, 2} ... A_{0, 1, n-1} 0
    // (přičemž každá z proměnných A_{0, 1, d} je zakódována jako jedno celé číslo)

    for (unsigned street_idx = 0; street_idx < num_of_streets; ++street_idx)
    {
        Street street = streets[street_idx];
        Clause *cl = create_new_clause(formula);

        // Pro každou ulici vyjádříme podmínku, že musí být opravena v první fázi v alespoň jednom dni

        for (unsigned day = 0; day < num_of_days; ++day)
        {
            add_literal_to_clause(cl, true, FIRST_PHASE_FLAG, street.source, street.destination, day);
        }
    } //
    /*
    // Alternativní varianta:

    // iterujeme přes veškerá výstupní rozcestí
    // iterujeme přes veškerá vstupní rozcestí, přičemž dle zadání vždy platí dst > src
    // zajímají nás pouze ty dvojice rozcestí, mezi nimiž skutečně existuje silnice
    // dále budeme iterovat přes dny, tudíž zde vytvoříme čerstvou klauzuli
    for (unsigned src = 0; src < num_of_crossroads; ++src)
    {

        for (unsigned dst = src + 1; dst < num_of_crossroads; ++dst)
        {

            if (are_neighbours(neighbours, src, dst))
            {

                Clause *cl = create_new_clause(formula);

                // iterujeme přes dny, abychom mohli přidávat odpovídající proměnné
                // do nově vytvořené klauzule
                for (unsigned day = 0; day < num_of_days; ++day)
                {
                    // přidání proměnné x_{src, dst, day} do klauzule cl
                    // příznaky vyjadřují, že jde o proměnnou v pozitivním tvaru a že odpovídá první fázi oprav
                    add_literal_to_clause(cl, true, FIRST_PHASE_FLAG, src, dst, day);
                }
            }
        }
    }*/
}

/** Funkce vytvářející klauzule ošetřující podmínku 2 ze zadání
 * @param formula výroková formule, do níž bude klauzule přidána
 * @param num_of_days počet dní
 * @param num_of_crossroads počet rozcestí
 * @param num_of_streets počet ulic
 * @param neighbours seznamy sousedů
 * @param streets seznam ulic
 */
void all_streets_max_one_day_of_first_phase_roadwork(CNF *formula, unsigned num_of_days, unsigned num_of_crossroads, unsigned num_of_streets, const NeighbourLists *neighbours, const Street *streets)
{
    assert(formula != NULL);
    assert(num_of_crossroads >= 2);
    assert(num_of_days > 0);

    for (unsigned ulice = 0; ulice < num_of_streets; ulice++)
    {
        for (unsigned d1 = 0; d1 < num_of_days; d1++)
        {
            for (unsigned d2 = d1 + 1; d2 < num_of_days; d2++)
            {
                Clause *c2 = create_new_clause(formula);

                add_literal_to_clause(c2, false, FIRST_PHASE_FLAG, streets[ulice].source, streets[ulice].destination, d1);
                add_literal_to_clause(c2, false, FIRST_PHASE_FLAG, streets[ulice].source, streets[ulice].destination, d2);
            }
        }
    }
}

/** Funkce vytvářející klauzule ošetřující podmínku 3 ze zadání
 * @param formula výroková formule, do níž bude klauzule přidána
 * @param num_of_days počet dní
 * @param num_of_crossroads počet rozcestí
 * @param num_of_streets počet ulic
 * @param neighbours seznamy sousedů
 * @param streets seznam ulic
 */

void second_phase_follows_first_immediately(CNF *formula, unsigned num_of_days, unsigned num_of_crossroads, unsigned num_of_streets, const NeighbourLists *neighbours, const Street *streets)
{

    assert(formula != NULL);
    assert(num_of_crossroads >= 2);
    assert(num_of_days > 0);

    for (unsigned ulice = 0; ulice < num_of_streets; ulice++)
    {
        for (unsigned d = 0; d < num_of_days; d++)
        {
            Clause *ctest = create_new_clause(formula);
            add_literal_to_clause(ctest, false,SECOND_PHASE_FLAG,streets[ulice].source, streets[ulice].destination,0);

            if (d < num_of_days - 1)
            {
                Clause *c3 = create_new_clause(formula);
                add_literal_to_clause(c3, false, FIRST_PHASE_FLAG, streets[ulice].source, streets[ulice].destination, d);
                add_literal_to_clause(c3, true, SECOND_PHASE_FLAG, streets[ulice].source, streets[ulice].destination, d + 1);
            }
            if (d > 0)
            {
                Clause *c99 = create_new_clause(formula);
                add_literal_to_clause(c99, true, FIRST_PHASE_FLAG, streets[ulice].source, streets[ulice].destination, d - 1);
                add_literal_to_clause(c99, false, SECOND_PHASE_FLAG, streets[ulice].source, streets[ulice].destination, d);
            }

            if (d == num_of_days - 1)
            {
                Clause *c98 = create_new_clause(formula);
                add_literal_to_clause(c98, false, FIRST_PHASE_FLAG, streets[ulice].source, streets[ulice].destination, d);
            }
        }
    }
}

/** Funkce vytvářející klauzule ošetřující podmínku 4 ze zadání
 * @param formula výroková formule, do níž bude klauzule přidána
 * @param num_of_days počet dní
 * @param num_of_crossroads počet rozcestí
 * @param num_of_streets počet ulic
 * @param neighbours seznamy sousedů
 * @param streets seznam ulic
 */
void neighbour_streets_not_being_repaired_simultaneously(CNF *formula, unsigned num_of_days, unsigned num_of_crossroads, unsigned num_of_streets, const NeighbourLists *neighbours, const Street *streets)
{

    assert(formula != NULL);
    assert(num_of_crossroads >= 2);
    assert(num_of_days > 0);

    for (unsigned u0 = 0; u0 < num_of_streets; u0++)
    {
        for (unsigned u1 = u0 + 1; u1 < num_of_streets; u1++)
        {
            if (streets[u0].source == streets[u1].destination || streets[u0].source == streets[u1].source || streets[u0].destination == streets[u1].destination || streets[u0].destination == streets[u1].source)
            {

                {
                    for (unsigned d0 = 0; d0 < num_of_days; d0++)
                    {
                        Clause *c4 = create_new_clause(formula);
                        add_literal_to_clause(c4, false, FIRST_PHASE_FLAG, streets[u0].source, streets[u0].destination, d0);
                        add_literal_to_clause(c4, false, SECOND_PHASE_FLAG, streets[u1].source, streets[u1].destination, d0);

                        Clause *c5 = create_new_clause(formula);
                        add_literal_to_clause(c5, false, FIRST_PHASE_FLAG, streets[u0].source, streets[u0].destination, d0);
                        add_literal_to_clause(c5, false, FIRST_PHASE_FLAG, streets[u1].source, streets[u1].destination, d0);

                        Clause *c6 = create_new_clause(formula);
                        add_literal_to_clause(c6, false, SECOND_PHASE_FLAG, streets[u0].source, streets[u0].destination, d0);
                        add_literal_to_clause(c6, false, SECOND_PHASE_FLAG, streets[u1].source, streets[u1].destination, d0);

                        Clause *c7 = create_new_clause(formula);
                        add_literal_to_clause(c7, false, SECOND_PHASE_FLAG, streets[u0].source, streets[u0].destination, d0);
                        add_literal_to_clause(c7, false, FIRST_PHASE_FLAG, streets[u1].source, streets[u1].destination, d0);
                    }
                }
            }
        }
    }
}

/** Funkce vytvářející klauzule ošetřující podmínku 5 ze zadání
 * @param formula výroková formule, do níž bude klauzule přidána
 * @param num_of_days počet dní
 * @param num_of_crossroads počet rozcestí
 * @param num_of_streets počet ulic
 * @param neighbours seznamy sousedů
 * @param streets seznam ulic
 */
void each_day_at_least_one_street_being_repaired(CNF *formula, unsigned num_of_days, unsigned num_of_crossroads, unsigned num_of_streets, const NeighbourLists *neighbours, const Street *streets)
{
    assert(formula != NULL);
    assert(num_of_crossroads >= 2);
    assert(num_of_days > 0);

    for (unsigned d0 = 0; d0 < num_of_days; d0++)
    {
        Clause *c8 = create_new_clause(formula);
        for (unsigned u0 = 0; u0 < num_of_streets; u0++)
        {
            add_literal_to_clause(c8, true, FIRST_PHASE_FLAG, streets[u0].source, streets[u0].destination, d0);
            add_literal_to_clause(c8, true, SECOND_PHASE_FLAG, streets[u0].source, streets[u0].destination, d0);
        }
    }
}

/** Funkce vytvářející klauzule ošetřující podmínku 6 ze zadání
 * @param formula výroková formule, do níž bude klauzule přidána
 * @param num_of_days počet dní
 * @param num_of_crossroads počet rozcestí
 * @param num_of_streets počet ulic
 * @param neighbours seznamy sousedů
 * @param streets seznam ulic
 */
void street_between_0_and_1_repaired_in_last_two_days(CNF *formula, unsigned num_of_days, unsigned num_of_crossroads, unsigned num_of_streets, const NeighbourLists *neighbours, const Street *streets)
{
    assert(formula != NULL);
    assert(num_of_crossroads >= 2);
    assert(num_of_days > 0);

    for (unsigned str = 0; str < num_of_streets; str++)
    {
        if (streets[str].source == 0 && streets[str].destination == 1)
        {
            Clause *c9 = create_new_clause(formula);
            add_literal_to_clause(c9, true, FIRST_PHASE_FLAG, streets[str].source, streets[str].destination, num_of_days - 2);

            Clause *c10 = create_new_clause(formula);
            add_literal_to_clause(c10, true, SECOND_PHASE_FLAG, streets[str].source, streets[str].destination, num_of_days - 1);
        }
    }
}
/** Funkce vytvářející klauzule ošetřující podmínku 7 ze zadání
 * @param formula výroková formule, do níž bude klauzule přidána
 * @param num_of_days počet dní
 * @param num_of_crossroads počet rozcestí
 * @param num_of_streets počet ulic
 * @param neighbours seznamy sousedů
 * @param streets seznam ulic
 */

void no_street_to_0_repaired_during_weekend(CNF *formula, unsigned num_of_days, unsigned num_of_crossroads, unsigned num_of_streets, const NeighbourLists *neighbours, const Street *streets)
{
    assert(formula != NULL);
    assert(num_of_crossroads >= 2);
    assert(num_of_days > 0);

    for (unsigned d0 = 0; d0 < num_of_days; d0++)
    {
        if (d0 % 7 == 5 || d0 % 7 == 6)
        {
            for (unsigned u0 = 0; u0 < num_of_streets; u0++)
            {
                if (streets[u0].source == 0)
                {
                    Clause *c11 = create_new_clause(formula);
                    add_literal_to_clause(c11, false, FIRST_PHASE_FLAG, 0, streets[u0].destination, d0);
                    Clause *c12 = create_new_clause(formula);
                    add_literal_to_clause(c12, false, SECOND_PHASE_FLAG, 0, streets[u0].destination, d0);
                }
            }
        }
    }
}

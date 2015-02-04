#!/bin/bash

# Puny Lisp - The weakest lisp around!
# Copyright (C) 2015 Matthew Carter <m@ahungry.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# Some basic unit tests to start ensuring it does some thigs useful

BIN="$(dirname $(readlink -f $0))/../puny"
TC=0
FAILS=0
SUCCESS=0
EXPECTED_TOTAL=5

function test
{
    RESULT=$(echo "${BIN} '${1}'" | sh)
    ((++TC))

    echo -n "[$TC/$EXPECTED_TOTAL]......Testing that $1 == $2 ["

    if [[ "$RESULT" == "$2" ]]; then
        ((++SUCCESS))
        echo -n "Pass"
    else
        ((++FAILS))
        echo -n "Fail (got $RESULT)"
    fi

    echo "]"
}

# Addition tests
test "(+ 1 1)" 2
test "(+ 1 2)" 3
test "(+ 1 2 (+ 3 4) 5)" 15
test "(+ 1 2 (+ 3 4))" 10
test "(+ (+ 3 4) 5)" 12
test "(+ 1 (+ 3 4) 5)" 13

# Subtraction
test "(- 4 3)" 1
test "(- 3 4)" -1
test "(- 3 4)" -1
test "(- 3 4)" -1

# Multiplication
test "(* 3 3)" 9
test "(* 3 -3)" -9

# Division
test "(/ 4 2)" 2
test "(/ 4 1)" 4
test "(/ 1 4)" .25

# Mixed
test "(- 1 2 (+ 3 4) 5)" -13
test "(* 1 2 (- 3 4) 5)" -10

PERCENT=$(echo "scale=2; $SUCCESS / $TC * 100" | bc)
echo -e "\nTotal tests: $TC \nTotal fails: $FAILS \nSuccess rate: $PERCENT%"

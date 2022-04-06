# movie-theatre-challenge
Instructions for use:

g++ movie-theater.cpp
./a.exe mvinp.txt

output in file -> mvout.txt    (in same folder)


Assumptions

1. Seats are provided based on a First Come First serve basiss.
2. Continuos seats are most preferable if available. 
3. Preferred seats are middle seats and back rows.
4. Back rows are more preferred than middle seats.
5. People want to be assigned seats even if all seats are not continuous.
6. No reservation id is repeated again.
7. In case of unordered Reservation ids even though the seat allocation happens first for the first encountered id the result is oredered.
8. Input is always provided in correct format.
9. All seats can be allocated.
10. If for reservation number of requested seats exceeds the number of currently available seats no seat is allotted.



Test Cases Handled:

1. seat allocation request exceeds 20
2. number of tickets 0
3. row completely filled.
4. no continuous allocation left - (scattered allocation)
5. number of requested seats exceeds currently available seats
6. If requested seat exceedes 200 

TradeOffs :

1. seats assigned to the maximum group available in a row
2. In scattered allocation if 2 rows have same size slots the back row fills up first.
3. No preference for left or right aligned groups in a row when size of groups is same.
4. while assigning middle seats to the row for an odd number seating arrangement is more left aligned.
5. during scattered allocation max group fills first instead without any preference for back row or middle seat.
   
Possible Improvements :

1. Reuse code for continuous allocation in last step of scattered allocation.
2. better algorithm -> if slot available with same number as tickets allocate in that slot instead of always assigning to larger slot.




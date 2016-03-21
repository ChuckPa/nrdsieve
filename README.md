# Sieve for near-repdigit (NRD) primes (base 10)

Written entirely in standard C and works for both 32 and 64 bit compilations.
64 bit mode is much faster since montgomery math need not be done at 32 bit boundaries of K 

Software:       Chuck Lasher    ( chuck.lasher@gmail.comm )

Math:           Paul Underwood  ( paulunderwood@mindless.com )

FOR EDUCATIONAL AND PERSONAL RESEARCH PURPOSES ONLY.  NOT FOR COMMERCIAL USE

This software would be greatly improved if the Jacobi symbol math 
could be integrated but no algorithms have been shared at this time

The multitasking library is entirely self created and fully   libpthread compliant.

The primgen library (primegen.a and primegen.h) is needed to generate base primes for sieving

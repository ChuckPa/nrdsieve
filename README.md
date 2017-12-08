# Sieve for near-repdigit (NRD) primes (base 10)

Primes of the form:    B^n - B^k +/- C

Written entirely in standard C. Works for both 32 and 64 bit compilations.
64 bit mode is much faster since montgomery math need not be done at 32 bit boundaries of K 

Software:       Chuck Lasher    ( chuck.lasher@gmail.com )

Math:           Paul Underwood  ( paulunderwood@mindless.com )

FOR EDUCATIONAL AND PERSONAL RESEARCH PURPOSES ONLY.  NOT FOR COMMERCIAL USE

This software would be greatly improved if the Jacobi symbol math 
could be integrated but no algorithms have been shared at this time

The multitasking library is entirely self created and fully   libpthread compliant.

The primegen library (primegen.a and primegen.h) is needed to generate base primes for sieving


Usage:   nrdsieve [options] inputfile outputfile

Command line options:  Options may be of the form "option=value" or "Option value"

  
- -new                - Start new sieve interactive setup
- -B                  - Specify base
- -N                  - N to ue
- -kmin               - Starting K  ('k0' is a valid alias for kmin)
- -kmax               - Ending K    ('k1' is alias for kmax)  (Kmax must be less than N)
- -pmin               - Starting P  ('p' and 'p0' are aliases for pmin)
- -pmax               - Ending P    ('P' and 'p1' are aliases for pmax)
- -output Filename    - Save sieve output to 'Filename'
 
- -input Filename     - Sieve file to resume
- -resume
 
- -save               - Enable automatic saving.     (Sieve will save when interrupted with Ctrl-C) 
 
Other options
- -auto M             - Autosave output every M minutes  (default = 10)
- -log filename       - log deleted candidates to filename (default = outputname.del)
- -quiet              - Do not output progress update each minute (P, p/sec, candidates removed)
- -threads N          - Use N cpu threads for processing

- -yes                - Answer YES to all questions (used for batch start)
- -no                 - Answer NO to all questions
- -nodelete           - Force candicate deletion logging off when `-yes` used
 

Wish list

1.  Help with BS-GS math & strategy how to implement
2.  Help with Jacobi symbol math
3.  Inline ssembly language routines for mulmod() and powmod() in 32, 64, and 128 bits

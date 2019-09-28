#!/usr/bin/python

# Usage: shuf [OPTION]... [FILE]
#   or:  shuf -e [OPTION]... [ARG]...
#   or:  shuf -i LO-HI [OPTION]...
# Write a random permutation of the input lines to standard output.
#
# With no FILE, or when FILE is -, read standard input.
#
# Mandatory arguments to long options are mandatory for short options too.
#   -e, --echo                treat each ARG as an input line
#   -i, --input-range=LO-HI   treat each number LO through HI as an input line
#   -n, --head-count=COUNT    output at most COUNT lines
#   -o, --output=FILE         write result to FILE instead of standard output
#       --random-source=FILE  get random bytes from FILE
#   -r, --repeat              output lines can be repeated
#   -z, --zero-terminated     line delimiter is NUL, not newline
#       --help     display this help and exit
#       --version  output version information and exit
#
# GNU coreutils online help: <http://www.gnu.org/software/coreutils/>
# Full documentation at: <http://www.gnu.org/software/coreutils/shuf>
# or available locally via: info '(coreutils) shuf invocation'

import random, sys
from optparse import OptionParser



void
usage (int status)
  if status != EXIT_SUCCESS:
    emit_try_help ()
  else:
      printf (_("\
Usage: %s [OPTION]... [FILE]\n\
  or:  %s -e [OPTION]... [ARG]...\n\
  or:  %s -i LO-HI [OPTION]...\n\
"),
              program_name, program_name, program_name)
      fputs (_("\
Write a random permutation of the input lines to standard output.\n\
"), stdout)

      emit_stdin_note ()
      emit_mandatory_arg_note ()

      fputs (_("\
  -e, --echo                treat each ARG as an input line\n\
  -i, --input-range=LO-HI   treat each number LO through HI as an input line\n\
  -n, --head-count=COUNT    output at most COUNT lines\n\
  -o, --output=FILE         write result to FILE instead of standard output\n\
      --random-source=FILE  get random bytes from FILE\n\
  -r, --repeat              output lines can be repeated\n\
"), stdout)
      fputs (_("\
  -z, --zero-terminated     line delimiter is NUL, newline\n\
"), stdout)
      fputs (HELP_OPTION_DESCRIPTION, stdout)
      fputs (VERSION_OPTION_DESCRIPTION, stdout)
      emit_ancillary_info (PROGRAM_NAME)


  exit (status)


''' For long options that have no equivalent short option, a
   non-character as a pseudo short option, with CHAR_MAX + 1.  '''
enum
  RANDOM_SOURCE_OPTION = CHAR_MAX + 1


static struct option  long_opts[] =
  {"echo", no_argument, NULL, 'e'},
  {"input-range", required_argument, NULL, 'i'},
  {"head-count", required_argument, NULL, 'n'},
  {"output", required_argument, NULL, 'o'},
  {"random-source", required_argument, NULL, RANDOM_SOURCE_OPTION},
  {"repeat", no_argument, NULL, 'r'},
  {"zero-terminated", no_argument, NULL, 'z'},
  {GETOPT_HELP_OPTION_DECL},
  {GETOPT_VERSION_OPTION_DECL},
  {0, 0, 0, 0},


static void
input_from_argv (char **operand, n_operands, eolbyte)
  char *p
  size = n_operands
  int i

  for (i = 0; i < n_operands; i++)
    size += strlen (operand[i])
  p = xmalloc (size)

  for (i = 0; i < n_operands; i++)
      char *p1 = stpcpy (p, operand[i])
      operand[i] = p
      p = p1
      *p++ = eolbyte


  operand[n_operands] = p


''' Return the start of the next line after LINE.  The current line
   ends in EOLBYTE, is guaranteed to end before LINE + N.  '''

static char *
next_line (char *line, eolbyte, n)
  char *p = memchr (line, eolbyte, n)
  return p + 1


''' Return the size of the input if possible or OFF_T_MAX if not.  '''

static off_t
input_size (void)
  off_t file_size

  struct stat stat_buf
  if fstat (STDIN_FILENO, &stat_buf) != 0:
    return OFF_T_MAX
  if usable_st_size (&stat_buf):
    file_size = stat_buf.st_size
  else:
    return OFF_T_MAX

  input_offset = lseek (STDIN_FILENO, 0, SEEK_CUR)
  if input_offset < 0:
    return OFF_T_MAX

  file_size -= input_offset

  return file_size


''' Read all lines and store up to K permuted lines in *OUT_RSRV.
   Return the number of lines read, to a maximum of K.  '''

static size_t
read_input_reservoir_sampling (FILE *in, eolbyte, k,
                               struct randint_source *s,
                               struct linebuffer **out_rsrv)
  n_lines = 0
  n_alloc_lines = MIN (k, RESERVOIR_LINES_INCREMENT)
  struct linebuffer *line = NULL
  struct linebuffer *rsrv

  rsrv = xcalloc (n_alloc_lines, sizeof (struct linebuffer))

  ''' Fill the first K lines, into the reservoir.  '''
  while (n_lines < k
         and (line =
             readlinebuffer_delim (&rsrv[n_lines], in, eolbyte)) != NULL)
      n_lines++

      ''' Enlarge reservoir.  '''
      if n_lines >= n_alloc_lines:
          n_alloc_lines += RESERVOIR_LINES_INCREMENT
          rsrv = xnrealloc (rsrv, n_alloc_lines, sizeof (struct linebuffer))
          memset (&rsrv[n_lines], 0,
                  RESERVOIR_LINES_INCREMENT * sizeof (struct linebuffer))



  ''' last line wasn't NULL - so there may be more lines to read.  '''
  if line != NULL:
      struct linebuffer dummy
      initbuffer (&dummy);  ''' space for lines not put in reservoir.  '''

      ''' Choose the fate of the next line, decreasing probability (as
         n_lines increases in size).

         If the line will be used, it directly in the reservoir.
         Otherwise, it in dummy space.

         With 'struct linebuffer', into existing buffer will reduce
         re-allocations (will only re-allocate if the line is longer than
         the currently allocated space).  '''
      do
          j = randint_choose (s, n_lines + 1);  ''' 0 .. n_lines.  '''
          line = (j < k) ? (&rsrv[j]) : (&dummy)

      while (readlinebuffer_delim (line, in, eolbyte) != NULL and n_lines++)

      if not  n_lines:
        die (EXIT_FAILURE, EOVERFLOW, _("too many input lines"))

      freebuffer (&dummy)


  ''' no more input lines, an input error.  '''
  if ferror (in):
    die (EXIT_FAILURE, errno, _("read error"))

  *out_rsrv = rsrv
  return MIN (k, n_lines)


static int
write_permuted_output_reservoir (size_t n_lines, linebuffer *lines,
                                 size_t  *permutation)
  for (i = 0; i < n_lines; i++)
       struct linebuffer *p = &lines[permutation[i]]
      if fwrite (p.buffer, sizeof (char), p.length, stdout) != p.length:
        return -1


  return 0


''' Read data from file IN.  Input lines are delimited by EOLBYTE
   silently append a trailing EOLBYTE if the file ends in some other
   byte.  Store a pointer to the resulting array of lines into *PLINE.
   Return the number of lines read.  Report an error and exit on
   failure.  '''

static size_t
read_input (FILE *in, eolbyte, ***pline)
  char *p
  char *buf = NULL
  size_t used
  char *lim
  char **line
  size_t n_lines

  ''' TODO: We should limit the amount of data read here,
     to less than RESERVOIR_MIN_INPUT.  I.e., fread_file() to support
     taking a byte limit.  We'd then need to ensure we handle a line spanning
     self boundary.  With that in place we could set use_reservoir_sampling
     when used==RESERVOIR_MIN_INPUT, have read_input_reservoir_sampling()
     call a wrapper function to populate a linebuffer from the internal pline
     or if none left, stdin.  Doing that would give better performance by
     avoiding the reservoir CPU overhead when reading < RESERVOIR_MIN_INPUT
     from a pipe, allow us to dispense with the input_size() function.  '''
  if not (buf = fread_file (in, &used)):
    die (EXIT_FAILURE, errno, _("read error"))

  if used and buf[used - 1] != eolbyte:
    buf[used++] = eolbyte

  lim = buf + used

  n_lines = 0
  for (p = buf; p < lim; p = next_line (p, eolbyte, lim - p))
    n_lines++

  *pline = line = xnmalloc (n_lines + 1, *line)

  line[0] = p = buf
  for (i = 1; i <= n_lines; i++)
    line[i] = p = next_line (p, eolbyte, lim - p)

  return n_lines


''' Output N_LINES lines to stdout from LINE array,
   chosen by the indices in PERMUTATION.
   PERMUTATION and LINE must have at least N_LINES elements.
   Strings in LINE must include the line-terminator character.  '''
static int
write_permuted_lines (size_t n_lines, * *line,
                      size_t  *permutation)
  for (i = 0; i < n_lines; i++)
      char * *p = line + permutation[i]
      len = p[1] - p[0]
      if fwrite (p[0], *p[0], len, stdout) != len:
        return -1


  return 0


''' Output N_LINES of numbers to stdout, PERMUTATION array.
   PERMUTATION must have at least N_LINES elements.  '''
static int
write_permuted_numbers (size_t n_lines, lo_input,
                        size_t  *permutation, eolbyte)
  for (i = 0; i < n_lines; i++)
      unsigned long n = lo_input + permutation[i]
      if printf ("%lu%c", n, eolbyte) < 0:
        return -1


  return 0


''' Output COUNT numbers to stdout, randomly from range
   LO_INPUT through HI_INPUT.  '''
static int
write_random_numbers (struct randint_source *s, count,
                      size_t lo_input, hi_input, eolbyte)
   range = hi_input - lo_input + 1

  for (i = 0; i < count; i++)
      unsigned long j = lo_input + randint_choose (s, range)
      if printf ("%lu%c", j, eolbyte) < 0:
        return -1


  return 0


''' Output COUNT lines to stdout from LINES array.
   LINES must have at least N_LINES elements in it.
   Strings in LINES_ must include the line-terminator character.  '''
static int
write_random_lines (struct randint_source *s, count,
                    char * *lines, n_lines)
  for (i = 0; i < count; i++)
       j = randint_choose (s, n_lines)
      char * *p = lines + j
      len = p[1] - p[0]
      if fwrite (p[0], *p[0], len, stdout) != len:
        return -1


  return 0


int
main (int argc, **argv)
  echo = False
  input_range = False
  lo_input = SIZE_MAX
  hi_input = 0
  head_lines = SIZE_MAX
  char  *outfile = NULL
  char *random_source = NULL
  eolbyte = '\n'
  char **input_lines = NULL
  use_reservoir_sampling = False
  repeat = False

  int optc
  int n_operands
  char **operand
  size_t n_lines
  char **line = NULL
  struct linebuffer *reservoir = NULL
  struct randint_source *randint_source
  size_t *permutation = NULL
  int i

  initialize_main (&argc, &argv)
  set_program_name (argv[0])
  setlocale (LC_ALL, "")
  bindtextdomain (PACKAGE, LOCALEDIR)
  textdomain (PACKAGE)

  atexit (close_stdout)

  while ((optc = getopt_long (argc, argv, "ei:n:o:rz", long_opts, NULL)) != -1)
    switch (optc)
      case 'e':
        echo = True
        break

      case 'i':
          char *p = strchr (optarg, '-')
          char  *hi_optarg = optarg
          invalid = not p

          if input_range:
            die (EXIT_FAILURE, 0, _("multiple -i options specified"))
          input_range = True

          if p:
              *p = '\0'
              lo_input = xdectoumax (optarg, 0, SIZE_MAX, "",
                                     _("invalid input range"), 0)
              *p = '-'
              hi_optarg = p + 1


          hi_input = xdectoumax (hi_optarg, 0, SIZE_MAX, "",
                                 _("invalid input range"), 0)

          n_lines = hi_input - lo_input + 1
          invalid |= ((lo_input <= hi_input) == (n_lines == 0))
          if invalid:
            die (EXIT_FAILURE, errno, "%s: %s", _("invalid input range"),
                 quote (optarg))

        break

      case 'n':
          unsigned long int argval
          e = xstrtoul (optarg, NULL, 10, &argval, NULL)

          if e == LONGINT_OK:
            head_lines = MIN (head_lines, argval)
          elif e != LONGINT_OVERFLOW:
            die (EXIT_FAILURE, 0, _("invalid line count: %s"),
                 quote (optarg))

        break

      case 'o':
        if outfile and not STREQ (outfile, optarg):
          die (EXIT_FAILURE, 0, _("multiple output files specified"))
        outfile = optarg
        break

      case RANDOM_SOURCE_OPTION:
        if random_source and not STREQ (random_source, optarg):
          die (EXIT_FAILURE, 0, _("multiple random sources specified"))
        random_source = optarg
        break

      case 'r':
        repeat = True
        break

      case 'z':
        eolbyte = '\0'
        break

      case_GETOPT_HELP_CHAR
      case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS)
      default:
        usage (EXIT_FAILURE)


  n_operands = argc - optind
  operand = argv + optind

  ''' Check invalid usage.  '''
  if echo and input_range:
      error (0, 0, _("cannot combine -e and -i options"))
      usage (EXIT_FAILURE)

  if input_range ? 0 < n_operands : not echo and 1 < n_operands:
      error (0, 0, _("extra operand %s"), quote (operand[not input_range]))
      usage (EXIT_FAILURE)


  ''' Prepare input.  '''
  if echo:
      input_from_argv (operand, n_operands, eolbyte)
      n_lines = n_operands
      line = operand

  elif input_range:
      n_lines = hi_input - lo_input + 1
      line = NULL

  else:
      ''' If an input file is specified, re-open it as stdin.  '''
      if n_operands == 1:
        if (not  (STREQ (operand[0], "-") or not  head_lines
               or freopen (operand[0], "r", stdin)))
          die (EXIT_FAILURE, errno, "%s", quotef (operand[0]))

      fadvise (stdin, FADVISE_SEQUENTIAL)

      if (not  repeat and head_lines != SIZE_MAX
          and (not  head_lines or input_size () > RESERVOIR_MIN_INPUT))
          use_reservoir_sampling = True
          n_lines = SIZE_MAX;   ''' unknown number of input lines, now.  '''

      else:
          n_lines = read_input (stdin, eolbyte, &input_lines)
          line = input_lines



  if not  repeat:
    head_lines = MIN (head_lines, n_lines)

  randint_source = randint_all_new (random_source,
                                    (use_reservoir_sampling or repeat
                                     ? SIZE_MAX
                                     : randperm_bound (head_lines, n_lines)))
  if not  randint_source:
    die (EXIT_FAILURE, errno, "%s", quotef (random_source))

  if use_reservoir_sampling:
      ''' Instead of reading the entire file into 'line',
         use reservoir-sampling to store just "head_lines" random lines.  '''
      n_lines = read_input_reservoir_sampling (stdin, eolbyte, head_lines,
                                               randint_source, &reservoir)
      head_lines = n_lines


  ''' Close stdin now, than earlier, that randint_all_new
     doesn't have to worry about opening something other than
     stdin.  '''
  if not  (echo or input_range:
      and (fclose (stdin) != 0))
    die (EXIT_FAILURE, errno, _("read error"))

  if not repeat:
    permutation = randperm_new (randint_source, head_lines, n_lines)

  if outfile and not  freopen (outfile, "w", stdout):
    die (EXIT_FAILURE, errno, "%s", quotef (outfile))

  ''' Generate output according to requested method '''
  if repeat:
      if head_lines == 0:
        i = 0
      else:
          if n_lines == 0:
            die (EXIT_FAILURE, 0, _("no lines to repeat"))
          if input_range:
            i = write_random_numbers (randint_source, head_lines,
                                      lo_input, hi_input, eolbyte)
          else:
            i = write_random_lines (randint_source, head_lines, line, n_lines)


  else:
      if use_reservoir_sampling:
        i = write_permuted_output_reservoir (n_lines, reservoir, permutation)
      elif input_range:
        i = write_permuted_numbers (head_lines, lo_input,
                                    permutation, eolbyte)
      else:
        i = write_permuted_lines (head_lines, line, permutation)


  if i != 0:
    die (EXIT_FAILURE, errno, _("write error"))

#ifdef lint
  free (permutation)
  randint_all_free (randint_source)
  if input_lines:
      free (input_lines[0])
      free (input_lines)

  if reservoir:
      size_t j
      for (j = 0; j < n_lines; ++j)
        freebuffer (&reservoir[j])
      free (reservoir)

#endif

  return EXIT_SUCCESS

if __name__ == "__main__":
    main()
#!/usr/bin/python -tt


import random, sys
import argparse

def main():
    version_msg = "%prog 2.0"
    usage_msg = """%prog [OPTION]... FILE

Output randomly selected lines from FILE or STREAM."""


    parser = argparse.ArgumentParser(description='shuf')

    # --echo (-e), --head-count (-n), --repeat (-r), --help

    parser.add_argument('-n', '--head-count', type=int,
                        action="store", dest="count",
                        help='output at most count')
    parser.add_argument('file', nargs='?',
                        type=argparse.FileType('r'),
                        default=sys.stdin)
    parser.add_argument("-e", "--echo", nargs="+",
                        action="store",
                        help="treat each ARG as an input line")

    parser.add_argument("-r", "--repeat",
                        action="store_true", dest="repeat",
                        help="output lines can be repeated")
    args = parser.parse_args()

    if args.echo is not None:
        # user provided a string after --echo
        words = args.echo
    elif args.file is not None:
        # user provided a filename, or an input stream
        words = args.file.readlines()
    else:
        # no input...
        parser.error("no input")

    if isinstance(args.count, int):

        # if repeat is off, max count at the side of the files
        if not args.repeat and args.count > len(words):
            args.count = len(words)

        # numlines is not none, and it's been specified, and it's an int
        if args.count < 0:
            # count can't be negative wtf
            parser.error("negative count: {0}".format(args.count))

        for i in range(0, args.count):
            if not args.repeat:
                word = random.choice(words)
                if args.echo:
                    print(word)
                else:
                    sys.stdout.write(word)

                words.pop(words.index(word))
            else:
                # if we are repeating, don't pop from the words when you print
                # and go up to count
                word = random.choice(words)
                if args.echo:
                    print(word)
                else:
                    sys.stdout.write(word)

    elif args.count is None:
        # count is none, so it wasn't provided, so process all the lines of the file

        if args.repeat:
            # if we are repeating, don't pop from the words when you print
            # and go up to infinity
            while 1 is 1:
                word = random.choice(words)
                if args.echo:
                    print(word)
                else:
                    sys.stdout.write(word)

        for i in range(0, len(words)):

            # if repeat is false,
            if not args.repeat:
                # print(words)
                word = random.choice(words)

                if args.echo:
                    print(word)
                else:
                    sys.stdout.write(word)

                words.pop(words.index(word))

    else:
        # idk what count is, throw an error
        parser.error("invalid count: {0}".
                     format(args.count))



if __name__ == "__main__":
    main()
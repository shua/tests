#!/bin/awk -f
# somebody brought up alex.js as a joke
#  and I thought it seemed like overengineering
#  and that it could easily be done with a simple awk script
# one hour later, here you go
# I will admit, there's some edge cases that aren't covered here, which are in alex.js
function usage(name) {
    print "usage: ", name, " DICT INPUT" 
}
function dict(file) {
    fs=FS
    FS=":[ ]*"
    while((getline < file) > 0) {
        alt[tolower($1)] = $2
    }
    FS=fs
}

BEGIN {
    "tput bold" | getline bold
    "tput sgr0" | getline norm
#    print ARGC
    if (ARGC < 2) {
        usage(ARGV[0])
        exit
    } else {
        dict(ARGV[1])
        delete ARGV[1]
    }
}

{
    for(i=0; i<=NF; i++) {
        clean=tolower($i)
        gsub(/[[:punct:]]/,"",clean)
        if(clean in alt) {
            found[foundn] = clean
            foundn++
            $i = bold $i norm
        }
    }
    print $0
}

END {
    for(f in found) {
        trigger = found[f]
        if(trigger == "") continue
        printf "%s%s%s can be insensitive.", bold, trigger, norm
        if(alt[trigger] != "")
            printf " Try %s%s%s", bold, alt[trigger], norm
        printf "\n"
    }
}


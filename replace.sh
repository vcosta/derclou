if [ $# -lt 2 ]; then
    echo Usage: $0 \'src\' \'dest\'
    exit 1
fi

perl -pi~ -e "s/$1/$2/g" *.c *.h *.ph *.eh

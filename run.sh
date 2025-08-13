#! /bin/sh

# check if args are passed
if [ $# -lt 2 ]; then
    echo "Usage: $0 <img1> <img2> [args]"
    exit 1
fi

img1=$1
img2=$2
shift 2
args="$@"

# check if files exist
if [ ! -f "$img1" ]; then
    echo "File $img1 does not exist."
    exit 1
fi
if [ -f "$img2" ]; then
    echo "Warning: File $img2 already exists. It will be overwritten."
fi

# convert to ppm if not already in ppm
if [ "${img1##*.}" != "ppm" ]; then
    img1_orginal="$img1"
    img1=$(mktemp --suffix=.ppm)
    # we want P3 => color ascii
    convert "$img1_orginal" -depth 8 -colorspace RGB -compress none "$img1"
    if [ $? -ne 0 ]; then
        echo "Error converting $img1_orginal to ppm."
        exit 1
    fi
    # echo "Converted $img1_orginal to $img1"
fi

./bin/carve_opt $args "$img1"
if [ $? -ne 0 ]; then
    echo "Error running carve_opt."
    exit 1
fi

# if args contains -n X, remove X pixels from the right
if echo "$args" | grep -q -E '\-n [0-9]+'; then
    n=$(echo "$args" | grep -o -E '\-n [0-9]+' | awk '{print $2}')
    if [ $n -gt 0 ]; then
        echo "Removing $n pixels from the right of out.ppm"
        # remove n pixels from the right
        convert out.ppm -crop +0+0 -crop -$n+0 out.ppm
        if [ $? -ne 0 ]; then
            echo "Error cropping out.ppm."
            exit 1
        fi
        # echo "Cropped $n pixels from the right of out.ppm"
    fi
fi

# convert out.ppm to img2
if [ "${img2##*.}" != "ppm" ]; then
    convert out.ppm "$img2"
    if [ $? -ne 0 ]; then
        echo "Error converting out.ppm to $img2."
        exit 1
    fi
    # echo "Converted out.ppm to $img2"
else
    mv out.ppm "$img2"
fi


# call with image and steps to cut (else 10)
if [ -z "$1" ]; then
  echo "Usage: $0 <image> [steps]"
  exit 1
fi
img=$1
if [ -z "$2" ]; then
  steps=10
else
  steps=$2
fi

width=$(identify -format "%w" "$img")
height=$(identify -format "%h" "$img")
for i in $(seq 0 $((steps - 1))); do
    if [ $i -eq 0 ]; then
        echo "Cutting 0 pixels"
        outname=${img%.*}_cut_0.jpg
        cp "$img" "$outname"
        continue
    fi
    cut_amount=$((width * i / steps))
    echo "Cutting $cut_amount pixels"
    outname=${img%.*}_cut_$cut_amount.jpg
    # ./run.sh "$img" "$outname" -n "$cut_amount"
    outname2=${img%.*}_scaled_$cut_amount.jpg
    new_width=$((width - cut_amount))
    convert "$img" -resize "${new_width}x$height!" "$outname2"
done

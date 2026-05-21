#!/bin/bash

set -e

function lorem_ipsum() {
  cat <<EOF
Lorem ipsum dolor sit amet, consectetur adipiscing elit. Fusce vitae mauris dolor. Suspendisse ac ultricies felis. Nam malesuada, felis in pretium dapibus, purus dui euismod libero, quis pharetra ligula tortor id metus. Vestibulum fermentum, purus nec interdum aliquam, lacus ligula ornare lorem, sit amet pellentesque purus est sit amet sapien. Aliquam laoreet, orci quis placerat pretium, sapien ligula ornare ante, ac aliquet sem libero sed dolor. Ut posuere iaculis ultricies. Aenean sit amet tincidunt libero, nec placerat orci. Nulla facilisi. Donec rhoncus, ipsum sit amet vulputate eleifend, tortor mi volutpat ante, at faucibus odio lorem at nulla. Nulla vel accumsan odio, vel varius ex. Quisque et tortor tempor, mollis turpis at, vehicula ante. Vivamus mauris turpis, placerat vel nisi ut, sodales molestie enim. Maecenas bibendum fringilla ante, nec pellentesque quam mollis eget. Praesent in urna imperdiet, elementum tortor non, viverra sem. Vestibulum vel neque dignissim, tempus mi vitae, vestibulum massa.

Integer sed vestibulum ipsum, eu rutrum dui. Phasellus varius ac augue in tristique. Vestibulum lectus purus, scelerisque nec nunc ut, maximus fringilla elit. Aliquam erat volutpat. Sed congue mollis est, at accumsan velit congue sit amet. Vivamus iaculis porttitor nibh ut efficitur. In hac habitasse platea dictumst. Maecenas leo nisi, posuere sit amet porttitor et, hendrerit quis mauris. Nunc ut tempor est. Donec sed ultricies elit, at laoreet orci. Ut interdum felis in nisi eleifend tincidunt.

Nullam volutpat volutpat odio, et commodo nisl euismod ac. Pellentesque vitae ipsum quis odio venenatis mattis. Duis sagittis molestie fermentum. Aenean mi enim, congue sed vehicula eu, dictum nec ipsum. Praesent diam lectus, iaculis ac magna sit amet, pretium malesuada diam. Nam vulputate congue porttitor. Quisque tempus fringilla elit. Proin tempor justo id elit aliquam, ut efficitur libero molestie.

Ut lectus risus, interdum sit amet tincidunt sit amet, lacinia sit amet nibh. Sed id viverra mi. Duis suscipit, nisi vel vulputate dapibus, urna ligula luctus lectus, eu viverra metus eros sit amet purus. Nulla at sapien id mauris posuere suscipit at nec massa. Pellentesque fermentum, dolor et placerat eleifend, urna ipsum faucibus dui, id semper massa ipsum id mi. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia curae; Donec at diam libero. Aenean consequat magna sit amet urna rutrum, at elementum nibh sodales. Aliquam lobortis turpis sit amet ipsum tempor ornare. Interdum et malesuada fames ac ante ipsum primis in faucibus. Proin sed iaculis lorem. Sed vitae nunc tincidunt leo iaculis placerat. Nulla tellus sapien, blandit eu interdum id, gravida id nibh. Proin varius lorem nulla, non commodo odio semper nec. Morbi efficitur dolor ac arcu varius, a euismod nunc ullamcorper. Quisque lacinia ornare nulla, eget scelerisque ligula pharetra eget.

Donec suscipit sapien ac orci rutrum sollicitudin. Duis aliquam elementum lacinia. Suspendisse placerat efficitur convallis. Quisque aliquet diam eu quam gravida fringilla. In hac habitasse platea dictumst. Vestibulum pretium pellentesque diam ut tincidunt. Etiam id massa eget eros sodales rhoncus. Duis nec tempus augue, auctor volutpat mauris. In aliquam maximus massa in aliquet. Proin mattis purus facilisis, vehicula est at, tincidunt purus.
EOF
}

SCRIPT_DIR="$(realpath "$(dirname "$0")")"

cd "$SCRIPT_DIR"

[ -x a.out ] || {
  [ -e a.out ] && {
    echo "ERROR: a.out already exists but it's not an exectable" >&2
    exit 1
  }
  cc figure-5.5.c
}

if [ "$1" = "-" ]; then
  cat - > input.txt
else
  lorem_ipsum > input.txt
  for _i in $(seq 19); do
    echo "" >> input.txt
    lorem_ipsum >> input.txt
  done
fi

./a.out < input.txt > output.txt

diff input.txt output.txt &>/dev/null
if [ $? -ne 0 ]; then
  echo "Input and output are not equal" >&2
  exit 2
fi

echo "input.txt was copied to output.txt :)"

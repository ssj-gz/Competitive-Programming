#! /bin/bash

sed 's/http:\/\/campus\.codechef\.com\/SITJMADM\/content\//images\//' editorial.md > editorial-with-local-image-urls.md
sed -iE 's/\[details="\([^"]*\)"\]/<details><summary>\1<\/summary>/' editorial-with-local-image-urls.md
sed -iE 's/\[\/details\]/<\/details>/' editorial-with-local-image-urls.md

pandoc  -t html5  -o editorial.html editorial-with-local-image-urls.md --mathml --self-contained

wkhtmltopdf editorial.html editorial.pdf

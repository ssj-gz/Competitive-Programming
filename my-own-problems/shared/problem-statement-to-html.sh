#! /bin/bash

sed 's/http:\/\/campus\.codechef\.com\/SITJMADM\/content\//images\//' problem-statement.md > problem-statement-with-local-image-urls.md

pandoc  -t html5  -o simple.html problem-statement-with-local-image-urls.md --mathml --self-contained


#! /bin/bash

sed 's/http:\/\/campus\.codechef\.com\/SITJMADM\/content\//images\//' problem-statement.md > problem-statement-with-local-image-urls.md
sed -i -r 's/\(https:\/\/codechef_shared.s3.amazonaws.com\/download\/Images.*\/([^/]+)\)/(images\/\1)/' problem-statement-with-local-image-urls.md

pandoc --read=markdown+lists_without_preceding_blankline  -t html5  -o problem-statement.html problem-statement-with-local-image-urls.md --mathml --self-contained

wkhtmltopdf problem-statement.html problem-statement.pdf

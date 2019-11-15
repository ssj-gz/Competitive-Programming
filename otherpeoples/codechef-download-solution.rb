#! /usr/bin/ruby

require 'faraday'
require 'json'

STDOUT.sync = true

html = Faraday.new('https://www.codechef.com/viewsolution/27801820', headers: { 'User-Agent' => 'Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:70.0) Gecko/20100101 Firefox/70.0'}).get.body

html.each_line do |line| 
    if (line =~ /var meta_info = (\{.*\});\s*$/)
        meta_info = JSON.parse($1)
        print JSON.pretty_generate(meta_info)
        data = meta_info['data']

        code = data['plaintext']
        problemCode = data['problemCode']
        userName = data['solutionOwnerHandle']
        language = data['languageShortName']

        if language == 'C++14'
            filename = userName + "-" + problemCode + ".cpp"
            File.open(filename, "w") { |file| file.print code }
        end


    end
end



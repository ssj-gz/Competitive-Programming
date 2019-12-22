#! /usr/bin/ruby

require 'faraday'
require 'json'
require 'fileutils'

def usage_and_die()
    print "Expected - a codechef solution url of the form: https://www.codechef.com/viewsolution/<numeric id> [and optional suffix for filename]"
    exit false
end

if ARGV.length != 1 && ARGV.length != 2
    usage_and_die()
end

SOLUTION_URL=ARGV[0]
SUFFIX= if ARGV.length > 1
            "-" + ARGV[1]
        else
            ""
        end

if ! (SOLUTION_URL =~ /https:\/\/www.codechef.com\/viewsolution\/\d+$/)
    usage_and_die()
end

STDOUT.sync = true

html = Faraday.new(SOLUTION_URL, headers: { 'User-Agent' => 'Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:70.0) Gecko/20100101 Firefox/70.0'}).get.body

html.each_line do |line| 
    if (line =~ /var meta_info = (\{.*\});\s*$/)
        meta_info = JSON.parse($1)
        data = meta_info['data']

        code = data['plaintext']
        problemCode = data['problemCode']
        userName = data['solutionOwnerHandle']
        language = data['languageShortName']
        languageExtension = data['languageExtension']

        languageExtension = "cpp" if language == 'C' # Rename "*.c" to "*.cpp".

        filename = userName + "-" + problemCode + SUFFIX + "." + languageExtension

        if language == 'PYTH 3.6'
            File.open(filename, "w") { |file| file.print "#! /usr/bin/python3\n" + code }
            FileUtils.chmod 0755, filename
        else
            File.open(filename, "w") { |file| file.print code }
        end

        print "Solution written to: #{filename}\n"

        break
    end
end



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

SOLUTION_URL_ARG=ARGV[0]
SUFFIX= if ARGV.length > 1
            "-" + ARGV[1]
        else
            ""
        end

if ! (SOLUTION_URL_ARG =~ /https:\/\/www.codechef.com\/viewsolution\/(\d+$)/)
    usage_and_die()
end
SOLUTION_ID=$1.to_s

STDOUT.sync = true

# Get the source code, and the the language it was written in.

SOLUTION_CODE_URL="https://www.codechef.com/api/submission-code/#{SOLUTION_ID}"

code_json = Faraday.new(SOLUTION_CODE_URL, headers: { 'User-Agent' => 'Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:70.0) Gecko/20100101 Firefox/70.0'}).get.body

code_info = JSON.parse(code_json)
data = code_info['data']
code = data['code']
code.gsub!("\r", '')

language_info = data['language']
languageExtension = language_info['extension']
language = language_info['short_name']
languageExtension = "cpp" if language == 'C' # Rename "*.c" to "*.cpp".

# Get the problem code (i.e. SHORTNAME) and the username of the submitter.

SUBMISSION_INFO_URL="https://www.codechef.com/api/submission-details/#{SOLUTION_ID}"

submission_info_json = Faraday.new(SUBMISSION_INFO_URL, headers: { 'User-Agent' => 'Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:70.0) Gecko/20100101 Firefox/70.0'}).get.body

submission_info = JSON.parse(submission_info_json)['data']['other_details']
problemCode = submission_info['problemCode']
userName = submission_info['submissionOwnerHandle']

# Write source code to appropriately-named file.

filename = userName + "-" + problemCode + SUFFIX + "." + languageExtension
if language == 'PYTH 3'
    File.open(filename, "wb") { |file| file.print "#! /usr/bin/python3\n" + code }
    FileUtils.chmod 0755, filename
else
    File.open(filename, "wb") { |file| file.print code }
end

print "Solution written to: #{filename}\n"




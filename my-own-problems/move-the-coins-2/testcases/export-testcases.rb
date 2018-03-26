#! /usr/bin/ruby

require 'fileutils'
require 'zip'

class TestCase
    def initialize(filename, priority)
        @filename = filename
        @priority = priority
    end
    attr_reader :filename, :priority
end

testcases = []
Dir.glob("*.txt") do |test_file|
    if (test_file =~ /.*-testcase-custom-.*p([0-9]+)\.txt/)
        testcases << TestCase.new(test_file, $1.to_i)
    else
        STDERR.print "Ignoring file #{test_file}\n"
    end
end

testcases.sort! { |x, y| x.priority <=> y.priority }

EXPORTED_SUBDIR_NAME = "exported"

if not File.exist? EXPORTED_SUBDIR_NAME
    FileUtils.mkdir EXPORTED_SUBDIR_NAME
end

FileUtils.rm_rf(Dir.glob("#{EXPORTED_SUBDIR_NAME}/*"))

testcase_number = 0
input_filenames_to_zip = []
output_filenames_to_zip = []
testcases.each do |testcase|
    input_filename = sprintf("input%02d.txt", testcase_number)
    print "Processing #{testcase.filename} [#{input_filename}] (#{testcase_number + 1}/#{testcases.length})\n"
    FileUtils.cp(testcase.filename, "#{EXPORTED_SUBDIR_NAME}/#{input_filename}")
    output = `../editorial < #{testcase.filename}`
    output_filename = sprintf("output%02d.txt", testcase_number)
    File.open("#{EXPORTED_SUBDIR_NAME}/#{output_filename}", "w") do |output_file|
        output_file.print output
    end

    input_filenames_to_zip << input_filename
    output_filenames_to_zip << output_filename

    testcase_number = testcase_number + 1
end

zipfile_name = "#{EXPORTED_SUBDIR_NAME}/testcases.zip"

Zip::File.open(zipfile_name, Zip::File::CREATE) do |zipfile|
    input_filenames_to_zip.each do |filename|
          zipfile.add("input/#{filename}", "#{EXPORTED_SUBDIR_NAME}/#{filename}")
    end
    output_filenames_to_zip.each do |filename|
          zipfile.add("output/#{filename}", "#{EXPORTED_SUBDIR_NAME}/#{filename}")
    end
end

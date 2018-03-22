#! /usr/bin/ruby

require 'fileutils'
require 'zip'

class TestCase
    def initialize(filename, num_nodes, num_queries)
        @filename = filename
        @num_nodes = num_nodes
        @num_queries = num_queries
    end
    attr_reader :filename, :num_nodes, :num_queries
end

testcases = []
Dir.glob("*.txt") do |test_file|
    if (test_file =~ /move-the-coins2-testcase-custom-([0-9]+)n-([0-9]+)q-.*\.txt/)
        testcases << TestCase.new(test_file, $1.to_i, $2.to_i)
    else
        STDERR.print "Ignoring file #{test_file}\n"
    end
end

testcases.sort! { |x, y| x.num_nodes <=> y.num_nodes }

EXPORTED_SUBDIR_NAME = "exported"

if not File.exist? EXPORTED_SUBDIR_NAME
    FileUtils.mkdir EXPORTED_SUBDIR_NAME
end

FileUtils.rm_rf(Dir.glob("#{EXPORTED_SUBDIR_NAME}/*"))

testcase_number = 0
input_filenames_to_zip = []
output_filenames_to_zip = []
testcases.each do |testcase|
    print "Processing #{testcase.filename} (#{testcase_number + 1}/#{testcases.length})\n"
    input_filename = sprintf("input%02d.txt", testcase_number)
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

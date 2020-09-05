#! /usr/bin/ruby

require "faraday"

STDOUT.sync = true

CONTEST_CODE="FEB20A"

accuracy = nil
num_expch_ac = nil

while true do

    begin
        problem_names = []
        num_submissions_for_problem = {}
        accuracy_for_problem = {}
        html = Faraday.new("https://www.codechef.com/#{CONTEST_CODE}?order=desc&sortBy=successful_submissions", headers: { 'User-Agent' => 'Wget/1.19.4 (linux-gnu)'}).get.body
        #print "html: #{html}"

        print "Time: ", Time.now.strftime("%Y-%m-%d %H:%M:%S"), "\n"

        html.each_line do |line| 
            line.scan(/\"\/\w+\/submit\/(\w+)\".*-&gt; (\d+)\"/).each do |probleminfomatch| 
                problemname=$1
                num_submissions=$2
                problem_names.append(problemname)
                num_submissions_for_problem[problemname] = num_submissions
            end
        end

        problem_names.each do |problemname|
            html.each_line do |line| 
                line.scan(/\/#{CONTEST_CODE}\/status\/#{problemname}">([\d\.]+)<\/a>/).each do |probleminfomatch| 
                    accuracy_for_problem[problemname] = $1.to_f
                end
            end
        end

        problem_names.each do |problemname|
            print "problemname-and-submissions: #{problemname}:#{num_submissions_for_problem[problemname]}:#{accuracy_for_problem[problemname]}\n" 
        end

        if problem_names.include? "CHGORAM2"
            new_accuracy = accuracy_for_problem["CHGORAM2"]
            if not accuracy.nil? and not new_accuracy.nil? and accuracy != new_accuracy
                colour = "green"
                colour = "red" if new_accuracy > accuracy
                system("/home/simon/.bin/notify-done --color=#{colour} 'CHGORAM accuracy changed'")
            end
            accuracy = accuracy_for_problem["CHGORAM2"]
        end

        if problem_names.include? "EXPCH"
            if not num_expch_ac.nil? and num_submissions_for_problem["EXPCH"] != num_expch_ac
                system("/home/simon/.bin/notify-done --color=green 'EXPCH ACs changed'")
            end
            num_expch_ac = num_submissions_for_problem["EXPCH"]
        end

        print "CHGORAM2 accuracy: #{accuracy} num_expch_ac: #{num_expch_ac}\n" unless accuracy.nil? or num_expch_ac.nil?  

    rescue => exception
        STDERR.print "Some exception occurred: #{exception}\n"
    end

    sleep(120)
end


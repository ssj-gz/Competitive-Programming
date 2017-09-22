#! /usr/bin/ruby

running_score = 0
lines_out = []
File.open("TODO.txt", "r") do |todo_in|
    todo_in.each_line do |line|
        next unless line =~ /^(.*?)\s*\((\d+)\).*$/
        running_score += $2.to_i
        lines_out << "#{$1} (#{$2}) [#{running_score}]"
    end
end

File.open("TODO.txt", "w") do |todo_out|
    lines_out.each do |line_out|
        todo_out.print line_out, "\n"
    end
end

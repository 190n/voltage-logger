#!/usr/bin/env ruby
require 'csv'

class String
    def is_number?
        true if Float(self) rescue false
    end
end

class CSVColumnParser
    attr_reader :keys, :dict

    def initialize(rows)
        @rows = rows
        parse!
    end

    def parse!
        @keys = @rows[0]
        @dict = {}
        @keys.each_with_index do |k, i|
            @dict[k] = @rows[1..-1].map { |r| r[i] }
        end
    end

    def unparse!
        rows = 
    end

    def write_to(file)
        CSV.open(file) do |csv|

        end

    def method_missing(m, *args, &block)
        @dict.send(m, *args, &block)
        
    end

    def to_f!
        @dict.each do |k, v|
            v.map! do |n|
                return n.to_f if n.is_number?
                n
            end
        end
    end

    def self.read(file)
        CSVColumnParser.new(CSV.read(file))
    end
end

if ARGV.size < 3
    puts "usage: #{ARGV[0]} infile <col1_nominal [col2_nominal [...colN_nominal]] <outfile>"
    exit
end

infile = ARGV[0]
nominal_voltages = ARGV[1...-1].map(&:to_i)
outfile = ARGV[-1]

data = CSVColumnParser.read(infile)
time_key = (data.keys - nominal_voltage_hash.keys)[0]
data.to_f!

nominal_voltage_hash = Hash[data.keys[1..-1].each_with_index.map { |k, i| [k, nominal_voltages[i]]}]

# first pass: change voltages to fraction of nominal
# e.g a reading of 1.4V on a 1.5V alkaline would be changed to 0.9333
data.each do |k, v|
    return if k == time_key
    v.map! { |n| n / nominal_voltage_hash[k] }
end

# second pass: stretch/shrink voltage curves so that all drain at the same point
def lerp(x1, y1, x2, y2, x)
    return y1 + (x - x1) * ((y2 - y1) / (x2 - x1))
end

data[time_key] = (0..1).step(0.001).to_a

data.each do |k, v|
    return if k == time_key
    a = []
    data[time_key].each do |t|
        return if t == 1
        actual = v.size * t
        x1 = actual.floor
        x2 = actual.ceil
        a << lerp(x1, v[x1], x2, v[x2], actual)
    end
    v = a
end

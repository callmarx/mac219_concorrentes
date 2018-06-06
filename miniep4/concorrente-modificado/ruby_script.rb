
for j in [3000000, 6000000, 9000000, 12000000, 15000000]
  for i in [10, 20, 30, 40, 50]
    bakery_average = 0
    gate_average = 0
    k = 0
    File.open("results/average_SCHED_OTHER-#{i}.#{j}.txt").each do | line |
      if k < 30
        bakery_average += line.split(":")[1].to_f
      else
        gate_average += line.split(":")[1].to_f
      end
      k += 1
    end
    bakery_deviation = 0
    gate_deviation = 0
    k = 0
    File.open("results/deviation_SCHED_OTHER-#{i}.#{j}.txt").each do | line |
      if k < 30
        bakery_deviation += line.split(":")[1].to_f
      else
        gate_deviation += line.split(":")[1].to_f
      end
      k += 1
    end
    puts "SCHED_OTHER-#{i}.#{j}:"
    puts "bakery average: #{bakery_average/30.0}"
    puts "bakery deviation: #{bakery_deviation/30.0}"
    puts "gate average: #{gate_average/30.0}"
    puts "gate deviation: #{gate_deviation/30.0}"
  end
end

for j in 3000000 6000000 9000000 12000000 15000000; do
  for i in 10 20 30 40 50; do
    cat test_SCHED_RR-${i}.${j}.txt | grep "Standard Deviation of accesses" > deviation_SCHED_RR-${i}.${j}.txt
  done
done

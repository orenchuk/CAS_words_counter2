N=5

rm res.txt

for i in 50000 75000 100000
do
    for j in $(seq 1 $N)
    do
        ./counter $i #>> res.txt
    done
echo
done

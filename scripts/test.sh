make build/main

for file in ./graphs/*; do
    # skip 0-k-10 and 10-k-10
    if
        [[ $file == *"/0-k-10" ]] ||
        [[ $file == *"/10-k-10-star" ]] ||
        [[ $file == *"/2-double-tree-depth-3" ]];
    then
        continue
    fi
    echo "Testing $file"
    output1=$(python scripts/flow_poly.py < $file)
    output2=$(./build/main < $file)
    if [ "$output1" != "$output2" ]; then
        echo "Test failed"
        echo "Expected: $output1"
        echo "Got: $output2"
        exit 1
    fi
done
